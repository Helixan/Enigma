#ifndef USER_H
#define USER_H

#include <QString>

class User {
public:
    User(int id, const QString &username);

    int getId() const;

    QString getUsername() const;

    static bool registerUser(const QString &username, const QString &password);

    static User *login(const QString &username, const QString &password);

private:
    int id;
    QString username;

    static QString hashPassword(const QString &password);
};

#endif // USER_H
