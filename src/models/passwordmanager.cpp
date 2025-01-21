#include "passwordmanager.h"
#include "core/dbmanager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

PasswordManager::PasswordManager(int userId, Encryption* encryption)
    : userId(userId)
    , encryption(encryption)
{
}

Encryption* PasswordManager::getEncryption()
{
    return encryption;
}

bool PasswordManager::addPassword(const PasswordEntry& entry)
{
    QSqlDatabase db = DBManager::instance().getDatabase();
    QSqlQuery query(db);

    QByteArray encService     = encryption->encrypt(entry.service);
    QByteArray encUrl         = encryption->encrypt(entry.url);
    QByteArray encUsername    = encryption->encrypt(entry.username);
    QByteArray encEmail       = encryption->encrypt(entry.email);
    QByteArray encPassword    = encryption->encrypt(entry.password);
    QByteArray encDescription = encryption->encrypt(entry.description);
    QByteArray encTotp        = encryption->encrypt(entry.totpSecret);

    query.prepare(R"(
        INSERT INTO passwords (
          user_id,
          encrypted_service,
          encrypted_url,
          encrypted_username,
          encrypted_email,
          encrypted_password,
          encrypted_description,
          encrypted_totp_secret
        ) VALUES (?, ?, ?, ?, ?, ?, ?, ?)
    )");
    query.addBindValue(userId);
    query.addBindValue(encService);
    query.addBindValue(encUrl);
    query.addBindValue(encUsername);
    query.addBindValue(encEmail);
    query.addBindValue(encPassword);
    query.addBindValue(encDescription);
    query.addBindValue(encTotp);

    if (!query.exec()) {
        qDebug() << "Add Password Error:" << query.lastError().text();
        return false;
    }
    return true;
}

bool PasswordManager::updatePassword(int id, const PasswordEntry& entry)
{
    QSqlDatabase db = DBManager::instance().getDatabase();
    QSqlQuery query(db);

    QByteArray encService     = encryption->encrypt(entry.service);
    QByteArray encUrl         = encryption->encrypt(entry.url);
    QByteArray encUsername    = encryption->encrypt(entry.username);
    QByteArray encEmail       = encryption->encrypt(entry.email);
    QByteArray encPassword    = encryption->encrypt(entry.password);
    QByteArray encDescription = encryption->encrypt(entry.description);
    QByteArray encTotp        = encryption->encrypt(entry.totpSecret);

    query.prepare(R"(
        UPDATE passwords
        SET
          encrypted_service = ?,
          encrypted_url = ?,
          encrypted_username = ?,
          encrypted_email = ?,
          encrypted_password = ?,
          encrypted_description = ?,
          encrypted_totp_secret = ?
        WHERE id = ? AND user_id = ?
    )");

    query.addBindValue(encService);
    query.addBindValue(encUrl);
    query.addBindValue(encUsername);
    query.addBindValue(encEmail);
    query.addBindValue(encPassword);
    query.addBindValue(encDescription);
    query.addBindValue(encTotp);

    query.addBindValue(id);
    query.addBindValue(userId);

    if (!query.exec()) {
        qDebug() << "Update Password Error:" << query.lastError().text();
        return false;
    }
    return (query.numRowsAffected() > 0);
}

QList<PasswordEntry> PasswordManager::getPasswords()
{
    QList<PasswordEntry> list;
    QSqlDatabase db = DBManager::instance().getDatabase();
    QSqlQuery query(db);

    query.prepare(R"(
        SELECT
          id,
          encrypted_service,
          encrypted_url,
          encrypted_username,
          encrypted_email,
          encrypted_password,
          encrypted_description,
          encrypted_totp_secret
        FROM passwords
        WHERE user_id = ?
    )");
    query.addBindValue(userId);

    if (query.exec()) {
        while (query.next()) {
            PasswordEntry entry;
            entry.id = query.value(0).toInt();

            entry.service     = encryption->decrypt(query.value(1).toByteArray());
            entry.url         = encryption->decrypt(query.value(2).toByteArray());
            entry.username    = encryption->decrypt(query.value(3).toByteArray());
            entry.email       = encryption->decrypt(query.value(4).toByteArray());
            entry.password    = encryption->decrypt(query.value(5).toByteArray());
            entry.description = encryption->decrypt(query.value(6).toByteArray());
            entry.totpSecret  = encryption->decrypt(query.value(7).toByteArray());

            list.append(entry);
        }
    } else {
        qDebug() << "Get Passwords Error:" << query.lastError().text();
    }

    return list;
}

bool PasswordManager::deletePassword(int id)
{
    QSqlDatabase db = DBManager::instance().getDatabase();
    QSqlQuery query(db);

    query.prepare("DELETE FROM passwords WHERE id = ? AND user_id = ?");
    query.addBindValue(id);
    query.addBindValue(userId);

    if (!query.exec()) {
        qDebug() << "Delete Password Error:" << query.lastError().text();
        return false;
    }
    return (query.numRowsAffected() > 0);
}
