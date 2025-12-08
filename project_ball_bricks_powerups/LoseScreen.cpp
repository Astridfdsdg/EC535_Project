#include "LoseScreen.h"
#include <QApplication>
#include <QPainter>
#include <QMouseEvent>
#include <QLinearGradient>
#include <QPen>
#include <cmath>

LoseScreen::LoseScreen(int level, QWidget* parent)
    : QWidget(parent),
    m_level(level)

{
    setAttribute(Qt::WA_AcceptTouchEvents, true);
    resetScreen();
    connect(&m_timer, &QTimer::timeout, this, &LoseScreen::onTick);
    m_timer.start(16);       // ~60 FPS
    m_clock.start();
}

void LoseScreen::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Draw background
    p.fillRect(rect(), Qt::black);

    const qreal sx = width() / m_bounds.width();
    const qreal sy = height() / m_bounds.height();

    p.save();
    p.scale(sx,sy);

    // Draw animated bricks
    for (const Brick& b : m_bricks) {
        p.setBrush(b.color);
        p.setPen(Qt::NoPen);
        p.drawRoundedRect(b.rect, 4, 4);
    }

    p.restore();

    const int tbw = 240;
    const int tbh = 36;
    float tcx = (width() - tbw) / 2;
    float titleY = (height() / 2) - 87;

    QRectF goBox(tcx, titleY , tbw, tbh);

    const int bw = 120;
    const int bh = 28;
    const int gap = 10;

    float cx = (width() - bw) / 2;
    float firstY = goBox.bottom() + 40;

    menuButton = QRectF(cx, firstY + 0*(bh + gap), bw, bh);
    retryButton = QRectF(cx, firstY + 1*(bh + gap), bw, bh);
    quitButton   = QRectF(cx, firstY + 2*(bh + gap), bw, bh);

    p.setBrush(QColor(40, 40, 40, 200));
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(goBox, 10, 10);
    QPen pen(Qt::white);      // white frame
    pen.setWidth(3);           // thickness of the border
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);   // don’t fill again
    p.drawRoundedRect(goBox, 10, 10);

    // Draw menu title
    p.setPen(Qt::white);
    p.setFont(QFont("Arial", 20, QFont::Bold));
    p.drawText(goBox, Qt::AlignCenter, "GAME OVER");

    // Draw buttons
    p.setBrush(QColor(40, 40, 140));
    p.setPen(Qt::NoPen);

    p.drawRoundedRect(menuButton, 10, 10);
    p.drawRoundedRect(retryButton, 10, 10);
    p.drawRoundedRect(quitButton,   10, 10);

    p.setPen(Qt::white);
    p.setFont(QFont("Arial", 12, QFont::Bold));

    p.drawText(menuButton, Qt::AlignCenter, "Main Menu");
    p.drawText(retryButton, Qt::AlignCenter, "Retry Level");
    p.drawText(quitButton,   Qt::AlignCenter, "Quit");
}

void LoseScreen::mousePressEvent(QMouseEvent* event) {
    {
        QPointF pos = event->localPos();
        if (menuButton.contains(pos)) {
            emit returnToMenu();
        }
        if (retryButton.contains(pos)) {
            emit retryLevel(m_level);
        }
        if (quitButton.contains(pos)) {
            m_timer.stop();       // stop any running timers
            this->hide();         // clear the widget from the screen
            QApplication::processEvents();  // ensure pending paint/hide events complete
            QApplication::quit();
        }
    }
}

void LoseScreen::onTick() {
    qint64 ms = m_clock.elapsed();

    float speed = 0.004f;
    float amplitude = 55.0f;  // how much brightness fluctuates

    for (size_t i = 0; i < m_bricks.size(); ++i) {
        Brick& b = m_bricks[i];

        float t = ms * speed + i * 0.3f;   // stagger bricks
        int valOffset = static_cast<int>(amplitude * std::sin(t));

        int val = 200 + valOffset;          // 200 → 255
        val = qBound(0, val, 255);          // keep in valid range

        b.color.setHsv(b.baseHue, 255, val);  // keep hue=0 (red), full saturation
    }

    update();
}

void LoseScreen::resetScreen() {
    initBricks();
}

void LoseScreen::initBricks() {
    m_bricks.clear();

    int rows = 5;
    int cols = 9;
    float bw = 80;
    float bh = 20;
    float gap = 4;
    float offsetX = 20;
    float offsetY = 60;

    for (int r = 0; r < rows; ++r) {
        for (int c =0; c < cols; ++c) {
            float x = offsetX + c * (bw + gap);
            float y = offsetY + r * (bh + gap);

            int baseHue = 0;

            Brick b;
            b.rect = QRectF(x, y, bw, bh);
            b.baseHue = baseHue;

            b.color = QColor::fromHsv(baseHue, 255, 255);

            m_bricks.push_back(b);
        }
    }
}
