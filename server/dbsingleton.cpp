#include "dbsingleton.h"

#include "securityutils.h"

#include <QDateTime>
#include <QJsonObject>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QDir>
#include <QCoreApplication>
#include <QDebug>
#include <QtGlobal>

DBSingleton &DBSingleton::getInstance()
{
    static DBSingleton instance;
    return instance;
}

DBSingleton::DBSingleton()
{
    m_db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"));
    const QString dbPath = QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("server_data.sqlite"));
    m_db.setDatabaseName(dbPath);

    QString error;
    if (!initDatabase(error)) {
        qCritical() << "Ошибка инициализации БД:" << error;
    }
}

DBSingleton::~DBSingleton()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool DBSingleton::initDatabase(QString &error)
{
    if (!m_db.open()) {
        error = m_db.lastError().text();
        return false;
    }

    QSqlQuery query(m_db);

    if (!query.exec(
            "CREATE TABLE IF NOT EXISTS users ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "login TEXT UNIQUE NOT NULL,"
            "password_hash TEXT NOT NULL,"
            "role TEXT NOT NULL DEFAULT 'user',"
            "games_played INTEGER NOT NULL DEFAULT 0,"
            "games_won INTEGER NOT NULL DEFAULT 0,"
            "games_lost INTEGER NOT NULL DEFAULT 0,"
            "best_time INTEGER,"
            "total_score INTEGER NOT NULL DEFAULT 0,"
            "created_at TEXT NOT NULL)")) {
        error = query.lastError().text();
        return false;
    }

    if (!query.exec(
            "CREATE TABLE IF NOT EXISTS game_results ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "user_id INTEGER NOT NULL,"
            "seconds INTEGER NOT NULL,"
            "won INTEGER NOT NULL,"
            "created_at TEXT NOT NULL,"
            "FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE)")) {
        error = query.lastError().text();
        return false;
    }

    return ensureDefaultAdmin(error);
}

bool DBSingleton::ensureDefaultAdmin(QString &error)
{
    QSqlQuery checkQuery(m_db);
    if (!checkQuery.exec("SELECT COUNT(*) FROM users WHERE role = 'admin'")) {
        error = checkQuery.lastError().text();
        return false;
    }

    if (!checkQuery.next()) {
        error = QStringLiteral("Не удалось проверить наличие администратора");
        return false;
    }

    if (checkQuery.value(0).toInt() > 0) {
        return true;
    }

    QSqlQuery insertQuery(m_db);
    insertQuery.prepare(
        "INSERT INTO users (login, password_hash, role, created_at) "
        "VALUES (:login, :password_hash, :role, :created_at)");
    insertQuery.bindValue(":login", QStringLiteral("admin"));
    insertQuery.bindValue(":password_hash", SecurityUtils::hashPassword(QStringLiteral("Admin123!")));
    insertQuery.bindValue(":role", QStringLiteral("admin"));
    insertQuery.bindValue(":created_at", QDateTime::currentDateTimeUtc().toString(Qt::ISODate));

    if (!insertQuery.exec()) {
        error = insertQuery.lastError().text();
        return false;
    }

    return true;
}

bool DBSingleton::auth(const QString &login, const QString &passwordHash, int &userId, QString &role, QString &error)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT id, password_hash, role FROM users WHERE login = :login");
    query.bindValue(":login", login);

    if (!query.exec()) {
        error = query.lastError().text();
        return false;
    }

    if (!query.next()) {
        error = QStringLiteral("Пользователь не найден");
        return false;
    }

    const QString storedHash = query.value("password_hash").toString();
    if (storedHash != passwordHash) {
        error = QStringLiteral("Неверный пароль");
        return false;
    }

    userId = query.value("id").toInt();
    role = query.value("role").toString();
    return true;
}

bool DBSingleton::reg(const QString &login, const QString &passwordHash, QString &error, const QString &role)
{
    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO users (login, password_hash, role, created_at) "
        "VALUES (:login, :password_hash, :role, :created_at)");
    query.bindValue(":login", login);
    query.bindValue(":password_hash", passwordHash);
    query.bindValue(":role", role);
    query.bindValue(":created_at", QDateTime::currentDateTimeUtc().toString(Qt::ISODate));

    if (!query.exec()) {
        error = QStringLiteral("Не удалось зарегистрировать пользователя: %1").arg(query.lastError().text());
        return false;
    }

    return true;
}

