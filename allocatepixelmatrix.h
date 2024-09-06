#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#ifndef ALLOCATEPIXELMATRIX_H
#define ALLOCATEPIXELMATRIX_H

pixel **allocatepixelmatrix(int rows, int cols)
{
    //aloc memorie pentru matricea color pe linii
	pixel **matrix = (pixel **)malloc(rows * sizeof(pixel *));
	if (!matrix) {
		fprintf(stderr, "malloc failed\n");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < rows; ++i) {
		matrix[i] = (pixel *)malloc(cols * sizeof(pixel));
		if (!matrix[i]) {
			fprintf(stderr, "malloc failed\n");
			//eliberarea memoriei alocate anterior
			for (int j = 0; j < i; ++j)
				free(matrix[j]);
			free(matrix);
			exit(EXIT_FAILURE);
		}
	}
    //returnez matricea obtinuta
	return matrix;
}

#endif
