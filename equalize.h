#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "allocvector.h"
#include <math.h>
#ifndef EQUALIZE_H
#define EQUALIZE_H

void equalize(image *img)
{
	int *frequency;

	// verific daca exista o imagine in memorie
	if (!img->data && !img->rgb) {
		printf("No image loaded");
		printf("\n");
	} else {
		// verific daca este o imagine alb-negru
		if (img->magic[1] != '2' && img->magic[1] != '5') {
			printf("Black and white image needed");
			printf("\n");
		} else {
			allocvector(&frequency); // aloc vector pentru frecvente

			// calculez frecventele elementelor
			for (int i = 0; i < img->height; i++)
				for (int j = 0; j < img->width; j++)
					frequency[img->data[i][j]]++;

			// aloc dinamic in memorie vectorul pentru sumele partiale
			double *sums = (double *)calloc(256, sizeof(double));
			if (!sums) {
				fprintf(stderr, "malloc failed\n");
				return;
			}

			sums[0] = frequency[0];

			// calculez sumele partiale ale frecventelor
			for (int i = 1; i < 256; i++)
				sums[i] = sums[i - 1] + frequency[i];

			double x;
			double area = img->height * img->width; // aria

			// egalizarea imaginii dupa formula din enunt
			for (int i = 0; i < img->height; i++) {
				for (int j = 0; j < img->width; j++) {
					x = ((double)255) * sums[img->data[i][j]] / area;
					if (x > 255)
						x = 255;
					if (x < 0)
						x = 0;
					int y = (int)round(x);
					img->data[i][j] = y;
				}
			}

			// eliberez vectorii utilizati
			free(frequency);
			free(sums);

			// confirm ca operatia s-a realizat cu succes
			printf("Equalize done\n");
		}
	}
}

#endif
