#include <stdlib.h>
#include <string.h>
#include "simprob_utils.h"
#include "number_utils.h"

char* createDnaString(double adeninePercentage, double guaninePercentage, double cytosinePercentage, double thyminePercentage, int dnaLength)
{
	char *dnaString = malloc((dnaLength + 1) * sizeof(char));
	int i;
	for(i = 0; i < dnaLength; i++){
		dnaString[i] = createNucleotide(adeninePercentage, guaninePercentage, cytosinePercentage, thyminePercentage);
	}
	dnaString[dnaLength] = '\0';
	return dnaString;
}

char createNucleotide(double adeninePercentage, double guaninePercentage, double cytosinePercentage, double thyminePercentage)
{
	char nucleotide;

	/* Obtiene un numero entre 0 y 0.99999 */
	double random = generateRandom();

	if (isBetween(random, 0, adeninePercentage))
	{
		nucleotide = ADENINE;
	}
	else if (isBetween(random,adeninePercentage, adeninePercentage + guaninePercentage))
	{
		nucleotide = GUANINE;
	}
	else if (isBetween(random, adeninePercentage + guaninePercentage, adeninePercentage + guaninePercentage + cytosinePercentage))
	{
		nucleotide = CYTOSINE;
	}
	else if (isBetween(random, adeninePercentage + guaninePercentage + cytosinePercentage, adeninePercentage + guaninePercentage + cytosinePercentage + thyminePercentage))
	{
		nucleotide = THYMINE;
	}

	return nucleotide;
}
