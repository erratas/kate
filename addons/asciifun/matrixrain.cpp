/*
    SPDX-FileCopyrightText: 2026 Misty contributors
    SPDX-License-Identifier: MIT
*/
#include "matrixrain.h"

#include <QFontDatabase>
#include <QFontMetrics>
#include <QPainter>
#include <QRandomGenerator>
#include <QResizeEvent>

static const QString &glyphPool()
{
    static const QString pool = QStringLiteral("0123456789ABCDEFZ:+=*<>|ｱｲｳｴｵｶｷｸｹｺｻｼｽｾｿﾀﾁﾂﾃﾄﾅﾆﾇﾈﾉﾊﾋﾌﾍﾎﾏﾐﾑﾒﾓﾔﾕﾖﾗﾘﾙﾚﾛﾜﾝ");
    return pool;
}

MatrixRain::MatrixRain(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setFocusPolicy(Qt::StrongFocus);
    setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    const QFontMetrics fm(font());
    m_cellWidth = std::max(1, fm.horizontalAdvance(QLatin1Char('M')));
    m_cellHeight = std::max(1, fm.height());

    // follow the parent's size for as long as we live
    parent->installEventFilter(this);
    setGeometry(parent->rect());
    show();
    raise();
    setFocus();
    m_timer.start(60, this);
}

MatrixRain::Drop MatrixRain::newDrop() const
{
    auto *rng = QRandomGenerator::global();
    Drop d;
    d.length = rng->bounded(4, std::max(5, m_rows));
    d.head = -rng->bounded(0, std::max(1, m_rows)); // start above the top so columns are staggered
    for (int i = 0; i < d.length; ++i) {
        d.glyphs += glyphPool()[rng->bounded(int(glyphPool().size()))];
    }
    return d;
}

void MatrixRain::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    m_rows = event->size().height() / m_cellHeight + 1;
    const int columns = event->size().width() / m_cellWidth + 1;
    m_drops.resize(columns);
    for (Drop &d : m_drops) {
        if (d.length == 0) {
            d = newDrop();
        }
    }
}

void MatrixRain::timerEvent(QTimerEvent *event)
{
    if (event->timerId() != m_timer.timerId()) {
        QWidget::timerEvent(event);
        return;
    }
    auto *rng = QRandomGenerator::global();
    for (Drop &d : m_drops) {
        ++d.head;
        if (d.head - d.length > m_rows) {
            d = newDrop();
        }
        // occasionally flicker one glyph of the trail
        d.glyphs[rng->bounded(int(d.glyphs.size()))] = glyphPool()[rng->bounded(int(glyphPool().size()))];
    }
    update();
}

void MatrixRain::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.fillRect(rect(), QColor(0, 0, 0, 235));
    p.setFont(font());
    const int ascent = QFontMetrics(font()).ascent();
    for (int col = 0; col < m_drops.size(); ++col) {
        const Drop &d = m_drops[col];
        for (int i = 0; i < d.length; ++i) {
            const int row = d.head - i;
            if (row < 0 || row >= m_rows) {
                continue;
            }
            // head is white-green, the tail fades out
            const QColor color = i == 0 ? QColor(220, 255, 220) : QColor(0, 255, 70, 255 - (200 * i / d.length));
            p.setPen(color);
            p.drawText(col * m_cellWidth, row * m_cellHeight + ascent, QString(d.glyphs[i]));
        }
    }
}

void MatrixRain::keyPressEvent(QKeyEvent *)
{
    close();
}

void MatrixRain::mousePressEvent(QMouseEvent *)
{
    close();
}

bool MatrixRain::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == parentWidget() && event->type() == QEvent::Resize) {
        setGeometry(parentWidget()->rect());
    }
    return QWidget::eventFilter(watched, event);
}
