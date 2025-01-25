#include "passwordmanager.h"
#include "core/dbmanager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <openssl/rand.h>

static QByteArray generateRandomSalt(int length = 16) {
    QByteArray salt;
    salt.resize(length);
    if (RAND_bytes(reinterpret_cast<unsigned char *>(salt.data()), length) != 1) {
        qWarning() << "Failed to generate random salt!";
        salt.fill(0);
    }
    return salt;
}

PasswordManager::PasswordManager(int userId, Encryption *encryption)
    : userId(userId), encryption(encryption) {
}

Encryption *PasswordManager::getEncryption() const {
    return encryption;
}

bool PasswordManager::addPassword(const PasswordEntry &entry) const {
    if (!encryption) {
        qWarning() << "No encryption object available!";
        return false;
    }

    QByteArray entrySalt = generateRandomSalt(16);

    QByteArray encService = encryption->encryptWithSalt(entry.service, entrySalt);
    QByteArray encUrl = encryption->encryptWithSalt(entry.url, entrySalt);
    QByteArray encUsername = encryption->encryptWithSalt(entry.username, entrySalt);
    QByteArray encEmail = encryption->encryptWithSalt(entry.email, entrySalt);
    QByteArray encPassword = encryption->encryptWithSalt(entry.password, entrySalt);
    QByteArray encDescription = encryption->encryptWithSalt(entry.description, entrySalt);
    QByteArray encTotp = encryption->encryptWithSalt(entry.totpSecret, entrySalt);

    QSqlDatabase db = DBManager::instance().getDatabase();
    QSqlQuery query(db);
    query.prepare(R"(
        INSERT INTO passwords (
            user_id,
            salt,
            encrypted_service,
            encrypted_url,
            encrypted_username,
            encrypted_email,
            encrypted_password,
            encrypted_description,
            encrypted_totp_secret
        ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)
    )");
    query.addBindValue(userId);
    query.addBindValue(entrySalt);
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

bool PasswordManager::updatePassword(int id, const PasswordEntry &entry) const {
    if (!encryption) {
        return false;
    }

    QByteArray entrySalt = generateRandomSalt(16);

    QByteArray encService = encryption->encryptWithSalt(entry.service, entrySalt);
    QByteArray encUrl = encryption->encryptWithSalt(entry.url, entrySalt);
    QByteArray encUsername = encryption->encryptWithSalt(entry.username, entrySalt);
    QByteArray encEmail = encryption->encryptWithSalt(entry.email, entrySalt);
    QByteArray encPassword = encryption->encryptWithSalt(entry.password, entrySalt);
    QByteArray encDescription = encryption->encryptWithSalt(entry.description, entrySalt);
    QByteArray encTotp = encryption->encryptWithSalt(entry.totpSecret, entrySalt);

    QSqlDatabase db = DBManager::instance().getDatabase();
    QSqlQuery query(db);
    query.prepare(R"(
        UPDATE passwords
        SET
            salt = ?,
            encrypted_service = ?,
            encrypted_url = ?,
            encrypted_username = ?,
            encrypted_email = ?,
            encrypted_password = ?,
            encrypted_description = ?,
            encrypted_totp_secret = ?
        WHERE id = ? AND user_id = ?
    )");

    query.addBindValue(entrySalt);
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

QList<PasswordEntry> PasswordManager::getPasswords() const {
    QList<PasswordEntry> list;
    if (!encryption) {
        return list;
    }
    QSqlDatabase db = DBManager::instance().getDatabase();
    QSqlQuery query(db);

    query.prepare(R"(
        SELECT
            id,
            salt,
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
            entry.salt = query.value(1).toByteArray();

            QByteArray encService = query.value(2).toByteArray();
            QByteArray encUrl = query.value(3).toByteArray();
            QByteArray encUsername = query.value(4).toByteArray();
            QByteArray encEmail = query.value(5).toByteArray();
            QByteArray encPassword = query.value(6).toByteArray();
            QByteArray encDescription = query.value(7).toByteArray();
            QByteArray encTotp = query.value(8).toByteArray();

            entry.service = encryption->decryptWithSalt(encService, entry.salt);
            entry.url = encryption->decryptWithSalt(encUrl, entry.salt);
            entry.username = encryption->decryptWithSalt(encUsername, entry.salt);
            entry.email = encryption->decryptWithSalt(encEmail, entry.salt);
            entry.password = encryption->decryptWithSalt(encPassword, entry.salt);
            entry.description = encryption->decryptWithSalt(encDescription, entry.salt);
            entry.totpSecret = encryption->decryptWithSalt(encTotp, entry.salt);

            list.append(entry);
        }
    } else {
        qDebug() << "Get Passwords Error:" << query.lastError().text();
    }
    return list;
}

bool PasswordManager::deletePassword(int id) const {
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
