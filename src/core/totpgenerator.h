#ifndef TOTPGENERATOR_H
#define TOTPGENERATOR_H

#include <QString>

class TOTPGenerator {
public:
    static QString generateTOTP(const QString &base32_secret,
                                int digits = 6,
                                int time_step = 30,
                                int t0 = 0);

private:
    static QByteArray base32Decode(const QString &base32);
};

#endif // TOTPGENERATOR_H
