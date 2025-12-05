#include "GameWidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QLinearGradient>
#include <QPen>
#include <algorithm>
#include <cmath>
#include <cstdlib>

GameWidget::GameWidget(int level, QWidget* parent)
    : QWidget(parent),
      m_level(level)
{
    setAttribute(Qt::WA_AcceptTouchEvents, true);
    resetWorld();
    connect(&m_timer, &QTimer::timeout, this, &GameWidget::onTick);
    m_timer.start(16);       // ~60 FPS
    m_clock.start();
}

void GameWidget::resetBall() {
    m_balls.clear();

    Ball b;
    b.pos = QPointF(m_paddle.center().x(), m_paddle.top() - m_ballR - 1);
    b.vel = QPointF(200, -260);
    b.attached = true;   // sits on paddle

    m_balls.push_back(b);
}



void GameWidget::resetWorld() {
    m_lives = 3;

    // Reset paddle size and position
    float w = 120;
    float h = 16;
    float cx = m_bounds.width() / 2;
    float cy = m_bounds.height() - 40 - h/2;
    m_paddle = QRectF(cx - w/2, cy - h/2, w, h);

    // bricks
    initBricks();

    // power-up state & per-life flags
    m_powerUpActive           = false;
    m_paddlePowerUsedThisLife = false;
    m_multiPowerUsedThisLife  = false;

    // balls
    resetBall();
}

void GameWidget::initBricks() {
    m_bricks.clear();

    float bw = 80;
    float bh = 20;
    float gap = 4;
    float offsetX = 20;
    float offsetY = 60;

    if (m_level == 1) {
        int rows = 4;
        int cols = 8;

        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                float x = offsetX + c * (bw + gap);
                float y = offsetY + r * (bh + gap);

                Brick b;
                b.rect = QRectF(x, y, bw, bh);

                if (r == 0) {
                    // TOP ROW = strong grey bricks, need 2 hits
                    b.hits  = 2;
                    b.color = QColor(170, 170, 170);   // base grey
                } else {
                    // normal colorful bricks, 1 hit
                    b.hits  = 1;
                    b.color = QColor::fromHsv(
                        std::rand() % 360,          // hue
                        200 + std::rand() % 55,     // saturation
                        200 + std::rand() % 55      // value (brightness)
                    );
                }

                m_bricks.push_back(b);
            }
        }
    }
    else if (m_level == 2) {
        // 5 rows, 8 cols. Top rows have a  block of grey bricks
        // centered, so visually it forms a pyramid
        
        int rows = 5;
        int cols = 8;

        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                float x = offsetX + c * (bw + gap);
                float y = offsetY + r * (bh + gap);

                Brick b;
                b.rect = QRectF(x, y, bw, bh);

                bool isGrey = false;

                if (r == 0) {
                    // row 0: grey in columns 2..5
                    if (c >= 2 && c <= 5) isGrey = true;
                } else if (r == 1) {
                    // row 1: grey in columns 1..6 (wider)
                    if (c >= 1 && c <= 6) isGrey = true;
                }

                if (isGrey) {
                    b.hits  = 2;
                    b.color = QColor(170, 170, 170);
                } else {
                    b.hits  = 1;
                    b.color = QColor::fromHsv(
                        std::rand() % 360,
                        200 + std::rand() % 55,
                        200 + std::rand() % 55
                    );
                }

                m_bricks.push_back(b);
            }
        }
    }
    else { 
        // 6 rows, 8 cols.

        int rows = 6;
        int cols = 8;

        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                float x = offsetX + c * (bw + gap);
                float y = offsetY + r * (bh + gap);

                Brick b;
                b.rect = QRectF(x, y, bw, bh);

                bool isBorder = (r == 0 || r == rows - 1 || c == 0 || c == cols - 1);

                if (isBorder) {
                    // strong grey frame
                    b.hits  = 2;
                    b.color = QColor(170, 170, 170);
                } else {
                    // inner colorful bricks
                    b.hits  = 1;
                    b.color = QColor::fromHsv(
                        std::rand() % 360,
                        200 + std::rand() % 55,
                        200 + std::rand() % 55
                    );
                }

                m_bricks.push_back(b);
            }
        }
    }
}

