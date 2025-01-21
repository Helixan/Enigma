#include "totpgenerator.h"

#include <ctime>
#include <cmath>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <QByteArray>
#include <QString>
#include <QStringBuilder>
#include <QDebug>
#include <iomanip>
#include <sstream>

static int base32CharValue(unsigned char c)
{
    if (c >= 'A' && c <= 'Z')
        return c - 'A';
    if (c >= '2' && c <= '7')
        return c - '2' + 26;
    return -1;
}

QByteArray TOTPGenerator::base32Decode(const QString& base32)
{
    QByteArray output;
    unsigned long buffer = 0;
    int bitsLeft = 0;

    for (QChar qc : base32) {
        if (qc == '=') {
            break;
        }
        unsigned char c = qc.toUpper().toLatin1();
        int val = base32CharValue(c);
        if (val < 0) {
            continue;
        }

        buffer = (buffer << 5) | val;
        bitsLeft += 5;

        if (bitsLeft >= 8) {
            bitsLeft -= 8;
            unsigned char byte = (buffer >> bitsLeft) & 0xFF;
            output.append(char(byte));
        }
    }
    return output;
}

QString TOTPGenerator::generateTOTP(const QString &base32_secret,
                                    int digits,
                                    int time_step,
                                    int t0)
{
    QByteArray key = base32Decode(base32_secret);

    std::time_t current_time = std::time(nullptr);

    long counter = (current_time - t0) / time_step;

    unsigned char counter_bytes[8];
    for (int i = 7; i >= 0; --i) {
        counter_bytes[i] = (unsigned char)(counter & 0xFF);
        counter >>= 8;
    }

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int len = 0;

    HMAC_CTX* ctx = HMAC_CTX_new();
    HMAC_Init_ex(ctx, key.constData(), key.size(), EVP_sha1(), nullptr);
    HMAC_Update(ctx, counter_bytes, 8);
    HMAC_Final(ctx, hash, &len);
    HMAC_CTX_free(ctx);

    int offset = hash[len - 1] & 0x0F;
    int binary =
        ((hash[offset] & 0x7F) << 24) |
        ((hash[offset + 1] & 0xFF) << 16) |
        ((hash[offset + 2] & 0xFF) << 8)  |
         (hash[offset + 3] & 0xFF);

    int otp = binary % (int)std::pow(10, digits);

    std::ostringstream oss;
    oss << std::setw(digits) << std::setfill('0') << otp;
    return QString::fromStdString(oss.str());
}
