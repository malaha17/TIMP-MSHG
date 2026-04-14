#ifndef TOP10_FORM_H
#define TOP10_FORM_H

#include <QDialog>

class QPushButton;
class QTableWidget;

class Top10Form : public QDialog
{
    Q_OBJECT
public:
    explicit Top10Form(QWidget *parent = nullptr);
    void loadForRole(const QString &role);

private slots:
    void Ok();
    void refreshData();
    void deleteSelectedUser();

private:
    void loadTop10();
    void loadUsers();

private:
    QString m_role;
    QTableWidget *m_topTable;
    QTableWidget *m_usersTable;
    QPushButton *m_refreshButton;
    QPushButton *m_deleteButton;
    QPushButton *m_okButton;
};

#endif // TOP10_FORM_H
