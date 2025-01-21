#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QtSql>

class DBManager {
public:
    static DBManager &instance();

    bool openConnection(const QString &host, const QString &dbName, const QString &user, const QString &password);

    QSqlDatabase getDatabase();

    void closeConnection();

private:
    DBManager() {
    }

    ~DBManager();

    DBManager(const DBManager &) = delete;

    DBManager &operator=(const DBManager &) = delete;

    QSqlDatabase db;
};

#endif // DBMANAGER_H
