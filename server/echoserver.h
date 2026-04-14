#ifndef ECHOSERVER_H
#define ECHOSERVER_H

#include "func2serv.h"

#include <QObject>
#include <QHash>

class QTcpServer;
class QTcpSocket;

class EchoServer : public QObject
{
    Q_OBJECT
public:
    explicit EchoServer(quint16 port, QObject *parent = nullptr);
    bool start();

private slots:
    void newConnect();
    void disconnectClient();
    void readyRead();

private:
    QTcpServer *m_server;
    quint16 m_port;
    Func2Serv m_handler;
    QHash<QTcpSocket *, ClientSession> m_sessions;
};

#endif // ECHOSERVER_H
