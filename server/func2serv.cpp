#include "func2serv.h"

#include "dbsingleton.h"
#include "protocol.h"

#include <QJsonArray>

QJsonObject Func2Serv::parse(const QJsonObject &request, ClientSession &session)
{
    const QString cmd = request.value("cmd").toString().trimmed().toUpper();

    if (cmd == QStringLiteral("PING")) {
        return Protocol::makeResponse(QStringLiteral("ok"), QStringLiteral("pong"));
    }
    if (cmd == QStringLiteral("AUTH")) {
        return auth(request, session);
    }
    if (cmd == QStringLiteral("REG")) {
        return reg(request);
    }
    if (cmd == QStringLiteral("TOP10")) {
        return top10();
    }
    if (cmd == QStringLiteral("SAVE_RESULT")) {
        return saveResult(request, session);
    }
    if (cmd == QStringLiteral("USERS")) {
        return users(session);
    }
    if (cmd == QStringLiteral("DELETE_USER")) {
        return deleteUser(request, session);
    }

    return Protocol::makeResponse(QStringLiteral("error"), QStringLiteral("Неизвестная команда"));
}

QJsonObject Func2Serv::auth(const QJsonObject &request, ClientSession &session)
{
    const QString login = request.value("login").toString().trimmed();
    const QString passwordHash = request.value("password_hash").toString().trimmed();

    if (login.isEmpty() || passwordHash.isEmpty()) {
        return Protocol::makeResponse(QStringLiteral("error"), QStringLiteral("Логин и пароль обязательны"));
    }

    int userId = -1;
    QString role;
    QString error;
    if (!DBSingleton::getInstance().auth(login, passwordHash, userId, role, error)) {
        return Protocol::makeResponse(QStringLiteral("error"), error);
    }

    session.authorized = true;
    session.userId = userId;
    session.login = login;
    session.role = role;

    QJsonObject response = Protocol::makeResponse(QStringLiteral("ok"), QStringLiteral("Авторизация успешна"));
    response["user_id"] = userId;
    response["login"] = login;
    response["role"] = role;
    return response;
}

QJsonObject Func2Serv::reg(const QJsonObject &request)
{
    const QString login = request.value("login").toString().trimmed();
    const QString passwordHash = request.value("password_hash").toString().trimmed();

    if (login.isEmpty() || passwordHash.isEmpty()) {
        return Protocol::makeResponse(QStringLiteral("error"), QStringLiteral("Логин и пароль обязательны"));
    }

    QString error;
    if (!DBSingleton::getInstance().reg(login, passwordHash, error)) {
        return Protocol::makeResponse(QStringLiteral("error"), error);
    }

    return Protocol::makeResponse(QStringLiteral("ok"), QStringLiteral("Регистрация успешна"));
}

QJsonObject Func2Serv::top10() const
{
    QString error;
    const QJsonArray top = DBSingleton::getInstance().getTop10(error);
    if (!error.isEmpty()) {
        return Protocol::makeResponse(QStringLiteral("error"), error);
    }

    QJsonObject response = Protocol::makeResponse(QStringLiteral("ok"), QStringLiteral("Топ-10 получен"));
    response["items"] = top;
    return response;
}

QJsonObject Func2Serv::saveResult(const QJsonObject &request, const ClientSession &session) const
{
    if (!session.authorized) {
        return Protocol::makeResponse(QStringLiteral("error"), QStringLiteral("Требуется авторизация"));
    }

    const int seconds = request.value("seconds").toInt(-1);
    const bool won = request.value("won").toBool(false);

    if (seconds < 0) {
        return Protocol::makeResponse(QStringLiteral("error"), QStringLiteral("Некорректное время"));
    }

    QString error;
    if (!DBSingleton::getInstance().saveResult(session.userId, seconds, won, error)) {
        return Protocol::makeResponse(QStringLiteral("error"), error);
    }

    return Protocol::makeResponse(QStringLiteral("ok"), QStringLiteral("Результат сохранён"));
}

QJsonObject Func2Serv::users(const ClientSession &session) const
{
    if (!session.authorized || session.role != QStringLiteral("admin")) {
        return Protocol::makeResponse(QStringLiteral("error"), QStringLiteral("Доступ только для администратора"));
    }

    QString error;
    const QJsonArray usersArray = DBSingleton::getInstance().getAllUsers(error);
    if (!error.isEmpty()) {
        return Protocol::makeResponse(QStringLiteral("error"), error);
    }

    QJsonObject response = Protocol::makeResponse(QStringLiteral("ok"), QStringLiteral("Список пользователей получен"));
    response["items"] = usersArray;
    return response;
}

QJsonObject Func2Serv::deleteUser(const QJsonObject &request, const ClientSession &session) const
{
    if (!session.authorized || session.role != QStringLiteral("admin")) {
        return Protocol::makeResponse(QStringLiteral("error"), QStringLiteral("Доступ только для администратора"));
    }

    const int userId = request.value("user_id").toInt(-1);
    if (userId <= 0) {
        return Protocol::makeResponse(QStringLiteral("error"), QStringLiteral("Некорректный идентификатор пользователя"));
    }

    if (userId == session.userId) {
        return Protocol::makeResponse(QStringLiteral("error"), QStringLiteral("Нельзя удалить самого себя"));
    }

    QString error;
    if (!DBSingleton::getInstance().deleteUser(userId, error)) {
        return Protocol::makeResponse(QStringLiteral("error"), error);
    }

    return Protocol::makeResponse(QStringLiteral("ok"), QStringLiteral("Пользователь удалён"));
}
