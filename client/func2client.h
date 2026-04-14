#ifndef FUNC2CLIENT_H
#define FUNC2CLIENT_H

#include <QString>
#include <stdexcept>

class ClientLogicException : public std::runtime_error
{
public:
    explicit ClientLogicException(const QString &message)
        : std::runtime_error(message.toStdString())
    {
    }
};

namespace Func2Client {

void check_password(const QString &password);
QString hash(const QString &password);

} // namespace Func2Client

#endif // FUNC2CLIENT_H