bool DBSingleton::saveResult(int userId, int seconds, bool won, QString &error)
{
    QSqlQuery insertResult(m_db);
    insertResult.prepare(
        "INSERT INTO game_results (user_id, seconds, won, created_at) "
        "VALUES (:user_id, :seconds, :won, :created_at)");
    insertResult.bindValue(":user_id", userId);
    insertResult.bindValue(":seconds", seconds);
    insertResult.bindValue(":won", won ? 1 : 0);
    insertResult.bindValue(":created_at", QDateTime::currentDateTimeUtc().toString(Qt::ISODate));

    if (!insertResult.exec()) {
        error = insertResult.lastError().text();
        return false;
    }

    int scoreDelta = 0;
    if (won) {
        scoreDelta = qMax(10, 1000 - seconds * 5);
    }

    QSqlQuery updateUser(m_db);
    if (won) {
        updateUser.prepare(
            "UPDATE users SET "
            "games_played = games_played + 1, "
            "games_won = games_won + 1, "
            "total_score = total_score + :score_delta, "
            "best_time = CASE "
            "WHEN best_time IS NULL OR :seconds < best_time THEN :seconds "
            "ELSE best_time END "
            "WHERE id = :user_id");
        updateUser.bindValue(":score_delta", scoreDelta);
        updateUser.bindValue(":seconds", seconds);
        updateUser.bindValue(":user_id", userId);
    } else {
        updateUser.prepare(
            "UPDATE users SET "
            "games_played = games_played + 1, "
            "games_lost = games_lost + 1 "
            "WHERE id = :user_id");
        updateUser.bindValue(":user_id", userId);
    }

    if (!updateUser.exec()) {
        error = updateUser.lastError().text();
        return false;
    }

    return true;
}

QJsonArray DBSingleton::getTop10(QString &error) const
{
    QJsonArray result;
    QSqlQuery query(m_db);

    if (!query.exec(
            "SELECT login, best_time, games_won, total_score "
            "FROM users "
            "WHERE best_time IS NOT NULL "
            "ORDER BY best_time ASC, total_score DESC, login ASC "
            "LIMIT 10")) {
        error = query.lastError().text();
        return result;
    }

    while (query.next()) {
        QJsonObject item;
        item["login"] = query.value("login").toString();
        item["best_time"] = query.value("best_time").toInt();
        item["games_won"] = query.value("games_won").toInt();
        item["total_score"] = query.value("total_score").toInt();
        result.append(item);
    }

    return result;
}

QJsonArray DBSingleton::getAllUsers(QString &error) const
{
    QJsonArray result;
    QSqlQuery query(m_db);

    if (!query.exec(
            "SELECT id, login, role, games_played, games_won, games_lost, best_time, total_score "
            "FROM users ORDER BY id ASC")) {
        error = query.lastError().text();
        return result;
    }

    while (query.next()) {
        QJsonObject item;
        item["id"] = query.value("id").toInt();
        item["login"] = query.value("login").toString();
        item["role"] = query.value("role").toString();
        item["games_played"] = query.value("games_played").toInt();
        item["games_won"] = query.value("games_won").toInt();
        item["games_lost"] = query.value("games_lost").toInt();

        if (query.value("best_time").isNull()) {
            item["best_time"] = QJsonValue();
        } else {
            item["best_time"] = query.value("best_time").toInt();
        }

        item["total_score"] = query.value("total_score").toInt();
        result.append(item);
    }

    return result;
}

bool DBSingleton::deleteUser(int userId, QString &error)
{
    QSqlQuery roleQuery(m_db);
    roleQuery.prepare("SELECT role FROM users WHERE id = :id");
    roleQuery.bindValue(":id", userId);

    if (!roleQuery.exec()) {
        error = roleQuery.lastError().text();
        return false;
    }

    if (!roleQuery.next()) {
        error = QStringLiteral("Пользователь не найден");
        return false;
    }

    if (roleQuery.value("role").toString() == QStringLiteral("admin")) {
        error = QStringLiteral("Нельзя удалить администратора");
        return false;
    }

    QSqlQuery deleteResults(m_db);
    deleteResults.prepare("DELETE FROM game_results WHERE user_id = :user_id");
    deleteResults.bindValue(":user_id", userId);
    if (!deleteResults.exec()) {
        error = deleteResults.lastError().text();
        return false;
    }

    QSqlQuery deleteUserQuery(m_db);
    deleteUserQuery.prepare("DELETE FROM users WHERE id = :id");
    deleteUserQuery.bindValue(":id", userId);

    if (!deleteUserQuery.exec()) {
        error = deleteUserQuery.lastError().text();
        return false;
    }

    if (deleteUserQuery.numRowsAffected() == 0) {
        error = QStringLiteral("Пользователь не найден");
        return false;
    }

    return true;
}
