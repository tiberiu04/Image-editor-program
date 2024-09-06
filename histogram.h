#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "allocvector.h"
#ifndef HISTOGRAM_H
#define HISTOGRAM_H

void histogram(image img)
{
	int num, x, y = 0, z = 0, stars;
	char input[50];

	// citesc x si y
	fgets(input, sizeof(input), stdin);

	// pentru verificare daca nu este si ceva in plus
	num = sscanf(input, "%d %d %d", &x, &y, &z);

	// verific daca exista o imagine in memorie
	if (!img.data && !img.rgb) {
		printf("No image loaded");
		printf("\n");
	} else {
		// verific daca s-a citit ce trebuie
		if (num != 2 || y < 2 || y > 256 || ((y & (y - 1)) != 0)) {
			printf("Invalid command");
			printf("\n");
		} else {
			// verific daca imaginea este alb-negru
			if (img.magic[1] != '2' && img.magic[1] != '5') {
				printf("Black and white image needed");
				printf("\n");
			} else {
				int *frequency, mx = -1, *bins;
				allocvector(&frequency); // aloc un vector de frecventa
				for (int i = 0; i < img.height; i++)
					for (int j = 0; j < img.width; j++)
						frequency[img.data[i][j]]++; // actualizez frecventa
				allocvector(&bins);	// aloc in memorie un vector de bin-uri
				for (int i = 0; i < y; i++) {
					bins[i] = 0;
					// calculez pentru fiecare bin frecventa totala
					for (int j = 0; j < 256 / y; j++)
						bins[i] = bins[i] + frequency[i * (256 / y) + j];
				}

				// determin frecventa maxima din bins
				for (int i = 0; i < 256; i++)
					if (bins[i] > mx)
						mx = bins[i];

				// calculez numarul de stele si afisez histograma
				for (int i = 0; i < y; i++) {
					stars = (bins[i] * x) / mx;
					printf("%d\t|\t", stars);
					for (int j = 0; j < stars; j++)
						printf("*");
					printf("\n");
				}

				free(frequency);
				free(bins);
			}
		}
	}
}

#endif
