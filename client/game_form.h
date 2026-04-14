#ifndef GAME_FORM_H
#define GAME_FORM_H

#include "gamesaper.h"

#include <QWidget>
#include <QVector>
#include <QElapsedTimer>

class CellButton;
class QLabel;
class QPushButton;
class QGridLayout;
class QTimer;

class GameForm : public QWidget
{
    Q_OBJECT
public:
    explicit GameForm(QWidget *parent = nullptr);

    void setCurrentUser(int userId, const QString &login, const QString &role);

signals:
    void top10Requested(const QString &role);
    void logoutRequested();

private slots:
    void startNewGame();
    void onCellLeftClicked(int row, int col);
    void onCellRightClicked(int row, int col);
    void on_game_over(bool won);
    void on_push_button_top10();
    void updateTimer();

private:
    void rebuildBoard();
    void refreshBoard(bool revealAll = false);
    void lockBoard(bool locked);

private:
    int m_userId;
    QString m_login;
    QString m_role;
    bool m_finished;

    GameSaper m_game;
    QVector<QVector<CellButton *>> m_buttons;

    QLabel *m_userLabel;
    QLabel *m_timerLabel;
    QLabel *m_statusLabel;
    QPushButton *m_newGameButton;
    QPushButton *m_top10Button;
    QPushButton *m_logoutButton;
    QWidget *m_boardWidget;
    QGridLayout *m_gridLayout;
    QTimer *m_timer;
    QElapsedTimer m_elapsedTimer;
};

#endif // GAME_FORM_H
