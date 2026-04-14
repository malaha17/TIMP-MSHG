#include "client_singleton.h"

#include "protocol.h"

#include <QTcpSocket>

ClientSingleton &ClientSingleton::instance()
{
    static ClientSingleton client;
    return client;
}

ClientSingleton::ClientSingleton(QObject *parent)
    : QObject(parent)
    , m_socket(new QTcpSocket(this))
    , m_host(QStringLiteral("127.0.0.1"))
    , m_port(4242)
{
}

bool ClientSingleton::connectToServer(const QString &host, quint16 port)
{
    m_host = host;
    m_port = port;

    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        return true;
    }

    m_socket->connectToHost(m_host, m_port);
    return m_socket->waitForConnected(3000);
}

void ClientSingleton::disconnectFromServer()
{
    if (m_socket->state() != QAbstractSocket::UnconnectedState) {
        m_socket->disconnectFromHost();
        if (m_socket->state() != QAbstractSocket::UnconnectedState) {
            m_socket->waitForDisconnected(1000);
        }
    }
}

bool ClientSingleton::isConnected() const
{
    return m_socket->state() == QAbstractSocket::ConnectedState;
}

QJsonObject ClientSingleton::send_msg(const QJsonObject &request)
{
    if (!isConnected() && !connectToServer(m_host, m_port)) {
        return Protocol::makeResponse(QStringLiteral("error"), QStringLiteral("Нет соединения с сервером"));
    }

    m_socket->write(Protocol::serialize(request));
    if (!m_socket->waitForBytesWritten(3000)) {
        return Protocol::makeResponse(QStringLiteral("error"), QStringLiteral("Не удалось отправить запрос"));
    }

    while (!m_socket->canReadLine()) {
        if (!m_socket->waitForReadyRead(3000)) {
            return Protocol::makeResponse(QStringLiteral("error"), QStringLiteral("Сервер не ответил"));
        }
    }

    QString error;
    const QJsonObject response = Protocol::deserialize(m_socket->readLine(), &error);
    if (response.isEmpty() && !error.isEmpty()) {
        return Protocol::makeResponse(QStringLiteral("error"), QStringLiteral("Некорректный ответ сервера: %1").arg(error));
    }

    return response;
}
