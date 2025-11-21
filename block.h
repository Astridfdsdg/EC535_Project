#ifndef BLOCK_H
#define BLOCK_H

struct block {
    int posx;
    int posy;
    unsigned int height;
    unsigned int width;
};

struct block* createBlock(unsigned int height, unsigned int width, int posx, int posy);

void removeBlock(struct block* myblock);

#endif
