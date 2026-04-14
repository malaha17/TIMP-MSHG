#include "top10_form.h"

#include "client_singleton.h"

#include <QHeaderView>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

Top10Form::Top10Form(QWidget *parent)
    : QDialog(parent)
    , m_topTable(new QTableWidget(this))
    , m_usersTable(new QTableWidget(this))
    , m_refreshButton(new QPushButton(QStringLiteral("Обновить"), this))
    , m_deleteButton(new QPushButton(QStringLiteral("Удалить выбранного пользователя"), this))
    , m_okButton(new QPushButton(QStringLiteral("OK"), this))
{
    setWindowTitle(QStringLiteral("Статистика"));
    resize(900, 650);

    m_topTable->setColumnCount(4);
    m_topTable->setHorizontalHeaderLabels({QStringLiteral("Логин"), QStringLiteral("Лучшее время"), QStringLiteral("Побед"), QStringLiteral("Очки")});
    m_topTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    m_usersTable->setColumnCount(8);
    m_usersTable->setHorizontalHeaderLabels({
        QStringLiteral("ID"),
        QStringLiteral("Логин"),
        QStringLiteral("Роль"),
        QStringLiteral("Игр"),
        QStringLiteral("Побед"),
        QStringLiteral("Поражений"),
        QStringLiteral("Лучшее время"),
        QStringLiteral("Очки")
    });
    m_usersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel(QStringLiteral("Топ-10 игроков"), this));
    layout->addWidget(m_topTable);
    layout->addWidget(new QLabel(QStringLiteral("Полная статистика пользователей (только для admin)"), this));
    layout->addWidget(m_usersTable);

    auto *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(m_refreshButton);
    buttonLayout->addWidget(m_deleteButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_okButton);
    layout->addLayout(buttonLayout);

    connect(m_okButton, &QPushButton::clicked, this, &Top10Form::Ok);
    connect(m_refreshButton, &QPushButton::clicked, this, &Top10Form::refreshData);
    connect(m_deleteButton, &QPushButton::clicked, this, &Top10Form::deleteSelectedUser);
}

void Top10Form::loadForRole(const QString &role)
{
    m_role = role;
    const bool isAdmin = (m_role == QStringLiteral("admin"));
    m_usersTable->setVisible(isAdmin);
    m_deleteButton->setVisible(isAdmin);
    refreshData();
}

void Top10Form::Ok()
{
    accept();
}

void Top10Form::refreshData()
{
    loadTop10();
    if (m_role == QStringLiteral("admin")) {
        loadUsers();
    }
}

void Top10Form::deleteSelectedUser()
{
    if (m_role != QStringLiteral("admin")) {
        return;
    }

    const int row = m_usersTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, QStringLiteral("Ошибка"), QStringLiteral("Выберите пользователя"));
        return;
    }

    const int userId = m_usersTable->item(row, 0)->text().toInt();

    QJsonObject request;
    request["cmd"] = QStringLiteral("DELETE_USER");
    request["user_id"] = userId;

    const QJsonObject response = ClientSingleton::instance().send_msg(request);
    if (response.value("status").toString() != QStringLiteral("ok")) {
        QMessageBox::warning(this, QStringLiteral("Ошибка"), response.value("message").toString());
        return;
    }

    QMessageBox::information(this, QStringLiteral("Успех"), response.value("message").toString());
    refreshData();
}

void Top10Form::loadTop10()
{
    QJsonObject request;
    request["cmd"] = QStringLiteral("TOP10");

    const QJsonObject response = ClientSingleton::instance().send_msg(request);
    if (response.value("status").toString() != QStringLiteral("ok")) {
        QMessageBox::warning(this, QStringLiteral("Ошибка"), response.value("message").toString());
        return;
    }

    const QJsonArray items = response.value("items").toArray();
    m_topTable->setRowCount(items.size());

    for (int i = 0; i < items.size(); ++i) {
        const QJsonObject item = items.at(i).toObject();
        m_topTable->setItem(i, 0, new QTableWidgetItem(item.value("login").toString()));
        m_topTable->setItem(i, 1, new QTableWidgetItem(QString::number(item.value("best_time").toInt())));
        m_topTable->setItem(i, 2, new QTableWidgetItem(QString::number(item.value("games_won").toInt())));
        m_topTable->setItem(i, 3, new QTableWidgetItem(QString::number(item.value("total_score").toInt())));
    }
}

void Top10Form::loadUsers()
{
    QJsonObject request;
    request["cmd"] = QStringLiteral("USERS");

    const QJsonObject response = ClientSingleton::instance().send_msg(request);
    if (response.value("status").toString() != QStringLiteral("ok")) {
        QMessageBox::warning(this, QStringLiteral("Ошибка"), response.value("message").toString());
        return;
    }

    const QJsonArray items = response.value("items").toArray();
    m_usersTable->setRowCount(items.size());

    for (int i = 0; i < items.size(); ++i) {
        const QJsonObject item = items.at(i).toObject();
        m_usersTable->setItem(i, 0, new QTableWidgetItem(QString::number(item.value("id").toInt())));
        m_usersTable->setItem(i, 1, new QTableWidgetItem(item.value("login").toString()));
        m_usersTable->setItem(i, 2, new QTableWidgetItem(item.value("role").toString()));
        m_usersTable->setItem(i, 3, new QTableWidgetItem(QString::number(item.value("games_played").toInt())));
        m_usersTable->setItem(i, 4, new QTableWidgetItem(QString::number(item.value("games_won").toInt())));
        m_usersTable->setItem(i, 5, new QTableWidgetItem(QString::number(item.value("games_lost").toInt())));

        const QJsonValue bestTime = item.value("best_time");
        m_usersTable->setItem(i, 6, new QTableWidgetItem(bestTime.isNull() ? QStringLiteral("-") : QString::number(bestTime.toInt())));
        m_usersTable->setItem(i, 7, new QTableWidgetItem(QString::number(item.value("total_score").toInt())));
    }
}
