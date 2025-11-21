#include "GameWidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <algorithm>
#include <cmath>


//This code implements the ball collision with the paddle and with the walls .

GameWidget::GameWidget(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_AcceptTouchEvents, true);
    resetWorld();
    connect(&m_timer, &QTimer::timeout, this, &GameWidget::onTick);
    m_timer.start(16);       // ~60 FPS
    m_clock.start();
}

void GameWidget::resetWorld() {
    // center paddle horizontally
    m_paddle.moveTo( (m_bounds.width()-m_paddle.width())/2, m_bounds.height()-40 );
    // put ball above paddle
    m_ballPos = QPointF(m_paddle.center().x(), m_paddle.top()-m_ballR-1);
    m_ballVel = QPointF(200, -260);
    m_ballAttached = true;
}

void GameWidget::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.fillRect(rect(), Qt::black);
    p.setRenderHint(QPainter::Antialiasing, true);

    // scale drawing to screen size
    const qreal sx = width()  / m_bounds.width();
    const qreal sy = height() / m_bounds.height();
    p.scale(sx, sy);

    // draw paddle
    p.setBrush(Qt::yellow);
    p.setPen(Qt::NoPen);
    p.drawRect(m_paddle);

    // draw ball
    p.setBrush(Qt::white);
    p.drawEllipse(m_ballPos, m_ballR, m_ballR);

    
    
}

void GameWidget::mouseMoveEvent(QMouseEvent* e) {
    // convert screen coords to logical coords
    const float sx = m_bounds.width() / width();
    float logicalX = e->x() * sx;
    
    // move paddle horizontally, clamp
    m_paddle.moveCenter(QPointF(logicalX, m_paddle.center().y()));
    clampPaddle();

    if (m_ballAttached) {
        m_ballPos = QPointF(m_paddle.center().x(), m_paddle.top()-m_ballR-1);
    }
}

void GameWidget::mousePressEvent(QMouseEvent*) {
    if (m_ballAttached) m_ballAttached = false; // launch with current m_ballVel
}

void GameWidget::clampPaddle() {
    const qreal half = m_paddle.width()/2;
    qreal cx = std::clamp<qreal>(m_paddle.center().x(), half, m_bounds.width()-half);
    m_paddle.moveCenter(QPointF(cx, m_paddle.center().y()));
}

void GameWidget::onTick() {
    float frame = m_clock.restart()/1000.f; // seconds
    m_accum += frame;
    while (m_accum >= m_dt) {
        if (!m_ballAttached) stepPhysics(m_dt);
        m_accum -= m_dt;
    }
    update();
}

void GameWidget::stepPhysics(float dt) {
  
    m_ballPos += m_ballVel * dt;

    // walls
    if (m_ballPos.x()-m_ballR < 0) { m_ballPos.setX(m_ballR); m_ballVel.setX(std::abs(m_ballVel.x())); }
    if (m_ballPos.x()+m_ballR > m_bounds.width()) { m_ballPos.setX(m_bounds.width()-m_ballR); m_ballVel.setX(-std::abs(m_ballVel.x())); }
    if (m_ballPos.y()-m_ballR < 0) { m_ballPos.setY(m_ballR); m_ballVel.setY(std::abs(m_ballVel.y())); }

    // bottom (lost ball) then reset just the ball
    if (m_ballPos.y()-m_ballR > m_bounds.height()) {
        m_ballAttached = true;
        m_ballPos = QPointF(m_paddle.center().x(), m_paddle.top()-m_ballR-1);
        m_ballVel = QPointF(200, -260);
    }

    // paddle collision 
    QRectF grow = m_paddle.adjusted(-m_ballR, -m_ballR, m_ballR, m_ballR);
    if (grow.contains(m_ballPos) && m_ballVel.y() > 0) {
        m_ballPos.setY(m_paddle.top()-m_ballR-1);
        m_ballVel.setY(-std::abs(m_ballVel.y()));
        // add spin on x depending on the hit
        float dx = (m_ballPos.x() - m_paddle.center().x()) / (m_paddle.width()/2);
        m_ballVel.setX( std::clamp(dx, -1.f, 1.f) * 320.f );
    }
}
