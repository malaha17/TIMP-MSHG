#ifndef MANAGER_FORM_H
#define MANAGER_FORM_H

#include <QWidget>

class AuthRegForm;
class GameForm;
class Top10Form;
class QStackedWidget;

class ManagerForm : public QWidget
{
    Q_OBJECT
public:
    explicit ManagerForm(QWidget *parent = nullptr);

private slots:
    void handleAuthenticated(int userId, const QString &login, const QString &role);
    void showTop10(const QString &role);
    void backToAuth();

private:
    AuthRegForm *m_authForm;
    GameForm *m_gameForm;
    Top10Form *m_top10Form;
    QStackedWidget *m_stack;
};

#endif // MANAGER_FORM_H
