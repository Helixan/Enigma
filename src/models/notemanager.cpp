#include "notemanager.h"
#include "core/dbmanager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

NoteManager::NoteManager(int userId, Encryption* encryption)
    : userId(userId)
    , encryption(encryption)
{
}

bool NoteManager::addNote(const NoteEntry& entry)
{
    QSqlDatabase db = DBManager::instance().getDatabase();
    QSqlQuery query(db);

    QByteArray encTitle   = encryption->encrypt(entry.title);
    QByteArray encContent = encryption->encrypt(entry.content);

    query.prepare(R"(
        INSERT INTO notes (user_id, encrypted_title, encrypted_content)
        VALUES (?, ?, ?)
    )");
    query.addBindValue(userId);
    query.addBindValue(encTitle);
    query.addBindValue(encContent);

    if (!query.exec()) {
        qDebug() << "Add Note Error:" << query.lastError().text();
        return false;
    }
    return true;
}

bool NoteManager::updateNote(int id, const NoteEntry& entry)
{
    QSqlDatabase db = DBManager::instance().getDatabase();
    QSqlQuery query(db);

    QByteArray encTitle   = encryption->encrypt(entry.title);
    QByteArray encContent = encryption->encrypt(entry.content);

    query.prepare(R"(
        UPDATE notes
        SET
          encrypted_title = ?,
          encrypted_content = ?
        WHERE id = ? AND user_id = ?
    )");

    query.addBindValue(encTitle);
    query.addBindValue(encContent);
    query.addBindValue(id);
    query.addBindValue(userId);

    if (!query.exec()) {
        qDebug() << "Update Note Error:" << query.lastError().text();
        return false;
    }
    return (query.numRowsAffected() > 0);
}

QList<NoteEntry> NoteManager::getNotes()
{
    QList<NoteEntry> list;
    QSqlDatabase db = DBManager::instance().getDatabase();
    QSqlQuery query(db);

    query.prepare(R"(
        SELECT
          id,
          encrypted_title,
          encrypted_content
        FROM notes
        WHERE user_id = ?
    )");
    query.addBindValue(userId);

    if (query.exec()) {
        while (query.next()) {
            NoteEntry entry;
            entry.id = query.value(0).toInt();

            QByteArray encTitle   = query.value(1).toByteArray();
            QByteArray encContent = query.value(2).toByteArray();

            entry.title   = encryption->decrypt(encTitle);
            entry.content = encryption->decrypt(encContent);

            list.append(entry);
        }
    } else {
        qDebug() << "Get Notes Error:" << query.lastError().text();
    }

    return list;
}

bool NoteManager::deleteNote(int id)
{
    QSqlDatabase db = DBManager::instance().getDatabase();
    QSqlQuery query(db);

    query.prepare("DELETE FROM notes WHERE id = ? AND user_id = ?");
    query.addBindValue(id);
    query.addBindValue(userId);

    if (!query.exec()) {
        qDebug() << "Delete Note Error:" << query.lastError().text();
        return false;
    }
    return (query.numRowsAffected() > 0);
}
