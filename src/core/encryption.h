#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <QString>
#include <QByteArray>

class Encryption {
public:
    explicit Encryption(const QByteArray &baseKey);

    QByteArray encryptWithSalt(const QString &plaintext, const QByteArray &entrySalt) const;

    QString decryptWithSalt(const QByteArray &ciphertext, const QByteArray &entrySalt) const;

    QByteArray encrypt(const QString &plaintext) const;

    QString decrypt(const QByteArray &ciphertext) const;

    static QByteArray deriveKeyFromPassword(const QString &password, const QByteArray &userSalt);

private:
    QByteArray baseKey;

    static QByteArray deriveKeyPBKDF2(const QByteArray &baseKey, const QByteArray &entrySalt);

    static QByteArray aesEncrypt(const QByteArray &plain, const QByteArray &key, QByteArray &ivOut);

    static QByteArray aesDecrypt(const QByteArray &cipher, const QByteArray &key);
};

#endif // ENCRYPTION_H
