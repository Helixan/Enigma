#include "passwordmanager.h"
#include "core/dbmanager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

PasswordManager::PasswordManager(const int userId, Encryption *encryption)
    : userId(userId)
      , encryption(encryption) {
}

Encryption *PasswordManager::getEncryption() const {
    return encryption;
}

bool PasswordManager::addPassword(const PasswordEntry &entry) const {
    const QSqlDatabase db = DBManager::instance().getDatabase();
    QSqlQuery query(db);

    const QByteArray encService = encryption->encrypt(entry.service);
    const QByteArray encUrl = encryption->encrypt(entry.url);
    const QByteArray encUsername = encryption->encrypt(entry.username);
    const QByteArray encEmail = encryption->encrypt(entry.email);
    const QByteArray encPassword = encryption->encrypt(entry.password);
    const QByteArray encDescription = encryption->encrypt(entry.description);
    const QByteArray encTotp = encryption->encrypt(entry.totpSecret);

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

bool PasswordManager::updatePassword(const int id, const PasswordEntry &entry) const {
    const QSqlDatabase db = DBManager::instance().getDatabase();
    QSqlQuery query(db);

    const QByteArray encService = encryption->encrypt(entry.service);
    const QByteArray encUrl = encryption->encrypt(entry.url);
    const QByteArray encUsername = encryption->encrypt(entry.username);
    const QByteArray encEmail = encryption->encrypt(entry.email);
    const QByteArray encPassword = encryption->encrypt(entry.password);
    const QByteArray encDescription = encryption->encrypt(entry.description);
    const QByteArray encTotp = encryption->encrypt(entry.totpSecret);

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
    return query.numRowsAffected() > 0;
}

QList<PasswordEntry> PasswordManager::getPasswords() const {
    QList<PasswordEntry> list;
    const QSqlDatabase db = DBManager::instance().getDatabase();
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

            entry.service = encryption->decrypt(query.value(1).toByteArray());
            entry.url = encryption->decrypt(query.value(2).toByteArray());
            entry.username = encryption->decrypt(query.value(3).toByteArray());
            entry.email = encryption->decrypt(query.value(4).toByteArray());
            entry.password = encryption->decrypt(query.value(5).toByteArray());
            entry.description = encryption->decrypt(query.value(6).toByteArray());
            entry.totpSecret = encryption->decrypt(query.value(7).toByteArray());

            list.append(entry);
        }
    } else {
        qDebug() << "Get Passwords Error:" << query.lastError().text();
    }

    return list;
}

bool PasswordManager::deletePassword(const int id) const {
    const QSqlDatabase db = DBManager::instance().getDatabase();
    QSqlQuery query(db);

    query.prepare("DELETE FROM passwords WHERE id = ? AND user_id = ?");
    query.addBindValue(id);
    query.addBindValue(userId);

    if (!query.exec()) {
        qDebug() << "Delete Password Error:" << query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;
}
