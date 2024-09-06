#include <stdio.h>
#include <stdlib.h>
#ifndef ALLOCATEMEMORY_H
#define ALLOCATEMEMORY_H

void allocatememory(int ***data, int m, int n)
{
    //aloc memorie pentru linii
	*data = (int **)calloc(m, sizeof(int *));
	if (!*data) {
		fprintf(stderr, "calloc failed.\n");
		exit(EXIT_FAILURE);
	}
    //aloc memorie pentru coloane
	for (int i = 0; i < m; i++) {
		(*data)[i] = (int *)calloc(n, sizeof(int));
		if (!(*data)[i]) {
			fprintf(stderr, "calloc failed.\n");
			// Eliberarea memoriei alocate anterior
			for (int k = 0; k < i; k++)
				free((*data)[k]);
			free(*data);
			exit(EXIT_FAILURE);
		}
	}
}

#endif