void GameWidget::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.fillRect(rect(), Qt::black);
    p.setRenderHint(QPainter::Antialiasing, true);

    // scale drawing to screen size
    const qreal sx = width()  / m_bounds.width();
    const qreal sy = height() / m_bounds.height();
    p.scale(sx, sy);

    //  bricks 
    p.setPen(Qt::NoPen);
    for (const Brick &b : m_bricks) {
        QLinearGradient grad(b.rect.topLeft(), b.rect.bottomRight());

        if (b.hits > 1) {
            // strong brick: more metallic grey
            QColor base = b.color; // should already be grey
            grad.setColorAt(0.0, base.lighter(140));
            grad.setColorAt(1.0, base.darker(180));
            p.setPen(QPen(QColor(230, 230, 230), 2));  // outline for strong brick
        } else {
            // normal brick
            grad.setColorAt(0.0, b.color.lighter(130));
            grad.setColorAt(1.0, b.color.darker(160));
            p.setPen(Qt::NoPen);
        }

        p.setBrush(grad);
        p.drawRoundedRect(b.rect, 4, 4);
    }
    p.setPen(Qt::NoPen); // reset pen

    // ---- paddle ----
    p.setBrush(Qt::yellow);
    p.setPen(Qt::NoPen);
    p.drawRect(m_paddle);

    // ---- balls ----
    p.setBrush(Qt::white);
    for (const Ball &b : m_balls) {
        p.drawEllipse(b.pos, m_ballR, m_ballR);
    }

    // ---- HUD ----
    bool anyAttached = false;
    for (const Ball &b : m_balls) {
        if (b.attached) { anyAttached = true; break; }
    }

    p.setPen(Qt::white);
    p.drawText(QPointF(10, 20), anyAttached ? "Tap to launch" : "Running");
    p.drawText(QPointF(10, 40), QString("Lives: %1").arg(m_lives));

    // ---- power-up ----
    if (m_powerUpActive) {
        p.setPen(Qt::NoPen);
        QLinearGradient g(m_powerUpRect.topLeft(), m_powerUpRect.bottomLeft());
        g.setColorAt(0.0, QColor(180, 255, 180));
        g.setColorAt(1.0, QColor(60, 160, 60));
        p.setBrush(g);
        p.drawRoundedRect(m_powerUpRect, 4, 4);

        p.setPen(Qt::black);
        QString text = (m_powerUpType == PowerType::PaddleSize) ? "P" : "M";
        p.drawText(m_powerUpRect, Qt::AlignCenter, text);
    }
}

void GameWidget::mouseMoveEvent(QMouseEvent* e) {
    // convert screen coords to logical coords
    const float sx = m_bounds.width() / width();
    float logicalX = e->x() * sx;

    // move paddle horizontally, clamp
    m_paddle.moveCenter(QPointF(logicalX, m_paddle.center().y()));
    clampPaddle();

    // keep any attached balls on the paddle
    for (Ball &b : m_balls) {
        if (b.attached) {
            b.pos = QPointF(m_paddle.center().x(), m_paddle.top()-m_ballR-1);
        }
    }
}

void GameWidget::mousePressEvent(QMouseEvent*) {
    // launch: any attached balls become free
    for (Ball &b : m_balls) {
        b.attached = false;
    }
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
        stepPhysics(m_dt);
        m_accum -= m_dt;
    }
    update();
}

void GameWidget::grantMultiball() {
    // do not exceed 2 balls
    if (m_balls.size() >= 2) return;
    if (m_balls.empty()) return;

    // take the first flying ball if possible
    const Ball* source = nullptr;
    for (const Ball &b : m_balls) {
        if (!b.attached) { source = &b; break; }
    }
    if (!source) {
        // all attached? use first ball
        source = &m_balls[0];
    }

    Ball extra;
    extra.pos = source->pos;
    // mirrored X velocity
    extra.vel = QPointF(-source->vel.x(), source->vel.y());
    extra.attached = false;   // extra ball is flying

    m_balls.push_back(extra);
}

