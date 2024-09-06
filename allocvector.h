#include <stdio.h>
#include <stdlib.h>
#ifndef ALLOCVECTOR_H
#define ALLOCVECTOR_H

void allocvector(int **vector)
{
	// Alocare dinamică a vectorului
	*vector = (int *)calloc(256, sizeof(int));

	// Verificare dacă alocarea a avut succes
	if (!(*vector)) {
		fprintf(stderr, "Calloc failed\n");
		return;
	}
}

#endif
