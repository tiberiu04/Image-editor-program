#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#ifndef FREEIMAGE_H
#define FREEIMAGE_H

void freeimage(image *img)
{
	// verific daca exista o imagine in memorie
	if (img->height && img->width) {
		int m = img->height;
		// daca exista o imagine pgm o eliberez
		if (img->data) {
			for (int i = 0; i < m; i++)
				free(img->data[i]);
			free(img->data);
			img->data = NULL;
		}
		// daca exista o imagine ppm o eliberez
		if (img->rgb) {
			for (int i = 0; i < m; i++)
				free(img->rgb[i]);
			free(img->rgb);
			img->rgb = NULL;
		}
		// actualizez datele dupa eliberare
		img->height = 0;
		img->width = 0;
		img->maxval = 0;
	}
}

#endif
