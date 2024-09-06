#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#ifndef SWAPELEM_H
#define SWAPELEM_H

void swapelem(image *img, selection *select)
{
    //realizeaza interschimbarea intre dimensiuni
	int temp = img->height;
	img->height = img->width;
	img->width = temp;

    //actualizez selectia
	select->x1 = 0;
	select->y1 = 0;
	select->x2 = img->width;
	select->y2 = img->height;
}

#endif
