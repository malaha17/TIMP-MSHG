#include "manager_form.h"

#include "auth_reg_form.h"
#include "game_form.h"
#include "top10_form.h"

#include <QStackedWidget>
#include <QVBoxLayout>

ManagerForm::ManagerForm(QWidget *parent)
    : QWidget(parent)
    , m_authForm(new AuthRegForm(this))
    , m_gameForm(new GameForm(this))
    , m_top10Form(new Top10Form(this))
    , m_stack(new QStackedWidget(this))
{
    setWindowTitle(QStringLiteral("Сапёр: клиент"));
    resize(650, 700);

    m_stack->addWidget(m_authForm);
    m_stack->addWidget(m_gameForm);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(m_stack);

    connect(m_authForm, &AuthRegForm::authenticated, this, &ManagerForm::handleAuthenticated);
    connect(m_gameForm, &GameForm::top10Requested, this, &ManagerForm::showTop10);
    connect(m_gameForm, &GameForm::logoutRequested, this, &ManagerForm::backToAuth);
}

void ManagerForm::handleAuthenticated(int userId, const QString &login, const QString &role)
{
    m_gameForm->setCurrentUser(userId, login, role);
    m_stack->setCurrentWidget(m_gameForm);
}

void ManagerForm::showTop10(const QString &role)
{
    m_top10Form->loadForRole(role);
    m_top10Form->exec();
}

void ManagerForm::backToAuth()
{
    m_stack->setCurrentWidget(m_authForm);
}
