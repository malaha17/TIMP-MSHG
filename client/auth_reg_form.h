#ifndef AUTH_REG_FORM_H
#define AUTH_REG_FORM_H

#include <QWidget>

class QLabel;
class QLineEdit;
class QPushButton;

class AuthRegForm : public QWidget
{
    Q_OBJECT
public:
    explicit AuthRegForm(QWidget *parent = nullptr);

signals:
    void authenticated(int userId, const QString &login, const QString &role);

private slots:
    void on_reg_button();
    void on_auth_button();
    void on_change_button();

private:
    void change();

private:
    bool m_regMode;
    QLabel *m_titleLabel;
    QLabel *m_infoLabel;
    QLineEdit *m_loginEdit;
    QLineEdit *m_passwordEdit;
    QPushButton *m_actionButton;
    QPushButton *m_changeButton;
};

#endif // AUTH_REG_FORM_H
