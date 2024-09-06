#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "allocatememory.h"
#include "allocatepixelmatrix.h"
#ifndef CROP_H
#define CROP_H

void crop(selection *select, image *img)
{
	int m = select->y2 - select->y1;
	int n = select->x2 - select->x1;

	// verific daca exista o imagine
	if (!img->rgb && !img->data) {
		printf("No image loaded");
		printf("\n");
	} else {
		if (img->magic[1] == '2' || img->magic[1] == '5') {
			int **temp = NULL;

			allocatememory(&temp, m, n);
			// in temp transfer elementele care apartin selectiei
			for (int i = 0; i < m; i++)
				for (int j = 0; j < n; j++)
					temp[i][j] = img->data[i + select->y1][j + select->x1];

			for (int i = 0; i < img->height; i++)
				free(img->data[i]);
			free(img->data);

			img->height = m;
			img->width = n;

			allocatememory(&img->data, m, n);

			// actualizez imaginea
			for (int i = 0; i < m; i++)
				for (int j = 0; j < n; j++)
					img->data[i][j] = temp[i][j];
			// eliberez matricea temporara
			for (int i = 0; i < m; i++)
				free(temp[i]);
			free(temp);
		} else {
			pixel **temp = allocatepixelmatrix(m, n);
			// in temp transfer elementele care apartin selectiei
			for (int i = 0; i < m; i++)
				for (int j = 0; j < n; j++)
					temp[i][j] = img->rgb[i + select->y1][j + select->x1];

			for (int i = 0; i < img->height; i++)
				free(img->rgb[i]);
			free(img->rgb);

			img->height = m;
			img->width = n;

			img->rgb = allocatepixelmatrix(m, n);
			// actualizez matricea cu pixelii din selectie
			for (int i = 0; i < m; i++)
				for (int j = 0; j < n; j++)
					img->rgb[i][j] = temp[i][j];
			// eliberez matricea temporara
			for (int i = 0; i < m; i++)
				free(temp[i]);
			free(temp);
		}
		// actualizez dimensiunile si selectia
		img->height = m;
		img->width = n;
		select->x1 = 0;
		select->y1 = 0;
		select->x2 = img->width;
		select->y2 = img->height;
		// confirm ca operatia a avut loc cu succes
		printf("Image cropped\n");
	}
}

#endif
