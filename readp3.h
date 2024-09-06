#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#include "allocatepixelmatrix.h"
#ifndef READP3_H
#define READP3_H

void readp3(FILE *file, image *img)
{
	int m = img->height;
	int n = img->width;
	int red, green, blue;
	// aloc memorie pentru imagini ppm
	img->rgb = allocatepixelmatrix(img->height, img->width);

	// citesc pixelii ppm
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			fscanf(file, "%d %d %d", &red, &green, &blue);
			img->rgb[i][j].red = red;
			img->rgb[i][j].green = green;
			img->rgb[i][j].blue = blue;
		}
	}
}

#endif
