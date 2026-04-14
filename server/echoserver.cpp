#include "echoserver.h"

#include "protocol.h"

#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>

EchoServer::EchoServer(quint16 port, QObject *parent)
    : QObject(parent)
    , m_server(new QTcpServer(this))
    , m_port(port)
{
    connect(m_server, &QTcpServer::newConnection, this, &EchoServer::newConnect);
}

bool EchoServer::start()
{
    return m_server->listen(QHostAddress::Any, m_port);
}

void EchoServer::newConnect()
{
    while (m_server->hasPendingConnections()) {
        QTcpSocket *socket = m_server->nextPendingConnection();
        if (!socket) {
            continue;
        }

        qInfo() << "Новое подключение:" << socket->peerAddress().toString() << socket->peerPort();
        m_sessions.insert(socket, ClientSession{});

        connect(socket, &QTcpSocket::disconnected, this, &EchoServer::disconnectClient);
        connect(socket, &QTcpSocket::readyRead, this, &EchoServer::readyRead);
    }
}

void EchoServer::disconnectClient()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (!socket) {
        return;
    }

    qInfo() << "Клиент отключился:" << socket->peerAddress().toString() << socket->peerPort();
    m_sessions.remove(socket);
    socket->deleteLater();
}

void EchoServer::readyRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (!socket) {
        return;
    }

    while (socket->canReadLine()) {
        const QByteArray rawData = socket->readLine();
        QString error;
        const QJsonObject request = Protocol::deserialize(rawData, &error);

        QJsonObject response;
        if (request.isEmpty() && !error.isEmpty()) {
            response = Protocol::makeResponse(QStringLiteral("error"), QStringLiteral("Ошибка JSON: %1").arg(error));
        } else {
            ClientSession &session = m_sessions[socket];
            response = m_handler.parse(request, session);
        }

        socket->write(Protocol::serialize(response));
        socket->flush();
    }
}
