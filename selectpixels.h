#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#ifndef SELECTPIXELS_H
#define SELECTPIXELS_H

void selectpixels(image img, selection *select, int x1, int y1, int x2, int y2)
{
	int aux;
	// verific daca x1 > x2 si realizez swap ul
	if (x1 > x2) {
		aux = x1;
		x1 = x2;
		x2 = aux;
	}

	// verific daca y1 > y2 si realizez swap ul
	if (y1 > y2) {
		aux = y1;
		y1 = y2;
		y2 = aux;
	}

	// verific daca exista o imagine sau daca selectia exista in imagine
	if (!img.data && !img.rgb) {
		printf("No image loaded");
		printf("\n");
	} else if (x1 < 0 || y1 < 0 || x2 > img.width || y2 > img.height) {
		printf("Invalid set of coordinates");
		printf("\n");
	} else if (x1 == x2 || y1 == y2) {
		printf("Invalid set of coordinates");
		printf("\n");
	} else {
		// actualizez selectia daca este valida
		select->x1 = x1;
		select->y1 = y1;
		select->x2 = x2;
		select->y2 = y2;

		// confirm ca selectia s-a realizat cu succes
		printf("Selected %d %d %d %d\n", x1, y1, x2, y2);
	}
}

#endif
