#ifndef MAINMENU_H
#define MAINMENU_H

#pragma once

#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>
#include <vector>

class MainMenu : public QWidget {
    Q_OBJECT
public:
    explicit MainMenu(QWidget* parent=nullptr);

protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent* event) override;  // tap to launch

private slots:
    void onTick();

private:
    void resetMenu();
    void startLevel1();
    void startLevel2();
    void startLevel3();
    void quitGame();
    void initBricks();

    QRectF    m_bounds = QRectF(0, 0, 800, 480);

    QRectF    level1Button;
    QRectF    level2Button;
    QRectF    level3Button;
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


#endif // MAINMENU_H
