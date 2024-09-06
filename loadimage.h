#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#include "readp2.h"
#include "readp3.h"
#include "readp5.h"
#include "readp6.h"
#include <string.h>
#ifndef LOADIMAGE_H
#define LOADIMAGE_H

void loadimage(char *filename, image *img)
{
	// deschid fisierul
	FILE *file = fopen(filename, "rb");
	// verific existenta fisierului
	if (!file) {
		printf("Failed to load %s\n", filename);
		img->rgb = NULL;
		img->data = NULL;
		return;
	}
	char line[1000];

	// ignor comentariile pana gasesc magic word ul
	while (fgets(line, sizeof(line), file)) {
		if (line[0] == '#')
			continue;
		if (sscanf(line, "%s", img->magic) == 1)
			break;
	}

	// verific daca exista ceva in fisier
	if (strlen(img->magic) == 0) {
		printf("Failed to load %s\n", filename);
		return;
	}

	// ignor comentariile pana gasesc dimensiunile
	while (fgets(line, sizeof(line), file)) {
		if (line[0] == '#')
			continue;
		if (sscanf(line, "%d %d", &img->width, &img->height) == 2)
			break;
	}

	// ignor comentariile pana gasesc maxval
	while (fgets(line, sizeof(line), file)) {
		if (line[0] == '#')
			continue;
		if (sscanf(line, "%d", &img->maxval) == 1)
			break;
	}

	// verific tipul imaginii
	if (strcmp(img->magic, "P2") == 0)
		readp2(file, img);
	if (strcmp(img->magic, "P5") == 0)
		readp5(file, img);
	if (strcmp(img->magic, "P3") == 0)
		readp3(file, img);
	if (strcmp(img->magic, "P6") == 0)
		readp6(file, img);
	printf("Loaded %s\n", filename);
	fclose(file);
}

#endif
