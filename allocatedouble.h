#include <stdio.h>
#include <stdlib.h>
#ifndef ALLOCATEDOUBLE_H
#define ALLOCATEDOUBLE_H

void allocatedouble(double ****data, int m, int n)
{
    //aloc dinamic o matrice 3D
	*data = (double ***)calloc(m, sizeof(double **));
	if (!*data) {
		fprintf(stderr, "Calloc failed.\n");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < m; i++) {
		(*data)[i] = (double **)calloc(n, sizeof(double *));
		if (!(*data)[i]) {
			fprintf(stderr, "Calloc failed).\n");
			// Eliberarea memoriei alocate anterior
			for (int k = 0; k < i; k++)
				free((*data)[k]);
			free(*data);
			exit(EXIT_FAILURE);
		}

		for (int j = 0; j < n; j++) {
			(*data)[i][j] = (double *)calloc(4, sizeof(double));
			if (!(*data)[i][j]) {
				fprintf(stderr, "Calloc failed.\n");
				// Eliberarea memoriei alocate anterior
				for (int k = 0; k < i; k++) {
					for (int l = 0; l < j; l++)
						free((*data)[k][l]);
					free((*data)[k]);
				}
				free(*data);
				exit(EXIT_FAILURE);
			}
		}
	}
}

#endif
