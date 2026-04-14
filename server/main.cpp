#include "dbsingleton.h"
#include "echoserver.h"

#include <QCoreApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QString dbError;
    DBSingleton::getInstance();

    quint16 port = 4242;
    if (app.arguments().size() > 1) {
        bool ok = false;
        const int parsedPort = app.arguments().at(1).toInt(&ok);
        if (ok && parsedPort > 0 && parsedPort <= 65535) {
            port = static_cast<quint16>(parsedPort);
        }
    }

    EchoServer server(port);
    if (!server.start()) {
        qCritical() << "Не удалось запустить сервер";
        return 1;
    }

    qInfo() << "Сервер запущен на порту" << port;
    return app.exec();
}
