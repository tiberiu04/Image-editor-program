#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#include "allocatememory.h"
#ifndef READP5_H
#define READP5_H

void readp5(FILE *file, image *img)
{
	int m = img->height;
	int n = img->width;
    //aloc memorie pixelilor imaginii pgm
	allocatememory(&img->data, m, n);

	unsigned char data;
    //citesc din fisier pixelii
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			fread(&data, sizeof(unsigned char), 1, file);
			img->data[i][j] = (int)data;
		}
	}
}

#endif
