/*
    SPDX-FileCopyrightText: 2026 Misty contributors
    SPDX-License-Identifier: MIT
*/
#pragma once

#include <QBasicTimer>
#include <QList>
#include <QWidget>

/**
 * Easter egg: falling green glyphs drawn over the parent widget, in the spirit of cmatrix.
 * Any key press or mouse click closes it.
 */
class MatrixRain : public QWidget
{
public:
    explicit MatrixRain(QWidget *parent);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void timerEvent(QTimerEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    struct Drop {
        int head = 0; // row of the brightest glyph
        int length = 0;
        QString glyphs;
    };

    Drop newDrop() const;

    QBasicTimer m_timer;
    QList<Drop> m_drops;
    int m_cellWidth = 0;
    int m_cellHeight = 0;
    int m_rows = 0;
};
