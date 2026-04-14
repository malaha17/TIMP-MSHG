#include "gamesaper.h"

#include <QQueue>
#include <QRandomGenerator>
#include <QSet>
#include <utility>

GameSaper::GameSaper()
    : m_rows(0)
    , m_cols(0)
    , m_mines(0)
    , m_exploded(false)
{
}

void GameSaper::generate_game(int rows, int cols, int mines)
{
    m_rows = rows;
    m_cols = cols;
    m_mines = mines;
    m_exploded = false;

    m_field = QVector<QVector<int>>(m_rows, QVector<int>(m_cols, 0));
    m_state = QVector<QVector<CellState>>(m_rows, QVector<CellState>(m_cols, Hidden));

    QSet<int> mineIndexes;
    while (mineIndexes.size() < m_mines) {
        mineIndexes.insert(QRandomGenerator::global()->bounded(m_rows * m_cols));
    }

    for (int index : std::as_const(mineIndexes)) {
        const int row = index / m_cols;
        const int col = index % m_cols;
        m_field[row][col] = -1;
    }

    for (int row = 0; row < m_rows; ++row) {
        for (int col = 0; col < m_cols; ++col) {
            if (m_field[row][col] == -1) {
                continue;
            }

            int count = 0;
            for (int dr = -1; dr <= 1; ++dr) {
                for (int dc = -1; dc <= 1; ++dc) {
                    if (dr == 0 && dc == 0) {
                        continue;
                    }

                    const int nr = row + dr;
                    const int nc = col + dc;
                    if (isValid(nr, nc) && m_field[nr][nc] == -1) {
                        ++count;
                    }
                }
            }
            m_field[row][col] = count;
        }
    }
}

QList<QPair<int, int>> GameSaper::move(int row, int col)
{
    QList<QPair<int, int>> opened;

    if (!isValid(row, col) || m_state[row][col] == Opened || m_state[row][col] == Flagged) {
        return opened;
    }

    if (m_field[row][col] == -1) {
        m_state[row][col] = Opened;
        m_exploded = true;
        opened.append(qMakePair(row, col));
        return opened;
    }

    QQueue<QPair<int, int>> queue;
    queue.enqueue(qMakePair(row, col));

    while (!queue.isEmpty()) {
        const auto [cr, cc] = queue.dequeue();

        if (!isValid(cr, cc) || m_state[cr][cc] == Opened || m_state[cr][cc] == Flagged) {
            continue;
        }

        m_state[cr][cc] = Opened;
        opened.append(qMakePair(cr, cc));

        if (m_field[cr][cc] != 0) {
            continue;
        }

        for (int dr = -1; dr <= 1; ++dr) {
            for (int dc = -1; dc <= 1; ++dc) {
                if (dr == 0 && dc == 0) {
                    continue;
                }
                queue.enqueue(qMakePair(cr + dr, cc + dc));
            }
        }
    }

    return opened;
}

bool GameSaper::toggleFlag(int row, int col)
{
    if (!isValid(row, col) || m_state[row][col] == Opened) {
        return false;
    }

    if (m_state[row][col] == Hidden) {
        m_state[row][col] = Flagged;
    } else if (m_state[row][col] == Flagged) {
        m_state[row][col] = Hidden;
    }

    return true;
}

int GameSaper::get(int row, int col) const
{
    return isValid(row, col) ? m_field[row][col] : -2;
}

GameSaper::CellState GameSaper::state(int row, int col) const
{
    return isValid(row, col) ? m_state[row][col] : Hidden;
}

int GameSaper::rows() const
{
    return m_rows;
}

int GameSaper::cols() const
{
    return m_cols;
}

int GameSaper::mines() const
{
    return m_mines;
}

bool GameSaper::exploded() const
{
    return m_exploded;
}

bool GameSaper::isWin() const
{
    return countOpenedSafeCells() == (m_rows * m_cols - m_mines) && !m_exploded;
}

QList<QPair<int, int>> GameSaper::allMinePositions() const
{
    QList<QPair<int, int>> result;
    for (int row = 0; row < m_rows; ++row) {
        for (int col = 0; col < m_cols; ++col) {
            if (m_field[row][col] == -1) {
                result.append(qMakePair(row, col));
            }
        }
    }
    return result;
}

bool GameSaper::isValid(int row, int col) const
{
    return row >= 0 && row < m_rows && col >= 0 && col < m_cols;
}

int GameSaper::countOpenedSafeCells() const
{
    int count = 0;
    for (int row = 0; row < m_rows; ++row) {
        for (int col = 0; col < m_cols; ++col) {
            if (m_field[row][col] != -1 && m_state[row][col] == Opened) {
                ++count;
            }
        }
    }
    return count;
}
