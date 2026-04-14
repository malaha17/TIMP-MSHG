#ifndef GAMESAPER_H
#define GAMESAPER_H

#include <QList>
#include <QPair>
#include <QVector>

class GameSaper
{
public:
    enum CellState {
        Hidden,
        Opened,
        Flagged
    };

    GameSaper();

    void generate_game(int rows = 9, int cols = 9, int mines = 10);
    QList<QPair<int, int>> move(int row, int col);
    bool toggleFlag(int row, int col);

    int get(int row, int col) const;
    CellState state(int row, int col) const;

    int rows() const;
    int cols() const;
    int mines() const;

    bool exploded() const;
    bool isWin() const;
    QList<QPair<int, int>> allMinePositions() const;

private:
    bool isValid(int row, int col) const;
    int countOpenedSafeCells() const;

private:
    int m_rows;
    int m_cols;
    int m_mines;
    bool m_exploded;

    QVector<QVector<int>> m_field;
    QVector<QVector<CellState>> m_state;
};

#endif // GAMESAPER_H
