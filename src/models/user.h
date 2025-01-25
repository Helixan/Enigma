#ifndef USER_H
#define USER_H

#include <QString>
#include <QByteArray>

class User {
public:
    User(int id, const QString &username, const QByteArray &salt);

    int getId() const;

    QString getUsername() const;

    QByteArray getSalt() const;

    static bool registerUser(const QString &username, const QString &password);

    static User *login(const QString &username, const QString &password);

private:
    int id;
    QString username;
    QByteArray salt;

    static QByteArray generateRandomSalt(int length = 16);

    static QString hashPassword(const QString &password, const QByteArray &salt);
};

#endif // USER_H
