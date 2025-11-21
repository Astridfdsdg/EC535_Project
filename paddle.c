#include "paddle.h"
#include <stdlib.h>


struct paddle* createPaddle(unsigned int height, unsigned int width, int posx, int posy) {
	struct paddle* newPaddle = malloc(sizeof(struct paddle));

	if(newPaddle != NULL) {

		newPaddle->height = height;
		newPaddle->width = width;
		newPaddle->posx = posx;
		newPaddle->posy = posy;
	}
	
	return newPaddle;
}

void movePaddle(struct paddle* paddle, int posx, int posy) {
	if(paddle != NULL) {
		paddle->posx = posx;
		paddle->posy = posy;
	}
}

void removePaddle(struct paddle* mypaddle) {
	free(mypaddle);
}
