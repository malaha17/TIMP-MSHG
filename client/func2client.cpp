#include "func2client.h"

#include "securityutils.h"

namespace Func2Client {

void check_password(const QString &password)
{
    QString error;
    if (!SecurityUtils::checkPasswordStrength(password, &error)) {
        throw ClientLogicException(error);
    }
}

QString hash(const QString &password)
{
    return SecurityUtils::hashPassword(password);
}

} // namespace Func2Client
