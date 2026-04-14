#include "auth_reg_form.h"

#include "client_singleton.h"
#include "func2client.h"

#include <QFormLayout>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

AuthRegForm::AuthRegForm(QWidget *parent)
    : QWidget(parent)
    , m_regMode(false)
    , m_titleLabel(new QLabel(this))
    , m_infoLabel(new QLabel(this))
    , m_loginEdit(new QLineEdit(this))
    , m_passwordEdit(new QLineEdit(this))
    , m_actionButton(new QPushButton(this))
    , m_changeButton(new QPushButton(this))
{
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_infoLabel->setWordWrap(true);

    auto *formLayout = new QFormLayout;
    formLayout->addRow(QStringLiteral("Логин:"), m_loginEdit);
    formLayout->addRow(QStringLiteral("Пароль:"), m_passwordEdit);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(m_titleLabel);
    layout->addWidget(m_infoLabel);
    layout->addLayout(formLayout);
    layout->addWidget(m_actionButton);
    layout->addWidget(m_changeButton);
    layout->addStretch();

    connect(m_actionButton, &QPushButton::clicked, this, [this]() {
        if (m_regMode) {
            on_reg_button();
        } else {
            on_auth_button();
        }
    });
    connect(m_changeButton, &QPushButton::clicked, this, &AuthRegForm::on_change_button);

    change();
}

void AuthRegForm::on_reg_button()
{
    const QString login = m_loginEdit->text().trimmed();
    const QString password = m_passwordEdit->text();

    if (login.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("Ошибка"), QStringLiteral("Введите логин и пароль"));
        return;
    }

    try {
        Func2Client::check_password(password);
    } catch (const ClientLogicException &ex) {
        QMessageBox::warning(this, QStringLiteral("Пароль не подходит"), QString::fromStdString(ex.what()));
        return;
    }

    QJsonObject request;
    request["cmd"] = QStringLiteral("REG");
    request["login"] = login;
    request["password_hash"] = Func2Client::hash(password);

    const QJsonObject response = ClientSingleton::instance().send_msg(request);
    if (response.value("status").toString() == QStringLiteral("ok")) {
        QMessageBox::information(this, QStringLiteral("Успех"), response.value("message").toString());
        m_regMode = false;
        change();
    } else {
        QMessageBox::warning(this, QStringLiteral("Ошибка"), response.value("message").toString());
    }
}

void AuthRegForm::on_auth_button()
{
    const QString login = m_loginEdit->text().trimmed();
    const QString password = m_passwordEdit->text();

    if (login.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("Ошибка"), QStringLiteral("Введите логин и пароль"));
        return;
    }

    QJsonObject request;
    request["cmd"] = QStringLiteral("AUTH");
    request["login"] = login;
    request["password_hash"] = Func2Client::hash(password);

    const QJsonObject response = ClientSingleton::instance().send_msg(request);
    if (response.value("status").toString() != QStringLiteral("ok")) {
        QMessageBox::warning(this, QStringLiteral("Ошибка авторизации"), response.value("message").toString());
        return;
    }

    emit authenticated(response.value("user_id").toInt(),
                       response.value("login").toString(),
                       response.value("role").toString());
}

void AuthRegForm::on_change_button()
{
    m_regMode = !m_regMode;
    change();
}

void AuthRegForm::change()
{
    if (m_regMode) {
        m_titleLabel->setText(QStringLiteral("Регистрация"));
        m_infoLabel->setText(QStringLiteral("Создай новый аккаунт. Пароль должен содержать минимум 6 символов, буквы и цифры."));
        m_actionButton->setText(QStringLiteral("Зарегистрироваться"));
        m_changeButton->setText(QStringLiteral("У меня уже есть аккаунт"));
    } else {
        m_titleLabel->setText(QStringLiteral("Авторизация"));
        m_infoLabel->setText(QStringLiteral("Войди в систему, чтобы отправлять результаты на сервер и смотреть статистику."));
        m_actionButton->setText(QStringLiteral("Войти"));
        m_changeButton->setText(QStringLiteral("Создать аккаунт"));
    }
}
