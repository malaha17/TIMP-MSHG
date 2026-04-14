#ifndef DBSINGLETON_H
#define DBSINGLETON_H

#include <QJsonArray>
#include <QSqlDatabase>
#include <QString>

class DBSingleton
{
public:
    static DBSingleton &getInstance();

    bool auth(const QString &login, const QString &passwordHash, int &userId, QString &role, QString &error);
    bool reg(const QString &login, const QString &passwordHash, QString &error, const QString &role = QStringLiteral("user"));
    bool saveResult(int userId, int seconds, bool won, QString &error);
    QJsonArray getTop10(QString &error) const;
    QJsonArray getAllUsers(QString &error) const;
    bool deleteUser(int userId, QString &error);

private:
    DBSingleton();
    ~DBSingleton();
    DBSingleton(const DBSingleton &) = delete;
    DBSingleton &operator=(const DBSingleton &) = delete;

    bool initDatabase(QString &error);
    bool ensureDefaultAdmin(QString &error);

private:
    QSqlDatabase m_db;
};

#endif // DBSINGLETON_H
