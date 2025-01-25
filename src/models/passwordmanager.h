#ifndef PASSWORDMANAGER_H
#define PASSWORDMANAGER_H

#include <QList>
#include <QByteArray>
#include "core/encryption.h"

struct PasswordEntry {
    int id;
    QByteArray salt;
    QString service;
    QString url;
    QString username;
    QString email;
    QString password;
    QString description;
    QString totpSecret;
};

class PasswordManager {
public:
    PasswordManager(int userId, Encryption *encryption);

    bool addPassword(const PasswordEntry &entry) const;

    bool updatePassword(int id, const PasswordEntry &entry) const;

    QList<PasswordEntry> getPasswords() const;

    bool deletePassword(int id) const;

    Encryption *getEncryption() const;

private:
    int userId;
    Encryption *encryption;
};

#endif // PASSWORDMANAGER_H
