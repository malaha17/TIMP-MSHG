#ifndef CLIENT_SINGLETON_H
#define CLIENT_SINGLETON_H

#include <QObject>
#include <QJsonObject>

class QTcpSocket;

class ClientSingleton : public QObject
{
    Q_OBJECT
public:
    static ClientSingleton &instance();

    bool connectToServer(const QString &host = QStringLiteral("127.0.0.1"), quint16 port = 4242);
    void disconnectFromServer();
    bool isConnected() const;
    QJsonObject send_msg(const QJsonObject &request);

private:
    explicit ClientSingleton(QObject *parent = nullptr);
    Q_DISABLE_COPY_MOVE(ClientSingleton)

private:
    QTcpSocket *m_socket;
    QString m_host;
    quint16 m_port;
};

#endif // CLIENT_SINGLETON_H
