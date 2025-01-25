#include "encryption.h"
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <QDebug>

static const int PBKDF2_ITERATIONS = 10000;
static const int AES_KEY_SIZE = 32;

Encryption::Encryption(const QByteArray &baseKey) : baseKey(baseKey)
{
}

QByteArray Encryption::deriveKeyPBKDF2(const QByteArray &baseKey, const QByteArray &entrySalt)
{
    QByteArray outKey;
    outKey.resize(AES_KEY_SIZE);

    int result = PKCS5_PBKDF2_HMAC(
        baseKey.constData(),
        baseKey.size(),
        reinterpret_cast<const unsigned char*>(entrySalt.constData()),
        entrySalt.size(),
        PBKDF2_ITERATIONS,
        EVP_sha256(),
        AES_KEY_SIZE,
        reinterpret_cast<unsigned char*>(outKey.data())
    );
    if (result != 1) {
        qWarning() << "Failed to derive PBKDF2 key!";
        return QByteArray();
    }
    return outKey;
}

QByteArray Encryption::deriveKeyFromPassword(const QString &password, const QByteArray &userSalt)
{
    QByteArray outKey;
    outKey.resize(AES_KEY_SIZE);

    QByteArray passBytes = password.toUtf8();

    int result = PKCS5_PBKDF2_HMAC(
        passBytes.constData(),
        passBytes.size(),
        reinterpret_cast<const unsigned char*>(userSalt.constData()),
        userSalt.size(),
        PBKDF2_ITERATIONS,
        EVP_sha256(),
        AES_KEY_SIZE,
        reinterpret_cast<unsigned char*>(outKey.data())
    );

    if (result != 1) {
        qWarning() << "Failed to derive user base key from password!";
        return QByteArray();
    }
    return outKey;
}

QByteArray Encryption::aesEncrypt(const QByteArray &plain, const QByteArray &key, QByteArray &ivOut)
{
    ivOut.resize(AES_BLOCK_SIZE);
    if (RAND_bytes(reinterpret_cast<unsigned char*>(ivOut.data()), AES_BLOCK_SIZE) != 1) {
        qWarning() << "Failed to generate IV!";
        return QByteArray();
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr,
                       reinterpret_cast<const unsigned char*>(key.constData()),
                       reinterpret_cast<const unsigned char*>(ivOut.constData()));

    QByteArray cipher;
    cipher.resize(plain.size() + AES_BLOCK_SIZE);

    int len = 0;
    if (!EVP_EncryptUpdate(ctx,
        reinterpret_cast<unsigned char*>(cipher.data()), &len,
        reinterpret_cast<const unsigned char*>(plain.constData()), plain.size())) {
        qWarning() << "EncryptUpdate failed!";
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }
    int totalLen = len;

    if (!EVP_EncryptFinal_ex(ctx,
        reinterpret_cast<unsigned char*>(cipher.data()) + totalLen, &len)) {
        qWarning() << "EncryptFinal failed!";
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }
    totalLen += len;
    EVP_CIPHER_CTX_free(ctx);

    cipher.resize(totalLen);

    return cipher;
}

QByteArray Encryption::aesDecrypt(const QByteArray &cipher, const QByteArray &key)
{
    if (cipher.size() < AES_BLOCK_SIZE) {
        qWarning() << "Ciphertext too short!";
        return QByteArray();
    }
    QByteArray iv = cipher.left(AES_BLOCK_SIZE);
    QByteArray actualCipher = cipher.mid(AES_BLOCK_SIZE);

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr,
                       reinterpret_cast<const unsigned char*>(key.constData()),
                       reinterpret_cast<const unsigned char*>(iv.constData()));

    QByteArray plain;
    plain.resize(actualCipher.size());

    int len = 0;
    if (!EVP_DecryptUpdate(ctx,
        reinterpret_cast<unsigned char*>(plain.data()), &len,
        reinterpret_cast<const unsigned char*>(actualCipher.constData()), actualCipher.size())) {
        qWarning() << "DecryptUpdate failed!";
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }

    int totalLen = len;
    if (!EVP_DecryptFinal_ex(ctx,
        reinterpret_cast<unsigned char*>(plain.data()) + totalLen, &len)) {
        qWarning() << "DecryptFinal failed!";
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }
    totalLen += len;
    EVP_CIPHER_CTX_free(ctx);

    plain.resize(totalLen);
    return plain;
}

QByteArray Encryption::encryptWithSalt(const QString &plaintext, const QByteArray &entrySalt) const
{
    if (plaintext.isEmpty()) {
        return QByteArray();
    }
    QByteArray finalKey = deriveKeyPBKDF2(this->baseKey, entrySalt);
    if (finalKey.isEmpty()) {
        return QByteArray();
    }

    QByteArray iv;
    QByteArray cipher = aesEncrypt(plaintext.toUtf8(), finalKey, iv);

    return iv + cipher;
}

QString Encryption::decryptWithSalt(const QByteArray &ciphertext, const QByteArray &entrySalt) const
{
    if (ciphertext.isEmpty()) {
        return QString();
    }

    QByteArray finalKey = deriveKeyPBKDF2(this->baseKey, entrySalt);
    if (finalKey.isEmpty()) {
        return QString();
    }

    QByteArray plain = aesDecrypt(ciphertext, finalKey);
    return QString::fromUtf8(plain);
}

QByteArray Encryption::encrypt(const QString &plaintext) const
{
    QByteArray iv;
    return iv + aesEncrypt(plaintext.toUtf8(), baseKey, iv);
}

QString Encryption::decrypt(const QByteArray &ciphertext) const
{
    if (ciphertext.isEmpty()) {
        return QString();
    }

    QByteArray plain = aesDecrypt(ciphertext, baseKey);
    return QString::fromUtf8(plain);
}
