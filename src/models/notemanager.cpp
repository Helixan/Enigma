#include "notemanager.h"
#include "core/dbmanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <openssl/rand.h>

NoteManager::NoteManager(int userId, Encryption *encryption)
    : userId(userId), encryption(encryption) {
}

QByteArray NoteManager::generateRandomSalt(int length) {
    QByteArray salt;
    salt.resize(length);
    if (RAND_bytes(reinterpret_cast<unsigned char *>(salt.data()), length) != 1) {
        qWarning() << "Failed to generate random salt!";
        salt.fill(0);
    }
    return salt;
}

bool NoteManager::addNote(const NoteEntry &entry) const {
    if (!encryption) {
        qWarning() << "No encryption object available!";
        return false;
    }

    QByteArray entrySalt = generateRandomSalt(16);
    QByteArray encTitle = encryption->encryptWithSalt(entry.title, entrySalt);
    QByteArray encContent = encryption->encryptWithSalt(entry.content, entrySalt);

    QSqlDatabase db = DBManager::instance().getDatabase();
    QSqlQuery query(db);

    query.prepare(R"(
        INSERT INTO notes (
            user_id,
            salt,
            encrypted_title,
            encrypted_content
        ) VALUES (?, ?, ?, ?)
    )");
    query.addBindValue(userId);
    query.addBindValue(entrySalt);
    query.addBindValue(encTitle);
    query.addBindValue(encContent);

    if (!query.exec()) {
        qDebug() << "Add Note Error:" << query.lastError().text();
        return false;
    }
    return true;
}

bool NoteManager::updateNote(int id, const NoteEntry &entry) const {
    if (!encryption) {
        return false;
    }

    QByteArray entrySalt = generateRandomSalt(16);
    QByteArray encTitle = encryption->encryptWithSalt(entry.title, entrySalt);
    QByteArray encContent = encryption->encryptWithSalt(entry.content, entrySalt);

    QSqlDatabase db = DBManager::instance().getDatabase();
    QSqlQuery query(db);
    query.prepare(R"(
        UPDATE notes
        SET
            salt = ?,
            encrypted_title = ?,
            encrypted_content = ?
        WHERE id = ? AND user_id = ?
    )");

    query.addBindValue(entrySalt);
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

QList<NoteEntry> NoteManager::getNotes() const {
    QList<NoteEntry> list;
    if (!encryption) {
        return list;
    }

    QSqlDatabase db = DBManager::instance().getDatabase();
    QSqlQuery query(db);

    query.prepare(R"(
        SELECT
            id,
            salt,
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
            entry.salt = query.value(1).toByteArray();

            QByteArray encTitle = query.value(2).toByteArray();
            QByteArray encContent = query.value(3).toByteArray();

            entry.title = encryption->decryptWithSalt(encTitle, entry.salt);
            entry.content = encryption->decryptWithSalt(encContent, entry.salt);

            list.append(entry);
        }
    } else {
        qDebug() << "Get Notes Error:" << query.lastError().text();
    }
    return list;
}

bool NoteManager::deleteNote(int id) const {
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
