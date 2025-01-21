#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <QString>
#include <QByteArray>

class Encryption
{
public:
    Encryption(const QByteArray& key);
    QByteArray encrypt(const QString& plaintext) const;
    QString decrypt(const QByteArray& ciphertext) const;

private:
    QByteArray key;
};

#endif // ENCRYPTION_H
