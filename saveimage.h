#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "printimage.h"
#ifndef SAVEIMAGE_H
#define SAVEIMAGE_H

void printascii(image *img, FILE *file)
{
	if (img->magic[1] == '2') { //imagine pgm in format text
		for (int i = 0; i < img->height; i++) {
			for (int j = 0; j < img->width; j++)
				fprintf(file, "%d ", img->data[i][j]); // afisez pixelii
			fprintf(file, "\n");
		}
	} else { // imagine ppm in format ext
		for (int i = 0; i < img->height; i++) {
			for (int j = 0; j < img->width; j++) { //afisez pixelii rgb
				fprintf(file, "%d ", img->rgb[i][j].red);
				fprintf(file, "%d ", img->rgb[i][j].green);
				fprintf(file, "%d ", img->rgb[i][j].blue);
			}
			fprintf(file, "\n");
		}
	}
}

void printtbinary(image *img, FILE *file)
{
	// afisare imagine pgm in format binar
	if (img->magic[1] == '5') {
		unsigned char c;
		for (int i = 0; i < img->height; i++) {
			for (int j = 0; j < img->width; j++) {
				c = (unsigned char)img->data[i][j];
				if (fwrite(&c, sizeof(unsigned char), 1, file) != 1) {
					fprintf(stderr, "Error writing to file");
					fclose(file);
					return;
				}
			}
		}
	}

	// afisare imagine ppm in format binar
	if (img->magic[1] == '6') {
		unsigned char c, d, e;
		for (int i = 0; i < img->height; i++) {
			for (int j = 0; j < img->width; j++) {
				c = (unsigned char)img->rgb[i][j].red;
				d = (unsigned char)img->rgb[i][j].green;
				e = (unsigned char)img->rgb[i][j].blue;
				if (fwrite(&c, sizeof(unsigned char), 1, file) != 1 ||
				    fwrite(&d, sizeof(unsigned char), 1, file) != 1 ||
				    fwrite(&e, sizeof(unsigned char), 1, file) != 1) {
					fprintf(stderr, "Error writing to file");
					fclose(file);
					return;
				}
			}
		}
	}
}

void saveimage(image *img, char *filename)
{
	char *format;
	format = (char *)malloc(50 * sizeof(char));
	if (!format) {
		printf("NU am putut creea format\n");
		return;
	}
	// citesc formatul de salvare
	fgets(format, sizeof(format), stdin);
	format[strcspn(format, "\n")] = '\0';
	//format ascii
	if (format[0] != '\0' && strcmp(format, " ascii") == 0) {
		FILE *file;
		file = fopen(filename, "w"); //deschid fisierul in mod write normal
		if (!file || (!img->rgb && !img->data)) { //verific daca exista imagine
			printf("No image loaded\n");
			return;
		}

		if (img->magic[1] == '5' || img->magic[1] == '6')
			img->magic[1] -= 3;
		printimage(img, file); // afisez informatiile despre imagine
		printascii(img, file);
		// confirm succesul operatiei
		printf("Saved %s\n", filename);
		fclose(file);
	} else {
		FILE *file;
		file = fopen(filename, "wb"); // deschid fisierul in scriere binara
		if (!file || (!img->rgb && !img->data)) { //verific existenta imaginii
			printf("No image loaded\n");
			return;
		}
		if (img->magic[1] == '2' || img->magic[1] == '3')
			img->magic[1] += 3;
		printimage(img, file);
		printtbinary(img, file);
		// confirm succesul operatiei
		printf("Saved %s\n", filename);
		fclose(file);
	}
	free(format);
}

#endif
