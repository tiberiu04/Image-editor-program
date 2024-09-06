#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "allocatedouble.h"
#include "structs.h"
#ifndef APPLYEDGE_H
#define APPLYEDGE_H

void applyedge(double ****temp, selection *select, image *img)
{
	double kernel[3][3];
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			kernel[i][j] = -1;
	kernel[1][1] = 8;//kernelul standard
	double red, green, blue;
	int m = img->height;
	int n = img->width;
	for (int i = select->y1; i < select->y2; i++)
		for (int j = select->x1; j < select->x2; j++) {
			if (i > 0 && i < m - 1 && j > 0 && j < n - 1) { //aplic filtrul
				red = (*temp)[i - 1][j - 1][0] * kernel[0][0] +
								(*temp)[i - 1][j][0] * kernel[0][1] +
								(*temp)[i - 1][j + 1][0] * kernel[0][2] +
								(*temp)[i][j - 1][0] * kernel[1][0] +
								(*temp)[i][j][0] * kernel[1][1] +
								(*temp)[i][j + 1][0] * kernel[1][2] +
								(*temp)[i + 1][j - 1][0] * kernel[2][0] +
								(*temp)[i + 1][j][0] * kernel[2][1] +
								(*temp)[i + 1][j + 1][0] * kernel[2][2];
				if (red > 255)
					red = 255;
				if (red < 0)
					red = 0;
				img->rgb[i][j].red = (int)round(red);
			}
		}
	for (int i = select->y1; i < select->y2; i++)
		for (int j = select->x1; j < select->x2; j++) {
			if (i > 0 && i < m - 1 && j > 0 && j < n - 1) {//aplic filtrul
				green = (*temp)[i - 1][j - 1][1] * kernel[0][0] +
								(*temp)[i - 1][j][1] * kernel[0][1] +
								(*temp)[i - 1][j + 1][1] * kernel[0][2] +
								(*temp)[i][j - 1][1] * kernel[1][0] +
								(*temp)[i][j][1] * kernel[1][1] +
								(*temp)[i][j + 1][1] * kernel[1][2] +
								(*temp)[i + 1][j - 1][1] * kernel[2][0] +
								(*temp)[i + 1][j][1] * kernel[2][1] +
								(*temp)[i + 1][j + 1][1] * kernel[2][2];
				if (green > 255)
					green = 255;
				if (green < 0)
					green = 0;
				img->rgb[i][j].green = (int)round(green);
			}
		}
	for (int i = select->y1; i < select->y2; i++)
		for (int j = select->x1; j < select->x2; j++) {
			if (i > 0 && i < m - 1 && j > 0 && j < n - 1) { //aplic filtrul
				blue = (*temp)[i - 1][j - 1][2] * kernel[0][0] +
								(*temp)[i - 1][j][2] * kernel[0][1] +
								(*temp)[i - 1][j + 1][2] * kernel[0][2] +
								(*temp)[i][j - 1][2] * kernel[1][0] +
								(*temp)[i][j][2] * kernel[1][1] +
								(*temp)[i][j + 1][2] * kernel[1][2] +
								(*temp)[i + 1][j - 1][2] * kernel[2][0] +
								(*temp)[i + 1][j][2] * kernel[2][1] +
								(*temp)[i + 1][j + 1][2] * kernel[2][2];
				if (blue > 255)
					blue = 255;
				if (blue < 0)
					blue = 0;
				img->rgb[i][j].blue = (int)round(blue);
			}
		}
}

#endif
