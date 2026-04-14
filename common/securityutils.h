#ifndef SECURITYUTILS_H
#define SECURITYUTILS_H

#include <QString>

namespace SecurityUtils {

QString hashPassword(const QString &password);
bool checkPasswordStrength(const QString &password, QString *error = nullptr);

} // namespace SecurityUtils

#endif // SECURITYUTILS_H