void GameWidget::stepPhysics(float dt) {
    // update all balls 
    for (std::size_t i = 0; i < m_balls.size(); ) {
        Ball &ball = m_balls[i];

        // move only if not attached
        if (!ball.attached) {
            ball.pos += ball.vel * dt;
        }

        // walls
        if (ball.pos.x() - m_ballR < 0) {
            ball.pos.setX(m_ballR);
            ball.vel.setX(std::abs(ball.vel.x()));
        }
        if (ball.pos.x() + m_ballR > m_bounds.width()) {
            ball.pos.setX(m_bounds.width() - m_ballR);
            ball.vel.setX(-std::abs(ball.vel.x()));
        }
        if (ball.pos.y() - m_ballR < 0) {
            ball.pos.setY(m_ballR);
            ball.vel.setY(std::abs(ball.vel.y()));
        }

        // bottom (this ball is lost)
        if (ball.pos.y() - m_ballR > m_bounds.height()) {
            m_balls.erase(m_balls.begin() + i);

            if (m_balls.empty()) {
                // life lost only when no balls remain
                m_lives--;

                if (m_lives <= 0) {
                    emit returnToMenu();
                    this->close();
                } else {
                    resetBall();
                    // allow power-ups again on new life
                    m_paddlePowerUsedThisLife = false;
                    m_multiPowerUsedThisLife  = false;
                    m_powerUpActive           = false;
                }
                return; // stop physics this frame
            }

            // we removed this ball; do not increment i
            continue;
        }

        // paddle collision
        QRectF paddleGrow = m_paddle.adjusted(-m_ballR, -m_ballR, m_ballR, m_ballR);
        if (paddleGrow.contains(ball.pos) && ball.vel.y() > 0) {
            ball.pos.setY(m_paddle.top()-m_ballR-1);
            ball.vel.setY(-std::abs(ball.vel.y()));

            // add spin on x depending on where it hit
            float dx = (ball.pos.x() - m_paddle.center().x()) / (m_paddle.width()/2);
            ball.vel.setX( std::clamp(dx, -1.f, 1.f) * 320.f );
        }

        // brick collisions (one brick per ball per frame)
        for (auto it = m_bricks.begin(); it != m_bricks.end(); ) {
            QRectF grow = it->rect.adjusted(-m_ballR, -m_ballR, m_ballR, m_ballR);

            if (grow.contains(ball.pos)) {
                // bounce
                ball.vel.setY(-ball.vel.y());

                // decrease hits
                it->hits--;

                if (it->hits <= 0) {
                    // brick destroyed 
                    bool canDropAny = (!m_paddlePowerUsedThisLife || !m_multiPowerUsedThisLife);

                    if (!m_powerUpActive && canDropAny) {
                        int r = std::rand() % 100;  
                        if (r < 20) {               // ~20% chance to drop something
                            // choose type
                            if (!m_paddlePowerUsedThisLife && !m_multiPowerUsedThisLife) {
                                // both available → 50/50
                                if (std::rand() % 2 == 0)
                                    m_powerUpType = PowerType::PaddleSize;
                                else
                                    m_powerUpType = PowerType::MultiBall;
                            } else if (!m_paddlePowerUsedThisLife) {
                                m_powerUpType = PowerType::PaddleSize;
                            } else {
                                m_powerUpType = PowerType::MultiBall;
                            }

                            QPointF c = it->rect.center();
                            float w = 30;
                            float h = 14;
                            m_powerUpRect = QRectF(c.x() - w/2, c.y() - h/2, w, h);
                            m_powerUpActive = true;
                        }
                    }

                    it = m_bricks.erase(it);

                    if (m_bricks.empty()) {
                        emit returnToMenu();
                        this->close();
                    }
                } else {
                    // still alive 
                    it->color = it->color.lighter(120);
                    ++it;
                }

                // stop after handling one brick for this ball
                break;

            } else {
                ++it;
            }
        }

        ++i; // next ball
    }

    
    if (m_powerUpActive) {
        float vy = 80.0f;  // px/s
        m_powerUpRect.translate(0, vy * dt);

        // missed
        if (m_powerUpRect.top() > m_bounds.height()) {
            m_powerUpActive = false;
        }
        // caught by paddle
        else if (m_powerUpRect.intersects(m_paddle)) {
            if (m_powerUpType == PowerType::PaddleSize) {
                // increase paddle width
                qreal newW = m_paddle.width() * 1.25;
                if (newW > 240) newW = 240;

                qreal h   = m_paddle.height();
                qreal cy  = m_paddle.center().y();
                qreal cx  = m_paddle.center().x();

                m_paddle = QRectF(cx - newW/2, cy - h/2, newW, h);
                clampPaddle();

                m_paddlePowerUsedThisLife = true;
            } else if (m_powerUpType == PowerType::MultiBall) {
                grantMultiball();
                m_multiPowerUsedThisLife = true;
            }

            m_powerUpActive = false;
        }
    }
}
