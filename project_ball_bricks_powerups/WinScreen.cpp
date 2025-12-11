#include "WinScreen.h"
#include <QPainter>
#include <QApplication>
#include <QMouseEvent>
#include <QLinearGradient>
#include <QPen>
#include <cmath>

WinScreen::WinScreen(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_AcceptTouchEvents, true);
    connect(&m_timer, &QTimer::timeout, this, &WinScreen::onTick);
    resetScreen();
    m_timer.start(16); //~60 fps
    m_clock.start();
}

void WinScreen::paintEvent(QPaintEvent*) {
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

    QRectF winBox(tcx, titleY , tbw, tbh); //Win text banner text box

    const int bw = 120;
    const int bh = 28;
    const int gap = 20;

    float cx = (width() - bw) / 2;
    float firstY = winBox.bottom() + 40;

    menuButton = QRectF(cx, firstY + 0*(bh + gap), bw, bh);
    quitButton   = QRectF(cx, firstY + 1*(bh + gap), bw, bh);

    p.setBrush(QColor(40, 40, 40, 200));
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(winBox, 10, 10);
    QPen pen(Qt::white);      // white frame
    pen.setWidth(3);           // thickness of the border
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);   // don’t fill again
    p.drawRoundedRect(winBox, 10, 10);

    // Draw menu title
    p.setPen(Qt::white);
    p.setFont(QFont("Arial", 20, QFont::Bold));
    p.drawText(winBox, Qt::AlignCenter, "YOU WIN!");

    // Draw buttons
    p.setBrush(QColor(40, 40, 140));
    p.setPen(Qt::NoPen);

    p.drawRoundedRect(menuButton, 10, 10);
    p.drawRoundedRect(quitButton,   10, 10);

    p.setPen(Qt::white);
    p.setFont(QFont("Arial", 12, QFont::Bold));

    p.drawText(menuButton, Qt::AlignCenter, "Main Menu");
    p.drawText(quitButton,   Qt::AlignCenter, "Quit");
}

void WinScreen::mousePressEvent(QMouseEvent* event) { //check for button presses
    {
        QPointF pos = event->localPos();
        if (menuButton.contains(pos)) {
            emit returnToMenu();
        }
        if (quitButton.contains(pos)) {
            m_timer.stop();       // stop any running timers
            this->hide();         // clear the widget from the screen
            QApplication::processEvents();  // ensure pending paint/hide events complete
            QApplication::quit();
        }
    }
}

void WinScreen::onTick() {
    qint64 ms = m_clock.elapsed();

    float speed = 0.004f;
    float amplitude = 40.0f;

    for (size_t i = 0; i < m_bricks.size(); ++i) {
        Brick& b = m_bricks[i];

        float t = ms * speed + i * 0.3f;   // stagger bricks to create wave effect
        float hueOffset = amplitude * std::sin(t);

        int hue = static_cast<int>(b.baseHue + hueOffset);
        hue = (hue % 360 + 360) % 360;     // safe wrap-around

        b.color = QColor::fromHsv(hue, 255, 255);
    }

    update();
}

void WinScreen::resetScreen() {
    initBricks();
}

void WinScreen::initBricks() {
    m_bricks.clear();

    int rows = 15;
    int cols = 9;
    float bw = 80;
    float bh = 20;
    float gap = 4;
    float offsetX = 20;
    float offsetY = 60;

    int total = rows * cols;
    int index = 0;

    for (int r = 0; r < rows; ++r) {
        for (int c =0; c < cols; ++c) {
            float x = offsetX + c * (bw + gap);
            float y = offsetY + r * (bh + gap);

            int baseHue = (index * 360) / total; //colors ordered for rainbow effect

            Brick b;
            b.rect = QRectF(x, y, bw, bh);
            b.baseHue = baseHue;

            b.color = QColor::fromHsv(baseHue, 255, 255);

            m_bricks.push_back(b);
            index++;
        }
    }
}

