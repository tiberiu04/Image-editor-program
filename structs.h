#include <stdio.h>
#include <stdlib.h>
#ifndef STRUCTS_H
#define STRUCTS_H

//structura pentru o imagine color
typedef struct {
	int red;
	int green;
	int blue;
} pixel;

//structura pentru imagini
typedef struct {
	char magic[3];
	int width;
	int height;
	int maxval;
	int **data;
	pixel **rgb;
} image;

//structura pentru selectie
typedef struct {
	int x1, y1, x2, y2;
	int selected;
} selection;

#endif
