#ifndef PASSWORDMANAGER_H
#define PASSWORDMANAGER_H

#include <QList>
#include "core/encryption.h"

struct PasswordEntry {
    int id;
    QString service;
    QString url;
    QString username;
    QString email;
    QString password;
    QString description;
    QString totpSecret;
};

class PasswordManager
{
public:
    PasswordManager(int userId, Encryption* encryption);

    bool addPassword(const PasswordEntry& entry);
    bool updatePassword(int id, const PasswordEntry& entry);
    QList<PasswordEntry> getPasswords();
    bool deletePassword(int id);

    Encryption* getEncryption();

private:
    int userId;
    Encryption* encryption;
};

#endif // PASSWORDMANAGER_H
