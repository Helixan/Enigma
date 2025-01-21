#include "user.h"
#include "core/dbmanager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QCryptographicHash>
#include <QDebug>

User::User(const int id, const QString &username)
    : id(id), username(username) {
}

int User::getId() const {
    return id;
}

QString User::getUsername() const {
    return username;
}

QString User::hashPassword(const QString &password) {
    const QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    return QString(hash.toHex());
}

bool User::registerUser(const QString &username, const QString &password) {
    if (username.isEmpty() || password.isEmpty()) {
        return false;
    }

    const QSqlDatabase db = DBManager::instance().getDatabase(); {
        QSqlQuery checkQuery(db);
        checkQuery.prepare("SELECT COUNT(*) FROM users WHERE LOWER(username) = LOWER(?)");
        checkQuery.addBindValue(username);
        if (!checkQuery.exec() || !checkQuery.next()) {
            qDebug() << "Error checking username availability:" << checkQuery.lastError().text();
            return false;
        }
        if (checkQuery.value(0).toInt() > 0) {
            return false;
        }
    }

    const QString sha256Hash = hashPassword(password);
    if (sha256Hash.isEmpty()) {
        return false;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO users (username, password) VALUES (?, ?)");
    query.addBindValue(username.toLower());
    query.addBindValue(sha256Hash);

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

    const QSqlDatabase db = DBManager::instance().getDatabase();
    QSqlQuery query(db);

    query.prepare("SELECT id, username, password FROM users WHERE LOWER(username) = LOWER(?)");
    query.addBindValue(username);

    if (!query.exec()) {
        qDebug() << "Login Error (exec fail):" << query.lastError().text();
        return nullptr;
    }

    if (!query.next()) {
        return nullptr;
    }

    const int id = query.value(0).toInt();
    const QString uname = query.value(1).toString();
    const QString storedHash = query.value(2).toString();

    if (const QString inputHash = hashPassword(password); inputHash == storedHash) {
        return new User(id, uname);
    }

    return nullptr;
}
