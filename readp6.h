#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#include "allocatepixelmatrix.h"
#ifndef READP6_H
#define READP6_H

void readp6(FILE *file, image *img)
{
	int m = img->height;
	int n = img->width;
	unsigned char red, green, blue;
    //aloc memorie pentru imagini ppm
	img->rgb = allocatepixelmatrix(m, n);

    //citesc din fisier caracter cu caracter pixelii ppm
	for (int i = 0; i < m; i++)
		for (int j = 0; j < n; j++) {
			fread(&red, sizeof(unsigned char), 1, file);
			fread(&green, sizeof(unsigned char), 1, file);
			fread(&blue, sizeof(unsigned char), 1, file);
			img->rgb[i][j].red = (int)red;
			img->rgb[i][j].green = (int)green;
			img->rgb[i][j].blue = (int)blue;
		}
}

#endif
