#include "encryption.h"
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <QDebug>

Encryption::Encryption(const QByteArray &key) {
    if (key.size() < 32) {
        this->key = key.leftJustified(32, '0');
    } else {
        this->key = key.left(32);
    }
}

QByteArray Encryption::encrypt(const QString &plaintext) const {
    const QByteArray data = plaintext.toUtf8();
    QByteArray ciphertext;
    ciphertext.resize(data.size() + AES_BLOCK_SIZE);

    int out_len1 = 0;
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr,
                       reinterpret_cast<const unsigned char *>(key.constData()),
                       reinterpret_cast<const unsigned char *>(key.constData()));

    EVP_EncryptUpdate(ctx,
                      reinterpret_cast<unsigned char *>(ciphertext.data()),
                      &out_len1,
                      reinterpret_cast<const unsigned char *>(data.constData()),
                      data.size());

    int out_len2 = 0;
    EVP_EncryptFinal_ex(ctx,
                        reinterpret_cast<unsigned char *>(ciphertext.data()) + out_len1,
                        &out_len2);

    EVP_CIPHER_CTX_free(ctx);
    ciphertext.resize(out_len1 + out_len2);
    return ciphertext;
}

QString Encryption::decrypt(const QByteArray &ciphertext) const {
    QByteArray decrypted;
    decrypted.resize(ciphertext.size());

    int out_len1 = 0;
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr,
                       reinterpret_cast<const unsigned char *>(key.constData()),
                       reinterpret_cast<const unsigned char *>(key.constData()));

    EVP_DecryptUpdate(ctx,
                      reinterpret_cast<unsigned char *>(decrypted.data()),
                      &out_len1,
                      reinterpret_cast<const unsigned char *>(ciphertext.constData()),
                      ciphertext.size());

    int out_len2 = 0;
    EVP_DecryptFinal_ex(ctx,
                        reinterpret_cast<unsigned char *>(decrypted.data()) + out_len1,
                        &out_len2);

    EVP_CIPHER_CTX_free(ctx);
    decrypted.resize(out_len1 + out_len2);
    return QString::fromUtf8(decrypted);
}
