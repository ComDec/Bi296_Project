#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "number_utils.h"

int isBetween(double num, double a, double b)
{
	return a <= num && num < b;
}

double generateRandom()
{
	return (double)rand() / (double)((unsigned)RAND_MAX + 1);
}

void sort(double *array, int n)
{
        double min;
        int i, j;

	for(i = 0; i < n - 1; i++){
		min = array[i];
		for(j = i + 1; j < n; j++){
			if (array[j] < min){
				min = array[j];
				array[j] = array[i];
				array[i] = min;
			}
		}
	}
}

int* changeBase(int decimal, int base, int n)
{
	int *res = malloc(n * sizeof(int));
	int numberLength = 0;
	int pos, i;
	int decimalCopy = decimal;

	while(decimalCopy != 0){
		numberLength++;
		decimalCopy = decimalCopy/base;
	}
	if(numberLength > n){
		printf("Error changeBase. Se pidio devolver un array de %d posiciones, pero %d base %d no cabe en el.\n", n, decimal, base);
		free(res);
		return NULL;
	}

	for(i = 0; i < n; i++){
		res[i] = 0;
	}

	pos = n - 1;
	while(decimal != 0){
		res[pos] = decimal % base;
		decimal = decimal / base;
		pos --;
	}
	return res;
}
