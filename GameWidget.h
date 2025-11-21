#pragma once
#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>

class GameWidget : public QWidget {
    Q_OBJECT
public:
    explicit GameWidget(QWidget* parent=nullptr);
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

    // timing
    QTimer        m_timer;        // render/update at ~60 FPS
    QElapsedTimer m_clock;
    float         m_accum = 0.f;
    const float   m_dt = 1.f/120.f; // physics step ~120 Hz

    // logical playfield
    QRectF m_bounds = QRectF(0,0,800,480); // adjust to LCD logical size
    // paddle
    QRectF m_paddle = QRectF(360, 440, 120, 16);
    // ball
    QPointF m_ballPos = QPointF(420, 420);
    QPointF m_ballVel = QPointF(200, -260); // px/s
    float   m_ballR   = 8.f;
    bool    m_ballAttached = true; // sits on paddle until tap
};