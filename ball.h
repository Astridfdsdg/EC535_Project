#ifndef BALL_H
#define BALL_H

struct ball {
    int posx;
    int posy;
    int speedx;
    int speedy;
    int accx;
    int accy;
    unsigned int radius;
};

struct ball* createBall(unsigned int radius, int posx, int posy);
void removeBall(struct ball* myball);

void setBallSpeed(struct ball* ball, int speedx, int speedy);
void setBallAcc(struct ball* ball, int accx, int accy);

void moveBall(struct ball* ball);

#endif
