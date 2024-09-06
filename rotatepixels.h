#include <stdio.h>
#include <stdlib.h>
#include "allocatepixelmatrix.h"
#include "structs.h"
#ifndef ROTATEPIXELS_H
#define ROTATEPIXELS_H

void rotatepixels(pixel ***a, selection *select)
{
	int x = select->x2 - select->x1;
	pixel **temp;

	// aloc dinamic o matrice temporara
	temp = allocatepixelmatrix(x, x);

	// initializez matricea temporara cu pixelii din imaginea curenta
	for (int i = 0; i < x; i++)
		for (int j = 0; j < x; j++)
			temp[i][j] = (*a)[i + select->y1][j + select->x1];

	// realizez rotatia
	for (int i = 0; i < x / 2; i++) {
		for (int j = i; j < x - i - 1; j++) {
			pixel aux = temp[i][j];
			temp[i][j] = temp[x - 1 - j][i];
			temp[x - 1 - j][i] = temp[x - 1 - i][x - 1 - j];
			temp[x - 1 - i][x - 1 - j] = temp[j][x - 1 - i];
			temp[j][x - 1 - i] = aux;
		}
	}

	// actualizez pixelii din imaginea curenta
	for (int i = select->y1; i < select->y2; i++)
		for (int j = select->x1; j < select->x2; j++) {
			pixel x = temp[i - select->y1][j - select->x1];
			(*a)[i][j] = x;
		}

	// eliberez matricea temporara
	for (int i = 0; i < x; i++)
		free(temp[i]);
	free(temp);
}

#endif
