#include "game_form.h"

#include "cellbutton.h"
#include "client_singleton.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QJsonObject>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <utility>

GameForm::GameForm(QWidget *parent)
    : QWidget(parent)
    , m_userId(-1)
    , m_finished(false)
    , m_userLabel(new QLabel(this))
    , m_timerLabel(new QLabel(this))
    , m_statusLabel(new QLabel(this))
    , m_newGameButton(new QPushButton(QStringLiteral("Новая игра"), this))
    , m_top10Button(new QPushButton(QStringLiteral("Топ-10 / Статистика"), this))
    , m_logoutButton(new QPushButton(QStringLiteral("Выйти"), this))
    , m_boardWidget(new QWidget(this))
    , m_gridLayout(new QGridLayout(m_boardWidget))
    , m_timer(new QTimer(this))
{
    auto *topLayout = new QHBoxLayout;
    topLayout->addWidget(m_userLabel);
    topLayout->addStretch();
    topLayout->addWidget(m_timerLabel);

    auto *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(m_newGameButton);
    buttonLayout->addWidget(m_top10Button);
    buttonLayout->addWidget(m_logoutButton);

    m_gridLayout->setSpacing(1);
    m_gridLayout->setContentsMargins(0, 0, 0, 0);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(m_statusLabel);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(m_boardWidget, 0, Qt::AlignCenter);
    mainLayout->addStretch();

    connect(m_newGameButton, &QPushButton::clicked, this, &GameForm::startNewGame);
    connect(m_top10Button, &QPushButton::clicked, this, &GameForm::on_push_button_top10);
    connect(m_logoutButton, &QPushButton::clicked, this, &GameForm::logoutRequested);
    connect(m_timer, &QTimer::timeout, this, &GameForm::updateTimer);

    startNewGame();
}

void GameForm::setCurrentUser(int userId, const QString &login, const QString &role)
{
    m_userId = userId;
    m_login = login;
    m_role = role;
    m_userLabel->setText(QStringLiteral("Пользователь: %1 (%2)").arg(m_login, m_role));
    m_statusLabel->setText(QStringLiteral("Игра началась. Левая кнопка — открыть, правая — поставить флажок."));
}

void GameForm::startNewGame()
{
    m_game.generate_game(9, 9, 10);
    m_finished = false;
    rebuildBoard();
    m_elapsedTimer.restart();
    m_timer->start(1000);
    updateTimer();
    lockBoard(false);
    m_statusLabel->setText(QStringLiteral("Новая игра запущена."));
}

void GameForm::onCellLeftClicked(int row, int col)
{
    if (m_finished) {
        return;
    }

    m_game.move(row, col);
    refreshBoard();

    if (m_game.exploded()) {
        on_game_over(false);
        return;
    }

    if (m_game.isWin()) {
        on_game_over(true);
    }
}

void GameForm::onCellRightClicked(int row, int col)
{
    if (m_finished) {
        return;
    }

    m_game.toggleFlag(row, col);
    refreshBoard();
}

void GameForm::on_game_over(bool won)
{
    m_finished = true;
    m_timer->stop();
    refreshBoard(true);
    lockBoard(true);

    const int seconds = static_cast<int>(m_elapsedTimer.elapsed() / 1000);

    QJsonObject request;
    request["cmd"] = QStringLiteral("SAVE_RESULT");
    request["seconds"] = seconds;
    request["won"] = won;

    const QJsonObject response = ClientSingleton::instance().send_msg(request);
    if (response.value("status").toString() != QStringLiteral("ok")) {
        QMessageBox::warning(this, QStringLiteral("Ошибка сервера"), response.value("message").toString());
    }

    if (won) {
        m_statusLabel->setText(QStringLiteral("Победа за %1 сек.").arg(seconds));
        QMessageBox::information(this, QStringLiteral("Победа"), QStringLiteral("Ты выиграл за %1 секунд!").arg(seconds));
    } else {
        m_statusLabel->setText(QStringLiteral("Поражение."));
        QMessageBox::information(this, QStringLiteral("Поражение"), QStringLiteral("Ты наступил на мину."));
    }
}

void GameForm::on_push_button_top10()
{
    emit top10Requested(m_role);
}

void GameForm::updateTimer()
{
    const int seconds = static_cast<int>(m_elapsedTimer.elapsed() / 1000);
    m_timerLabel->setText(QStringLiteral("Время: %1 сек").arg(seconds));
}

void GameForm::rebuildBoard()
{
    while (QLayoutItem *item = m_gridLayout->takeAt(0)) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }

    m_buttons.clear();
    m_buttons.resize(m_game.rows());

    for (int row = 0; row < m_game.rows(); ++row) {
        m_buttons[row].resize(m_game.cols());
        for (int col = 0; col < m_game.cols(); ++col) {
            auto *button = new CellButton(row, col, m_boardWidget);
            connect(button, &CellButton::leftClicked, this, &GameForm::onCellLeftClicked);
            connect(button, &CellButton::rightClicked, this, &GameForm::onCellRightClicked);
            m_gridLayout->addWidget(button, row, col);
            m_buttons[row][col] = button;
        }
    }

    refreshBoard();
}

void GameForm::refreshBoard(bool revealAll)
{
    for (int row = 0; row < m_game.rows(); ++row) {
        for (int col = 0; col < m_game.cols(); ++col) {
            CellButton *button = m_buttons[row][col];
            const int value = m_game.get(row, col);
            const GameSaper::CellState cellState = m_game.state(row, col);

            button->setEnabled(!m_finished);
            button->setStyleSheet(QString());

            if (revealAll && value == -1) {
                button->setText(QStringLiteral("*") );
                button->setStyleSheet(QStringLiteral("background-color: #ffb3b3;"));
                continue;
            }

            if (cellState == GameSaper::Flagged) {
                button->setText(QStringLiteral("F"));
                button->setStyleSheet(QStringLiteral("background-color: #fff2a8;"));
                continue;
            }

            if (cellState == GameSaper::Hidden) {
                button->setText(QString());
                continue;
            }

            button->setStyleSheet(QStringLiteral("background-color: #38761d;"));
            if (value == 0) {
                button->setText(QString());
            } else if (value == -1) {
                button->setText(QStringLiteral("*") );
                button->setStyleSheet(QStringLiteral("background-color: #cc0000;"));
            } else {
                button->setText(QString::number(value));
            }
        }
    }
}

void GameForm::lockBoard(bool locked)
{
    for (const auto &row : std::as_const(m_buttons)) {
        for (CellButton *button : row) {
            if (button) {
                button->setEnabled(!locked);
            }
        }
    }
}
