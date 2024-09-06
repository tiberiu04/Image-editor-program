#include <stdio.h>
#include <stdlib.h>
#include "allocatememory.h"
#include "structs.h"
#ifndef ROTATEWHOLE_H
#define ROTATEWHOLE_H

void rotatewhole(int ***a, int *m, int *n)
{
	int **rotated;
	allocatememory(&rotated, *n, *m);

    //rotated contine pixelii dupa rotatie cu 90 de grade spre dreapta
	for (int i = 0; i < *m; i++)
		for (int j = 0; j < *n; j++)
			rotated[j][*m - 1 - i] = (*a)[i][j];

	for (int i = 0; i < *m; i++)
		free((*a)[i]);
	free(*a);

	//aloc memoria pentru matricea originala cu dimensiunile schimbate
	allocatememory(a, *n, *m);

	//actualizez imaginea
	for (int i = 0; i < *n; i++)
		for (int j = 0; j < *m; j++)
			(*a)[i][j] = rotated[i][j];

	//eliberez memoria alocata pentru matricea rotita
	for (int i = 0; i < *n; i++)
		free(rotated[i]);
	free(rotated);

	//actualizez dimensiunile matricei
	int temp = *m;
	*m = *n;
	*n = temp;
}

#endif
