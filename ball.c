#include "ball.h"
#include <stdlib.h>


struct ball* createBall(unsigned int radius, int posx, int posy) {
	struct ball* newBall = malloc(sizeof(struct ball));

	if(newBall != NULL) {

		newBall->radius = radius;
		newBall->posx = posx;
		newBall->posy = posy;
		newBall->speedx = 0;
		newBall->speedy = 0;
		newBall->accx = 0;
		newBall->accy = 0;
	}
	
	return newBall;
}

void removeBall(struct ball* myball) {
	free(myball);
}

void setBallSpeed(struct ball* ball, int speedx, int speedy) {
	if(ball != NULL) {
		ball->speedx = speedx;
		ball->speedy = speedy;
	}
}

void setBallAcc(struct ball* ball, int accx, int accy) {
	if(ball != NULL) {
		ball->accx = accx;
		ball->accy = accy;
	}
}

void moveBall(struct ball* ball) {
	if(ball != NULL) {
		ball->posx += ball->speedx;
		ball->posy += ball->speedy;
		ball->speedx += ball->accx;
		ball->speedy += ball->accy;
	}
}


