#pragma once

#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>
#include <vector>

class GameWidget : public QWidget {
    Q_OBJECT
public:
    explicit GameWidget(int level = 1, QWidget* parent=nullptr);

signals:
    void winScreen();
    void loseScreen(int level);

protected:
    void paintEvent(QPaintEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;   // finger drag
    void mousePressEvent(QMouseEvent*) override;  // tap to launch

private slots:
    void onTick();

private:
    void resetWorld();
    void stepPhysics(float dt);
    void clampPaddle();
    void initBricks();          //initialize bricks display
    void resetBall();          // reset balls for a new life
    void grantMultiball();     // create an extra ball

    int m_level = 1;

    QTimer        m_timer;        // render/update at ~60 FPS
    QElapsedTimer m_clock;
    float         m_accum = 0.f;
    const float   m_dt = 1.f/120.f; // physics step ~120 Hz

 
    QRectF m_bounds = QRectF(0,0,800,480); // logical coords

    QRectF m_paddle = QRectF(360, 440, 120, 16);

    //  balls 
    struct Ball {
        QPointF pos;
        QPointF vel;
        bool    attached;   // sits on paddle until launch
    };
    std::vector<Ball> m_balls;
    float   m_ballR = 8.f;

    //  power-ups 
    enum class PowerType {
        PaddleSize,
        MultiBall
    };

    QRectF    m_powerUpRect;            // falling block
    bool      m_powerUpActive = false;
    PowerType m_powerUpType  = PowerType::PaddleSize;

    bool      m_paddlePowerUsedThisLife = false;
    bool      m_multiPowerUsedThisLife  = false;

    int       m_lives = 3;

    //  bricks 
    struct Brick {
        QRectF rect;
        QColor color;
        int    hits;   // how many hits remaining
    };

    std::vector<Brick> m_bricks;
};
