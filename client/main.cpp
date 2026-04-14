#include "client_singleton.h"
#include "manager_form.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    if (!ClientSingleton::instance().connectToServer()) {
        QMessageBox::warning(nullptr,
                             QStringLiteral("Нет соединения"),
                             QStringLiteral("Не удалось подключиться к серверу по адресу 127.0.0.1:4242.\n"
                                            "Запусти сервер и открой клиент снова."));
    }

    ManagerForm manager;
    manager.show();

    const int result = app.exec();
    ClientSingleton::instance().disconnectFromServer();
    return result;
}
