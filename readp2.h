#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#include "allocatememory.h"
#ifndef READP2_H
#define READP2_H

void readp2(FILE *file, image *img)
{
	int m = img->height;
	int n = img->width;
    //aloc memorie pixelilor
	allocatememory(&img->data, m, n);
    //citesc pixelii imaginii de tip pgm
	for (int i = 0; i < m; i++)
		for (int j = 0; j < n; j++)
			fscanf(file, "%d", &img->data[i][j]);
}

#endif
