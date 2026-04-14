#include "securityutils.h"

#include <QCryptographicHash>

namespace SecurityUtils {

QString hashPassword(const QString &password)
{
    return QString::fromUtf8(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex());
}

bool checkPasswordStrength(const QString &password, QString *error)
{
    if (password.size() < 6) {
        if (error) {
            *error = QStringLiteral("Пароль должен содержать минимум 6 символов");
        }
        return false;
    }

    bool hasLetter = false;
    bool hasDigit = false;
    for (const QChar &ch : password) {
        hasLetter = hasLetter || ch.isLetter();
        hasDigit = hasDigit || ch.isDigit();
    }

    if (!hasLetter || !hasDigit) {
        if (error) {
            *error = QStringLiteral("Пароль должен содержать хотя бы одну букву и одну цифру");
        }
        return false;
    }

    return true;
}

} // namespace SecurityUtils
