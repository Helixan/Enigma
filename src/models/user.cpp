#include "user.h"
#include "core/dbmanager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QCryptographicHash>
#include <QDebug>
#include <openssl/rand.h>

User::User(int id, const QString &username, const QByteArray &salt)
    : id(id), username(username), salt(salt) {
}

int User::getId() const {
    return id;
}

QString User::getUsername() const {
    return username;
}

QByteArray User::getSalt() const {
    return salt;
}

QByteArray User::generateRandomSalt(int length) {
    QByteArray salt;
    salt.resize(length);
    if (RAND_bytes(reinterpret_cast<unsigned char *>(salt.data()), length) != 1) {
        qWarning() << "Failed to generate random salt!";
        salt.fill(0);
    }
    return salt;
}

QString User::hashPassword(const QString &password, const QByteArray &salt) {
    QCryptographicHash hasher(QCryptographicHash::Sha256);
    hasher.addData(salt);
    hasher.addData(password.toUtf8());
    return hasher.result().toHex();
}

bool User::registerUser(const QString &username, const QString &password) {
    if (username.isEmpty() || password.isEmpty()) {
        return false;
    }

    QSqlDatabase db = DBManager::instance().getDatabase(); {
        QSqlQuery checkQuery(db);
        checkQuery.prepare("SELECT COUNT(*) FROM users WHERE LOWER(username) = LOWER(?)");
        checkQuery.addBindValue(username);
        if (!checkQuery.exec() || !checkQuery.next()) {
            qDebug() << "Error checking username:" << checkQuery.lastError().text();
            return false;
        }
        if (checkQuery.value(0).toInt() > 0) {
            return false;
        }
    }

    QByteArray salt = generateRandomSalt(16);
    QString saltedHash = hashPassword(password, salt);

    QSqlQuery query(db);
    query.prepare("INSERT INTO users (username, password, salt) VALUES (?, ?, ?)");
    query.addBindValue(username.toLower());
    query.addBindValue(saltedHash);
    query.addBindValue(salt);
    if (!query.exec()) {
        qDebug() << "Register Error:" << query.lastError().text();
        return false;
    }
    return true;
}

User *User::login(const QString &username, const QString &password) {
    if (username.isEmpty() || password.isEmpty()) {
        return nullptr;
    }

    QSqlDatabase db = DBManager::instance().getDatabase();
    QSqlQuery query(db);

    query.prepare("SELECT id, username, password, salt FROM users WHERE LOWER(username) = LOWER(?)");
    query.addBindValue(username);

    if (!query.exec()) {
        qDebug() << "Login Error (exec fail):" << query.lastError().text();
        return nullptr;
    }

    if (!query.next()) {
        return nullptr;
    }

    int id = query.value(0).toInt();
    QString uname = query.value(1).toString();
    QString storedHash = query.value(2).toString();
    QByteArray storedSalt = query.value(3).toByteArray();

    QString inputHash = hashPassword(password, storedSalt);
    if (inputHash == storedHash) {
        return new User(id, uname, storedSalt);
    }

    return nullptr;
}
