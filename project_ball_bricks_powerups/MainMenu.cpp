#include "MainMenu.h"
#include "GameWidget.h"
#include "LoseScreen.h"
#include "WinScreen.h"
#include <QPainter>
#include <QApplication>
#include <QMouseEvent>
#include <QLinearGradient>
#include <QPen>
#include <cmath>

MainMenu::MainMenu(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_AcceptTouchEvents, true);
    connect(&m_timer, &QTimer::timeout, this, &MainMenu::onTick);
    resetMenu();
    m_timer.start(16);
    m_clock.start();
}

void MainMenu::resetMenu() {
    initBricks();
}

void MainMenu::initBricks() {
    m_bricks.clear();

    int rows = 5;
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

            int baseHue = (index * 360) / total;

            Brick b;
            b.rect = QRectF(x, y, bw, bh);
            b.baseHue = baseHue;

            b.color = QColor::fromHsv(baseHue, 255, 255);

            m_bricks.push_back(b);
            index++;
        }
    }
}

void MainMenu::paintEvent(QPaintEvent*) {
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

    QRectF titleBox(tcx, titleY , tbw, tbh);

    const int bw = 120;
    const int bh = 28;
    const int gap = 10;

    float cx = (width() - bw) / 2;
    float firstY = titleBox.bottom() + 40;

    level1Button = QRectF(cx, firstY + 0*(bh + gap), bw, bh);
    level2Button = QRectF(cx, firstY + 1*(bh + gap), bw, bh);
    level3Button = QRectF(cx, firstY + 2*(bh + gap), bw, bh);
    quitButton   = QRectF(cx, firstY + 3*(bh + gap), bw, bh);

    p.setBrush(QColor(40, 40, 40, 200));
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(titleBox, 10, 10);
    QPen pen(Qt::white);      // white frame
    pen.setWidth(3);           // thickness of the border
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);   // don’t fill again
    p.drawRoundedRect(titleBox, 10, 10);

    // Draw menu title
    p.setPen(Qt::white);
    p.setFont(QFont("Arial", 20, QFont::Bold));
    p.drawText(titleBox, Qt::AlignCenter, "BRICK BREAKER");

    // Draw buttons
    p.setBrush(QColor(40, 40, 140));
    p.setPen(Qt::NoPen);

    p.drawRoundedRect(level1Button, 10, 10);
    p.drawRoundedRect(level2Button, 10, 10);
    p.drawRoundedRect(level3Button, 10, 10);
    p.drawRoundedRect(quitButton,   10, 10);

    p.setPen(Qt::white);
    p.setFont(QFont("Arial", 12, QFont::Bold));

    p.drawText(level1Button, Qt::AlignCenter, "Level 1");
    p.drawText(level2Button, Qt::AlignCenter, "Level 2");
    p.drawText(level3Button, Qt::AlignCenter, "Level 3");
    p.drawText(quitButton,   Qt::AlignCenter, "Quit");
}

void MainMenu::mousePressEvent(QMouseEvent* event) {
    {
        QPointF pos = event->localPos();

        if (level1Button.contains(pos)) {
            startLevel1();
            return;
        }
        if (level2Button.contains(pos)) {
            startLevel2();
            return;
        }
        if (level3Button.contains(pos)) {
            startLevel3();
            return;
        }
        if (quitButton.contains(pos)) {
            quitGame(); 
	    return;
        }
    }
}


void MainMenu::onTick() {
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

void MainMenu::startLevel1() {
    startLevel(1);
}

void MainMenu::startLevel2() {
    startLevel(2);
}

void MainMenu::startLevel3() {
    startLevel(3);
}

void MainMenu::startLevel(int level) {
    this->hide();

    GameWidget* game = new GameWidget(level);
    game->showFullScreen();

    connect(game, &GameWidget::loseScreen, this, [this, game](int level) {

        game->hide();
        game->deleteLater();

        LoseScreen* lose = new LoseScreen(level, nullptr);
        lose->showFullScreen();

        connect(lose, &LoseScreen::retryLevel, this, [this, lose](int level){
            lose->hide();
            lose->deleteLater();
            startLevel(level);    // generic startLevel(int) function
        });

        connect(lose, &LoseScreen::returnToMenu, this, [this, lose](){
            lose->hide();
            lose->deleteLater();
            this->show();         // show MainMenu
        });
    });

    connect(game, &GameWidget::winScreen, this, [this, game]() {

        game->hide();
        game->deleteLater();

        WinScreen* win = new WinScreen(nullptr);
        win->showFullScreen();

        connect(win, &WinScreen::returnToMenu, this, [this, win](){
            win->hide();
            win->deleteLater();
            this->show();         // show MainMenu
        });
    });
}

void MainMenu::quitGame() {
    m_timer.stop();       // stop any running timers
    this->hide();         // clear the widget from the screen
    QApplication::processEvents();  // ensure pending paint/hide events complete
    QApplication::quit();
}

