#include <stdio.h>
#include <stdlib.h>
#include "heap.h"

int main()
{
	int *a = (int *)malloc2(4*sizeof(int));
	char *b = (char *)calloc2(4,sizeof(char));
	printHeap();
	int *d = (int*)realloc2(a,10*sizeof(int));
	double *h = (double*)malloc2(6*sizeof(double));
	double *j = (double*)malloc2(6*sizeof(double));
	//exit(1);
	//d[0] = 17;
	//d[1] = 12;
	d[3] = 48;
	free2(h);
	printf("\n%d %d %d %d\n", d[1], d[0], b[0],b[3]);
	printHeap();
	return 0;
}
