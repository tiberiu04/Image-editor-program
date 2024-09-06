#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#ifndef SELECTALL_H
#define SELECTALL_H

void selectall(image img, selection *selection)
{
	// verific daca este o imagine incarcata
	if (!img.data && !img.rgb) {
		printf("No image loaded\n");
		return;
	}

	// actualizez selectia
	selection->x1 = 0;
	selection->y1 = 0;
	selection->x2 = img.width;
	selection->y2 = img.height;
	selection->selected = 1;
	// confirm ca selectia a fost cu succes
	printf("Selected ALL\n");
}

#endif
