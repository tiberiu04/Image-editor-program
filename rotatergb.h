#include <stdio.h>
#include <stdlib.h>
#include "allocatepixelmatrix.h"
#include "structs.h"
#ifndef ROTATERGB_H
#define ROTATERGB_H

void rotatergb(pixel ***a, int *m, int *n)
{
	pixel **rotated;

    //aloc in memorie o matrice care contine pixelii dupa rotatie
	rotated = allocatepixelmatrix(*n, *m);

	for (int i = 0; i < *m; i++)
		for (int j = 0; j < *n; j++)
			rotated[j][*m - 1 - i] = (*a)[i][j];

    //eliberez matricea curenta pentru a o putea modifica
	for (int i = 0; i < *m; i++)
		free((*a)[i]);
	free(*a);

    //aloc memoria pentru matricea originala cu dimensiunile schimbate
	*a = allocatepixelmatrix(*n, *m);

    //actualizez imaginea
	for (int i = 0; i < *n; i++)
		for (int j = 0; j < *m; j++)
			(*a)[i][j] = rotated[i][j];

    //eliberez memoria alocata pentru matricea rotita
	for (int i = 0; i < *n; i++)
		free(rotated[i]);
	free(rotated);

    //actualizez dimensiunile matricii
	int temp = *m;
	*m = *n;
	*n = temp;
}

#endif
