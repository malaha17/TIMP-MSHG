#ifndef FUNC2SERV_H
#define FUNC2SERV_H

#include <QJsonObject>
#include <QString>

struct ClientSession {
    bool authorized = false;
    int userId = -1;
    QString login;
    QString role = QStringLiteral("guest");
};

class Func2Serv
{
public:
    QJsonObject parse(const QJsonObject &request, ClientSession &session);

private:
    QJsonObject auth(const QJsonObject &request, ClientSession &session);
    QJsonObject reg(const QJsonObject &request);
    QJsonObject top10() const;
    QJsonObject saveResult(const QJsonObject &request, const ClientSession &session) const;
    QJsonObject users(const ClientSession &session) const;
    QJsonObject deleteUser(const QJsonObject &request, const ClientSession &session) const;
};

#endif // FUNC2SERV_H
