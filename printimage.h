#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#ifndef PRINTIMAGE_H
#define PRINTIMAGE_H

void printimage(image *img, FILE *file)
{
	fprintf(file, "%s", img->magic);
	fprintf(file, "\n");
	fprintf(file, "%d", img->width);
	fprintf(file, " ");
	fprintf(file, "%d", img->height);
	fprintf(file, "\n");
	fprintf(file, "%d", img->maxval);
	fprintf(file, "\n");
}

#endif
