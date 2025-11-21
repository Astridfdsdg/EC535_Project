#include "block.h"
#include <stdlib.h>


struct block* createBlock(unsigned int height, unsigned int width, int posx, int posy) {
	struct block* newBlock = malloc(sizeof(struct block));

	if(newBlock != NULL) {

		newBlock->height = height;
		newBlock->width = width;
		newBlock->posx = posx;
		newBlock->posy = posy;
	}

	return newBlock;
}

void removeBlock(struct block* myblock) {
	free(myblock);
}
