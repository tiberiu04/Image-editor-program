#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#include "rotate90.h"
#include "rotatepixels.h"
#include "rotatewhole.h"
#include "rotatergb.h"
#include "swapelem.h"
#ifndef ROTATE_H
#define ROTATE_H

void rotate(image *img, selection *select)
{
	int angle;
	int x, y;
	x = select->y2 - select->y1;
	y = select->x2 - select->x1;

	scanf("%d", &angle);
	if (!img->data && !img->rgb) { // verific existenta imaginii
		printf("No image loaded");
		printf("\n");
	} else if (angle % 90 != 0) { // verific daca e unghi valid
		printf("Unsupported rotation angle");
		printf("\n");
	} else if ((angle > 360 || angle < -360) && angle % 90 == 0) {
		printf("Unsupported rotation angle");
		printf("\n");
	} else if (x != y && x != img->height && y != img->width) {
		printf("The selection must be square");
		printf("\n"); // am verificat daca selectia e patrat dar != imagine
	} else {
		if (x == y && x > 0) { // pentru selectie patrat
			if (img->magic[1] == '2' || img->magic[1] == '5') { // imagine pgm
				if (angle >= 0 && angle < 360) {
					for (int i = 0; i < angle / 90; i++)
						rotate90(&img->data, select); // rotesc la dreapta
				} else if (angle < 0 && angle > -360) {
					for (int i = 0; i < 4 + angle / 90; i++)
						rotate90(&img->data, select); // rotire unghi negativ
				}
			}
			if (img->magic[1] == '3' || img->magic[1] == '6') { // imagine ppm
				if (angle >= 0 && angle < 360) {
					for (int i = 0; i < angle / 90; i++)
						rotatepixels(&img->rgb, select); // rotesc la dreapta
				} else if (angle < 0 && angle > -360) {
					for (int i = 0; i < 4 + angle / 90; i++)
						rotatepixels(&img->rgb, select); // rotire unghi negativ
				}
			}
		} else if (x == img->height && y == img->width) { // pt toata imaginea
			if (img->magic[1] == '2' || img->magic[1] == '5') { // imagine pgm
				if (angle >= 0 && angle < 360) {
					for (int i = 0; i < angle / 90; i++) {
						rotatewhole(&img->data, &x, &y); // rotire la dreapta
						swapelem(img, select);
					}
				} else if (angle < 0 && angle > -360) {
					for (int i = 0; i < 4 + angle / 90; i++) {
						rotatewhole(&img->data, &x, &y); // rotire la stanga
						swapelem(img, select);
					}
				}
			}
			if (img->magic[1] == '3' || img->magic[1] == '6') { // imagine ppm
				if (angle >= 0 && angle < 360) {
					for (int i = 0; i < angle / 90; i++) {
						rotatergb(&img->rgb, &x, &y); // rotire la dreapta
						swapelem(img, select);
					}
				} else if (angle < 0 && angle > -360) {
					for (int i = 0; i < 4 + angle / 90; i++) {
						rotatergb(&img->rgb, &x, &y); // rotire la stanga
						swapelem(img, select);
					}
				}
			}
		}

		// Rotirea pentru unghi negativ se realizeaza astfel: din 360 se scade
		// unghiul respectiv si se obtine unul pozitiv care este echivalentul
		// celui negatic. Mai precis o rotire la -90 este echivalenta cu una
		// la 270.

		// confirm ca operatia s-a realizat cu succes
		printf("Rotated %d\n", angle);
	}
}

#endif
