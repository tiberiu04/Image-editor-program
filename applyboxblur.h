#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "allocatedouble.h"
#include "structs.h"
#ifndef APPLYBOXBLUR_H
#define APPLYBOXBLUR_H

void applyboxblur(double ****temp, selection *select, image *img)
{
	double kernel[3][3];
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			kernel[i][j] = 1; // kernelul standard
	double red, green, blue;
	int m = img->height;
	int n = img->width;
	for (int i = select->y1; i < select->y2; i++)
		for (int j = select->x1; j < select->x2; j++) {
			// verific daca are 8 vecini
			if (i > 0 && i < m - 1 && j > 0 && j < n - 1) { // aplicare filtru
				red = (*temp)[i - 1][j - 1][0] * kernel[0][0] +
								 (*temp)[i - 1][j][0] * kernel[0][1] +
								 (*temp)[i - 1][j + 1][0] * kernel[0][2] +
								 (*temp)[i][j - 1][0] * kernel[1][0] +
								 (*temp)[i][j][0] * kernel[1][1] +
								 (*temp)[i][j + 1][0] * kernel[1][2] +
								 (*temp)[i + 1][j - 1][0] * kernel[2][0] +
								 (*temp)[i + 1][j][0] * kernel[2][1] +
								 (*temp)[i + 1][j + 1][0] * kernel[2][2];
				red /= 9;
				img->rgb[i][j].red = (int)round(red);
			}
		}

	for (int i = select->y1; i < select->y2; i++)
		for (int j = select->x1; j < select->x2; j++) {
			// verific daca are 8 vecini
			if (i > 0 && i < m - 1 && j > 0 && j < n - 1) { // aplicare filtru
				green = (*temp)[i - 1][j - 1][1] * kernel[0][0] +
								 (*temp)[i - 1][j][1] * kernel[0][1] +
								 (*temp)[i - 1][j + 1][1] * kernel[0][2] +
								 (*temp)[i][j - 1][1] * kernel[1][0] +
								 (*temp)[i][j][1] * kernel[1][1] +
								 (*temp)[i][j + 1][1] * kernel[1][2] +
								 (*temp)[i + 1][j - 1][1] * kernel[2][0] +
								 (*temp)[i + 1][j][1] * kernel[2][1] +
								 (*temp)[i + 1][j + 1][1] * kernel[2][2];
				green /= 9;
				img->rgb[i][j].green = (int)round(green);
			}
		}

	for (int i = select->y1; i < select->y2; i++)
		for (int j = select->x1; j < select->x2; j++) {
			// verific ca are 8 vecini
			if (i > 0 && i < m - 1 && j > 0 && j < n - 1) { // aplicare filtru
				blue = (*temp)[i - 1][j - 1][2] * kernel[0][0] +
								 (*temp)[i - 1][j][2] * kernel[0][1] +
								 (*temp)[i - 1][j + 1][2] * kernel[0][2] +
								 (*temp)[i][j - 1][2] * kernel[1][0] +
								 (*temp)[i][j][2] * kernel[1][1] +
								 (*temp)[i][j + 1][2] * kernel[1][2] +
								 (*temp)[i + 1][j - 1][2] * kernel[2][0] +
								 (*temp)[i + 1][j][2] * kernel[2][1] +
								 (*temp)[i + 1][j + 1][2] * kernel[2][2];
				blue /= 9;
				img->rgb[i][j].blue = (int)round(blue);
			}
		}
}

#endif
