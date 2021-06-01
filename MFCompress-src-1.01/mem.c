/*------------------------------------------------------------------------------

Copyright 2013 IEETA/DETI/University of Aveiro, Portugal.
All Rights Reserved.

These programs are supplied free of charge for research purposes only,
and may not be sold or incorporated into any commercial product. There is
ABSOLUTELY NO WARRANTY of any sort, nor any undertaking that they are
fit for ANY PURPOSE WHATSOEVER. Use them at your own risk. If you do
happen to find a bug, or have modifications to suggest, please report
the same to Armando J. Pinho, ap@ua.pt. The copyright notice above
and this statement of conditions must remain an integral part of each
and every copy made of these files.

------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "mem.h"

/*----------------------------------------------------------------------------*/

static size_t totalMem = 0;

/*----------------------------------------------------------------------------*/

void *Calloc(size_t nmemb, size_t size)

	{
	if(!nmemb || !size)
		{
		fprintf(stderr, "Request of zero bytes in Calloc...\n");
		exit(1);
		}

	totalMem += nmemb * size;
	return calloc(nmemb, size);
	}

/*----------------------------------------------------------------------------*/

void *Malloc(size_t size)

	{
	if(!size)
		{
		fprintf(stderr, "Request of zero bytes in Malloc...\n");
		exit(1);
		}

	totalMem += size;
	return malloc(size);
	}

/*----------------------------------------------------------------------------*/

void *Realloc(void *ptr, size_t size, size_t additionalSize)

	{
	totalMem += additionalSize;
	return realloc(ptr, size);
	}

/*----------------------------------------------------------------------------*/

void Free(void *ptr, size_t size)

	{
	totalMem -= size;
	free(ptr);
	}

/*----------------------------------------------------------------------------*/

size_t TotalMem()

	{
	return totalMem;
	}

