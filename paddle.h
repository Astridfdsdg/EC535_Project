#ifndef PADDLE_H
#define PADDLE_H

struct paddle {
    int posx;
    int posy;
    unsigned int height;
    unsigned int width;
};

struct paddle* createPaddle(unsigned int height, unsigned int width, int posx, int posy);

void movePaddle(struct paddle* paddle, int posx, int posy);

void removePaddle(struct paddle* mypaddle);

#endif
