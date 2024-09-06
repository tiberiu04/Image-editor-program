#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "applyboxblur.h"
#include "applyedge.h"
#include "applygaussianblur.h"
#include "applysharpen.h"
#include "allocatedouble.h"
#include "structs.h"
#include <math.h>
#ifndef APPLY_H
#define APPLY_H

void apply(image *img, selection select, char *command)
{
	char *format = NULL;
	char input[50];
	format = (char *)malloc(50 * sizeof(char));
	if (!format) {
		printf("NU am putut creea format\n");
		return;
	}
	// citesc filtrul
	fgets(input, sizeof(input), stdin);
	int num = sscanf(input, "%s", format);
	// verific daca exista vreo imagine in memorie
	if (!img->data && !img->rgb) {
		printf("No image loaded");
		printf("\n");
	} else {
		if (num <= 0 || strcmp(command, "APPLY") != 0) { // daca comanda e buna
			printf("Invalid command");
			printf("\n");
		} else {
			// verific daca imaginea este color
			if ((img->magic[1] == '2' || img->magic[1] == '5') && num != 0) {
				printf("Easy, Charlie Chaplin");
				printf("\n");
			} else {
				double ***temp;
				allocatedouble(&temp, img->height, img->width);
				for (int i = 0; i < img->height; i++)
					for (int j = 0; j < img->width; j++) {
						temp[i][j][0] = (double)(img->rgb[i][j].red);
						temp[i][j][1] = (double)(img->rgb[i][j].green);
						temp[i][j][2] = (double)(img->rgb[i][j].blue);
					}
				// verific ce filtru trebuie aplicat dupa ce s a citit
				if (strcmp(format, "EDGE") == 0) {
					applyedge(&temp, &select, img);
					printf("APPLY EDGE done\n");
				} else if (strcmp(format, "SHARPEN") == 0) {
					applysharpen(&temp, &select, img);
					printf("APPLY SHARPEN done\n");
				} else if (strcmp(format, "GAUSSIAN_BLUR") == 0) {
					applygaussianblur(&temp, &select, img);
					printf("APPLY GAUSSIAN_BLUR done\n");
				} else if (strcmp(format, "BLUR") == 0) {
					applyboxblur(&temp, &select, img);
					printf("APPLY BLUR done\n");
				} else {
					printf("APPLY parameter invalid\n");
					return;
				}

				for (int i = 0; i < img->height; ++i) {
					for (int j = 0; j < img->width; ++j)
						free(temp[i][j]);
					free(temp[i]);
				}
				free(temp);
			}
		}
	}
	free(format);
}

#endif
