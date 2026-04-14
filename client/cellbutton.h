#ifndef CELLBUTTON_H
#define CELLBUTTON_H

#include <QPushButton>
#include <QMouseEvent>

class CellButton : public QPushButton
{
    Q_OBJECT
public:
    CellButton(int row, int col, QWidget *parent = nullptr)
        : QPushButton(parent)
        , m_row(row)
        , m_col(col)
    {
        setFixedSize(36, 36);
    }

signals:
    void leftClicked(int row, int col);
    void rightClicked(int row, int col);

protected:
    void mousePressEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::RightButton) {
            emit rightClicked(m_row, m_col);
            return;
        }
        emit leftClicked(m_row, m_col);
        QPushButton::mousePressEvent(event);
    }

private:
    int m_row;
    int m_col;
};

#endif // CELLBUTTON_H
