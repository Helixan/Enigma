#include "dbmanager.h"
#include <QDebug>

DBManager &DBManager::instance() {
    static DBManager instance;
    return instance;
}

bool DBManager::openConnection(const QString &host, const QString &dbName, const QString &user,
                               const QString &password) {
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(host);
    db.setDatabaseName(dbName);
    db.setUserName(user);
    db.setPassword(password);

    if (!db.open()) {
        qDebug() << "Database Error:" << db.lastError().text();
        return false;
    }
    return true;
}

QSqlDatabase DBManager::getDatabase() {
    return db;
}

void DBManager::closeConnection() {
    if (db.isOpen()) {
        db.close();
    }
}

DBManager::~DBManager() {
    closeConnection();
}
