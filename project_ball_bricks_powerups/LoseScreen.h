#ifndef LOSESCREEN_H
#define LOSESCREEN_H

#pragma once

#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>
#include <vector>

class LoseScreen : public QWidget {
    Q_OBJECT
public:
    explicit LoseScreen(int level = 1, QWidget* parent=nullptr);

signals:
    void returnToMenu();
    void retryLevel(int level);

protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent* event) override;  // tap to launch

private slots:
    void onTick();

private:
    void resetScreen();
    void initBricks();

    int m_level = 1;

    QRectF    m_bounds = QRectF(0, 0, 800, 480);

    QRectF    menuButton;
    QRectF    retryButton;
    QRectF    quitButton;

    QTimer m_timer;
    QElapsedTimer m_clock;

    //  bricks
    struct Brick {
        QRectF rect;
        QColor color;
        int baseHue;
    };

    std::vector<Brick> m_bricks;
};
#endif // LOSESCREEN_H
