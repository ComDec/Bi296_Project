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
#include <string.h>
#include <math.h>
#include "defs.h"
#include "io.h"
#include "mem.h"
#include "fasta.h"
#include "fcm.h"

/*----------------------------------------------------------------------------*/

/*            1
 *            X
 */
static Coords template0[] = {{-1, 0}};

/*
 *      2  1  X
 */
static Coords template1[] = {{0, -1}, {0, -2}};

/*
 *            2
 *         1  X
 */
static Coords template2[] = {{0, -1}, {-1, 0}};

/*
 *         3  2
 *         1  X
 */
static Coords template3[] = {{0, -1}, {-1, 0}, {-1, -1}};

/*
 *         3  2  4
 *         1  X
 */
static Coords template4[] = {{0, -1}, {-1, 0}, {-1, -1}, {-1, 1}};

/*
 *         3  2  4
 *      5  1  X
 */
static Coords template5[] = {{0, -1}, {-1, 0}, {-1, -1}, {-1, 1}, {0, -2}};

static float logTable[LOG_TABLE_SIZE];

static unsigned counter04mask[4] = {0x0F, 0xF0, 0x0F, 0xF0};

/*----------------------------------------------------------------------------*/

void FillLogTable(void)

	{
	unsigned n;

	for(n = 1 ; n < LOG_TABLE_SIZE ; n++)
		logTable[n] = log(n);

	}

/*----------------------------------------------------------------------------*/

CTemplate2D CreateCTemplate2D(int id)

	{
	CTemplate2D cTemplate;

	switch(id)
		{
		case 0:
			cTemplate.position = template0;
			cTemplate.size = sizeof(template0) / sizeof(template0[0]);
			break;

		case 1:
			cTemplate.position = template1;
			cTemplate.size = sizeof(template1) / sizeof(template1[0]);
			break;

		case 2:
			cTemplate.position = template2;
			cTemplate.size = sizeof(template2) / sizeof(template2[0]);
			break;

		case 3:
			cTemplate.position = template3;
			cTemplate.size = sizeof(template3) / sizeof(template3[0]);
			break;

		case 4:
			cTemplate.position = template4;
			cTemplate.size = sizeof(template4) / sizeof(template4[0]);
			break;

		case 5:
			cTemplate.position = template5;
			cTemplate.size = sizeof(template5) / sizeof(template5[0]);
			break;

		default:
			fprintf(stderr, "Error: invalid template id\n");
			exit(1);

		}

	cTemplate.id = id;
	return cTemplate;
	}

/*----------------------------------------------------------------------------*/

static void InitArray(CModel *cModel)

	{
	switch(cModel->array.type)
		{
		case COUNTERS_16:
			if(!(cModel->array.counters16 = (Counter16 *)Calloc((size_t)
			  cModel->nPModels * cModel->nSymbols, sizeof(Counter16))))
				{
				fprintf(stderr, "Error: in memory allocation\n");
				exit(1);
				}

			return;

		case COUNTERS_08:
			if(!(cModel->array.counters08 = (Counter08 *)Calloc((size_t)
			  cModel->nPModels * cModel->nSymbols, sizeof(Counter08))))
				{
				fprintf(stderr, "Error: in memory allocation\n");
				exit(1);
				}

			return;

		case COUNTERS_04:
			if(cModel->nSymbols != 4)
				{
				fprintf(stderr,
				  "Error: 4-bit counters only valid for 4-symbol alphabets\n");
				exit(1);
				}

			if(!(cModel->array.counters04 = (Counter04 *)Calloc((size_t)
			  cModel->nPModels * 2, sizeof(Counter04))))
				{
				fprintf(stderr, "Error: in memory allocation\n");
				exit(1);
				}

			return;

		case COUNTERS_02:
			if(cModel->nSymbols != 4)
				{
				fprintf(stderr,
				  "Error: 2-bit counters only valid for 4-symbol alphabets\n");
				exit(1);
				}

			if(!(cModel->array.counters02 = (Counter02 *)Calloc((size_t)
			  cModel->nPModels, sizeof(Counter02))))
				{
				fprintf(stderr, "Error: in memory allocation\n");
				exit(1);
				}

			return;
		}

	}

/*----------------------------------------------------------------------------*/

CModel *CreateCModel(unsigned ctxSize, unsigned nSymbols, unsigned alphaNum,
  unsigned alphaDen, unsigned maxCount)

	{
	unsigned n, prod = 1;
	CModel *cModel;

	if(!(cModel = (CModel *)Calloc(1, sizeof(CModel))))
		{
		fprintf(stderr, "Error: in memory allocation\n");
		exit(1);
		}

	if(!(cModel->multipliers = (unsigned *)Calloc(ctxSize, sizeof(unsigned))))
		{
		fprintf(stderr, "Error: in memory allocation\n");
		exit(1);
		}

	if(ctxSize > 16)
		{
		fprintf(stderr, "Error: context size cannot be greater than 16\n");
		exit(1);
		}

	cModel->nPModels = (uint32_t)pow(nSymbols, ctxSize);
	if(cModel->nPModels > 1000000) // depth >= 10 for 4 symbols
		{
		cModel->array.type = COUNTERS_08;
		if(maxCount > 255)
			maxCount = 255;

		}

	else
		{
		cModel->array.type = COUNTERS_16;
		if(maxCount > 65535)
			maxCount = 65535;

		}

	if(nSymbols == 4 && ctxSize >= 14)
		{
		cModel->array.type = COUNTERS_04;
		maxCount = 15; // Do not change
		}

	if(nSymbols == 4 && ctxSize == 16)
		{
		cModel->array.type = COUNTERS_02;
		maxCount = 3; // Do not change
		}

	cModel->pModelIdx = 0;
	cModel->pModelIdxAux = cModel->nPModels - 1;

	// Initially, both pModelIdx and pModelIdxAux have valid values
	cModel->validPModelIdx = 1;
	cModel->validPModelIdxAux = 1;

	cModel->ctxSize = ctxSize;
	cModel->nSymbols = nSymbols;
	cModel->alphaNum = alphaNum;
	cModel->alphaDen = alphaDen;
	cModel->maxCount = maxCount;
	switch(nSymbols)
		{
		case 2:
			cModel->kMinusOneMask = (0x01 << (ctxSize - 1)) - 1;
			break;

		case 4:
			cModel->kMinusOneMask = (0x01 << 2 * (ctxSize - 1)) - 1;
			break;

		}

	for(n = 0 ; n < ctxSize ; n++)
		{
		cModel->multipliers[n] = prod;
		prod *= nSymbols;
		}

	cModel->kMinusOneShift = 2 * (ctxSize - 1);

	InitArray(cModel);

	return cModel;
	}

/*----------------------------------------------------------------------------*/

static void InitArray2(CModel *cModel)

	{
	uint64_t n;

	if(!(cModel->array.counters16 = (Counter16 *)
	  Malloc((cModel->nPModels << 1) * sizeof(Counter16))))
		{
		fprintf(stderr, "Error: in memory allocation\n");
		exit(1);
		}

	// Initiate all counters to "1" (Laplace estimator).
	for(n = 0 ; n < cModel->nPModels << 1 ; n++)
		cModel->array.counters16[n] = 1;

	}

/*----------------------------------------------------------------------------*/

CModel *CreateCModel2(unsigned ctxSize)

	{
	unsigned n, prod = 1;
	CModel *cModel;

	if(!(cModel = (CModel *)Calloc(1, sizeof(CModel))))
		{
		fprintf(stderr, "Error: in memory allocation\n");
		exit(1);
		}

	if(!(cModel->multipliers = (unsigned *)Calloc(ctxSize, sizeof(unsigned))))
		{
		fprintf(stderr, "Error: in memory allocation\n");
		exit(1);
		}

	cModel->nPModels = (uint32_t)pow(2, ctxSize);
	cModel->array.type = COUNTERS_16;
	cModel->pModelIdx = 0;
	cModel->ctxSize = ctxSize;
	cModel->nSymbols = 2;
	cModel->kMinusOneMask = (0x01 << (ctxSize - 1)) - 1;

	for(n = 0 ; n < ctxSize ; n++)
		{
		cModel->multipliers[n] = prod;
		prod *= 2;
		}

	InitArray2(cModel);

	return cModel;
	}

/*----------------------------------------------------------------------------*/

PModel *CreatePModel(unsigned nSymbols)

	{
	PModel *pModel;

	if(!(pModel = (PModel *)Malloc(sizeof(PModel))))
		{
		fprintf(stderr, "Error: in memory allocation\n");
		exit(1);
		}

	if(!(pModel->cum = (unsigned *)Calloc(nSymbols + 1, sizeof(unsigned))))
		{
		fprintf(stderr, "Error: in memory allocation\n");
		exit(1);
		}

	pModel->nSymbols = nSymbols;
	return pModel;
	}

/*----------------------------------------------------------------------------*/

void UpdateCModelCounter2(CModel *cModel, unsigned pModelIdx, unsigned symbol)

	{
	Counter16 *counters16;

	counters16 = cModel->array.counters16 + ((size_t)pModelIdx << 1);

	// NOTE: these counters cannot be zero
	if(counters16[symbol] == 65535)
		{
		counters16[0] = (counters16[0] >> 1) + 1;
		counters16[1] = (counters16[1] >> 1) + 1;
		}

	++counters16[symbol];
	}

/*----------------------------------------------------------------------------*/

void UpdateCModelCounter4(CModel *cModel, unsigned pModelIdx, unsigned symbol)

	{
	Counter16 *counters16;
	Counter08 *counters08;
	Counter04 *counters04;
	Counter02 *counters02;

	switch(cModel->array.type)
		{
		case COUNTERS_16:
			counters16 = cModel->array.counters16 + ((size_t)pModelIdx << 2);
			if(!cModel->maxCount)
				memset(counters16, 0, 8); // Set all counters to zero

			else
				if(counters16[symbol] == cModel->maxCount)
					{
					counters16[0] >>= 1;
					counters16[1] >>= 1;
					counters16[2] >>= 1;
					counters16[3] >>= 1;
					}

			++counters16[symbol];
			return;

		case COUNTERS_08:
			counters08 = cModel->array.counters08 + ((size_t)pModelIdx << 2);
			if(!cModel->maxCount)
				memset(counters08, 0, 4); // Set all counters to zero

			else
				if(counters08[symbol] == cModel->maxCount)
					{
					counters08[0] >>= 1;
					counters08[1] >>= 1;
					counters08[2] >>= 1;
					counters08[3] >>= 1;
					}

			++counters08[symbol];
			return;

		case COUNTERS_04:
			counters04 = cModel->array.counters04 + ((size_t)pModelIdx << 1);
			if((counter04mask[symbol] & counters04[symbol >> 1]) ==
			  counter04mask[symbol])
				{
				*counters04 = (*counters04 >> 1) & 0x77; // 0x77 = 01110111
				*(counters04 + 1) = (*(counters04 + 1) >> 1) & 0x77;
				}

			switch(symbol)
				{
				case 0:
					(*counters04)++;
					return;

				case 1:
					*counters04 += 16;
					return;

				case 2:
					(*(counters04 + 1))++;
					return;

				case 3:
					*(counters04 + 1) += 16;
					return;

				}

		case COUNTERS_02:
			counters02 = cModel->array.counters02 + (size_t)pModelIdx;
			if(!((0x03 << (symbol << 1)) ^ *counters02))
				*counters02 = (*counters02 >> 1) & 0x55; // 0x55 = 01010101

			switch(symbol)
				{
				case 0:
					(*counters02)++;
					return;

				case 1:
					*counters02 += 4;
					return;

				case 2:
					*counters02 += 16;
					return;

				case 3:
					*counters02 += 64;
					return;

				}

		}

	}

/*----------------------------------------------------------------------------*/

void UpdateCModelCounter(CModel *cModel, unsigned pModelIdx, unsigned symbol)

	{
	unsigned n;
	Counter16 *counters16;
	Counter08 *counters08;

	switch(cModel->array.type)
		{
		case COUNTERS_16:
			counters16 = cModel->array.counters16 +
			  (size_t)pModelIdx * cModel->nSymbols;
			if(counters16[symbol] == cModel->maxCount)
				for(n = 0 ; n < cModel->nSymbols ; n++)
					counters16[n] >>= 1;

			++counters16[symbol];
			return;

		case COUNTERS_08:
			counters08 = cModel->array.counters08 +
			  (size_t)pModelIdx * cModel->nSymbols;
			if(counters08[symbol] == cModel->maxCount)
				for(n = 0 ; n < cModel->nSymbols ; n++)
					counters08[n] >>= 1;

			++counters08[symbol];
			return;

		}

	}

/*----------------------------------------------------------------------------*/

inline void ComputePModel2(CModel *cModel, PModel *pModel)

	{
	Counter16 *counters16;

	counters16 = cModel->array.counters16 + ((size_t)cModel->pModelIdx << 1);
	pModel->cum[1] = *counters16++;
	pModel->cum[2] = pModel->cum[1] + *counters16;
	}

/*----------------------------------------------------------------------------*/

void ComputePModel4(CModel *cModel, PModel *pModel)

	{
	Counter16 *counters16;
	Counter08 *counters08;
	Counter04 *counters04;
	Counter02 *counters02;

	switch(cModel->array.type)
		{
		case COUNTERS_16:
			counters16 = cModel->array.counters16 +
			  ((size_t)cModel->pModelIdx << 2);
			pModel->cum[1] = cModel->alphaNum +
			  cModel->alphaDen * (*counters16++);
			pModel->cum[2] = pModel->cum[1] + cModel->alphaNum +
			  cModel->alphaDen * (*counters16++);
			pModel->cum[3] = pModel->cum[2] + cModel->alphaNum +
			  cModel->alphaDen * (*counters16++);
			pModel->cum[4] = pModel->cum[3] + cModel->alphaNum +
			  cModel->alphaDen * (*counters16);
			return;

		case COUNTERS_08:
			counters08 = cModel->array.counters08 +
			  ((size_t)cModel->pModelIdx << 2);
			pModel->cum[1] = cModel->alphaNum +
			  cModel->alphaDen * (*counters08++);
			pModel->cum[2] = pModel->cum[1] + cModel->alphaNum +
			  cModel->alphaDen * (*counters08++);
			pModel->cum[3] = pModel->cum[2] + cModel->alphaNum +
			  cModel->alphaDen * (*counters08++);
			pModel->cum[4] = pModel->cum[3] + cModel->alphaNum +
			  cModel->alphaDen * (*counters08);
			return;

		case COUNTERS_04:
			counters04 = cModel->array.counters04 +
			  ((size_t)cModel->pModelIdx << 1);
			pModel->cum[1] = cModel->alphaNum +
			  cModel->alphaDen * (*counters04 & 0x0F);
			pModel->cum[2] = pModel->cum[1] + cModel->alphaNum +
			  cModel->alphaDen * ((*counters04 & 0xF0) >> 4);
			pModel->cum[3] = pModel->cum[2] + cModel->alphaNum +
			  cModel->alphaDen * (*(counters04 + 1) & 0x0F);
			pModel->cum[4] = pModel->cum[3] + cModel->alphaNum +
			  cModel->alphaDen * ((*(counters04 + 1) & 0xF0) >> 4);
			return;

		case COUNTERS_02:
			counters02 = cModel->array.counters02 + (size_t)cModel->pModelIdx;
			pModel->cum[1] = cModel->alphaNum +
			  cModel->alphaDen * (*counters02 & 0x03);
			pModel->cum[2] = pModel->cum[1] + cModel->alphaNum +
			  cModel->alphaDen * ((*counters02 & 0x0C) >> 2);
			pModel->cum[3] = pModel->cum[2] + cModel->alphaNum +
			  cModel->alphaDen * ((*counters02 & 0x30) >> 4);
			pModel->cum[4] = pModel->cum[3] + cModel->alphaNum +
			  cModel->alphaDen * ((*counters02 & 0xC0) >> 6);
			return;

		}

	}

/*----------------------------------------------------------------------------*/

void ComputePModel(CModel *cModel, PModel *pModel)

	{
	int symbol;
	Counter16 *counters16;
	Counter08 *counters08;

	switch(cModel->array.type)
		{
		case COUNTERS_16:
			counters16 = cModel->array.counters16 +
			  (size_t)cModel->pModelIdx * cModel->nSymbols;
			for(symbol = 0 ; symbol < cModel->nSymbols ; symbol++)
				pModel->cum[symbol+1] = pModel->cum[symbol] + cModel->alphaNum +
				  cModel->alphaDen * (*counters16++);

			return;

		case COUNTERS_08:
			counters08 = cModel->array.counters08 +
			  (size_t)cModel->pModelIdx * cModel->nSymbols;
			for(symbol = 0 ; symbol < cModel->nSymbols ; symbol++)
				pModel->cum[symbol+1] = pModel->cum[symbol] + cModel->alphaNum +
				  cModel->alphaDen * (*counters08++);

			return;

		}

	}

/*----------------------------------------------------------------------------*/

double PModelSymbolNatsLog(PModel *pModel, unsigned symbol)

	{
	return log((double)pModel->cum[pModel->nSymbols] /
	  (pModel->cum[symbol+1] - pModel->cum[symbol]));
	}

/*----------------------------------------------------------------------------*/

double PModelSymbolNatsTable(PModel *pModel, unsigned symbol)

	{
	// Note: pModel->sum cannot exceed the size of the log table,
	// currently 65536*4. The values of deltaNum and deltaDen may
	// produce overflow of the table...
	return logTable[pModel->cum[pModel->nSymbols]] -
	  logTable[pModel->cum[symbol+1] - pModel->cum[symbol]];
	}

/*------------------------------------------------------------------------------

	"symbolPtr" points to the char before the char being encoded / decoded

------------------------------------------------------------------------------*/

inline void GetPModelIdx2(Symbol *symbolPtr, CModel *cModel)

	{
	cModel->pModelIdx = ((cModel->pModelIdx & cModel->kMinusOneMask) << 1)
	  + *symbolPtr;
	}

/*------------------------------------------------------------------------------

	"symbolPtr" points to the char being encoded / decoded

------------------------------------------------------------------------------*/

inline void GetPModelIdx4(Symbol *symbolPtr, CModel *cModel,
  CTemplate *cTemplate)

	{
	if(cModel->validPModelIdx)
		cModel->pModelIdx = ((cModel->pModelIdx & cModel->kMinusOneMask) << 2)
		  + (*(symbolPtr - 1) & 0x03);

	else
		{
		unsigned n;

		cModel->pModelIdx = 0;
		for(n = 0 ; n < cTemplate->size ; n++)
			cModel->pModelIdx += (*(symbolPtr - cTemplate->position[n]) &
			  0x03) << (n << 1);

		cModel->validPModelIdx = 1;
		}

	}

/*------------------------------------------------------------------------------

	"charPtr" points to the char being encoded / decoded

------------------------------------------------------------------------------*/

inline void GetPModelIdxAux4(Char *symbolPtr, CModel *cModel,
  CTemplate *cTemplate)

	{
	if(cModel->validPModelIdxAux)
		cModel->pModelIdxAux = (cModel->pModelIdxAux >> 2) +
		  ((3 - (*symbolPtr & 0x03)) << cModel->kMinusOneShift);

	else
		{
		unsigned n;

		symbolPtr++;
		cModel->pModelIdxAux = 0;
		for(n = 0 ; n < cTemplate->size ; n++)
			cModel->pModelIdxAux += (3 - (*(symbolPtr - cTemplate->position[n])
			  & 0x03)) << ((cTemplate->size - n - 1) << 1);

		cModel->validPModelIdxAux = 1;
		}

	}

/*------------------------------------------------------------------------------

	"symbolPtr" points to the char before the char being encoded / decoded

------------------------------------------------------------------------------*/

inline void GetInfoPModelIdx(Symbol *symbolPtr, CModel *cModel)

	{
	cModel->pModelIdx = ((cModel->pModelIdx - *(symbolPtr - cModel->ctxSize) *
	  cModel->multipliers[cModel->ctxSize-1]) * cModel->nSymbols) + *symbolPtr;
	}

/*----------------------------------------------------------------------------*/

unsigned GetPModelIdx2D(Char **charPtr, int col, CTemplate2D *cTemplate,
  HeaderAlphabet *alphabet, CModel *cModel)

	{
	unsigned n, idx = 0;

	for(n = 0 ; n < cTemplate->size ; n++)
		idx += alphabet->symbols[charPtr[cTemplate->position[n].row + 1]
		  [cTemplate->position[n].col + col]] * cModel->multipliers[n];

	return idx;
	}

/*----------------------------------------------------------------------------*/

void IncreaseTemplateStorage(CTemplate *cTemplate, int additionalSize)

	{
	if(cTemplate->size == 0)
		{
		if(!(cTemplate->position = (int *)Malloc(additionalSize * sizeof(int))))
			{
			fprintf(stderr, "Error: in memory allocation\n");
			exit(1);
			}

		}

	else
		{
		if(!(cTemplate->position = (int *)Realloc(cTemplate->position,
		  (additionalSize + cTemplate->size) * sizeof(int), additionalSize *
		  sizeof(int))))
			{
			fprintf(stderr, "Error: in memory allocation\n");
			exit(1);
			}

		}

	}

/*----------------------------------------------------------------------------*/

int StrToArgv(char *str, char ***argv)

	{
	int n, argc = 1;
	char *dup = strdup(str);

	n = 0;
	while(dup[n])
		if(dup[n++] == ' ')
			argc++;

	if(!(*argv = (char **)Malloc(argc * sizeof(char *))))
		{
		fprintf(stderr, "Error: in memory allocation\n");
		exit(1);
		}

	n = 0;
	(*argv)[0] = dup;
	argc = 1;
	do
		{
		if(dup[n] == ' ')
			{
			dup[n] = '\0';
			(*argv)[argc++] = dup + n + 1;
			}

		}
	while(dup[++n]);

	return argc;
	}

/*----------------------------------------------------------------------------*/

void ParseListOfPositions(char *str, CTemplate *cTemplate)

	{
	int n, low, high;

	cTemplate->size = 0;
	while(1)
		{
		switch(sscanf(str, "%d:%d", &low, &high))
			{
			case 1:
				if(low == 0)
					return;

				if(low < 0)
					{
					fprintf(stderr,"Error: can't handle non-causal contexts\n");
					exit(1);
					}

				IncreaseTemplateStorage(cTemplate, 1);
				cTemplate->position[cTemplate->size++] = low;
				break;

			case 2:
				if(low <= 0)
					{
					fprintf(stderr,"Error: can't handle non-causal contexts\n");
					exit(1);
					}

				if(high - low + 1 > 0)
					{
					IncreaseTemplateStorage(cTemplate, high - low + 1);
					for(n = 0 ; n < high - low + 1 ; n++)
						cTemplate->position[cTemplate->size++] = low + n;

					}

				break;

			default:
				return;

			}

		while(*str != '\0' && *str++ != ',')
			;

		}

	}

/*----------------------------------------------------------------------------*/

void InitDataStructs(HeaderAlphabet *hAlphabet, SequenceAlphabet *bAlphabet,
  char *hCtx, char *bCtx, char *iCtx, CTemplate2D *hCTemplate, CModel **hCModel,
  PModel **hPModel, unsigned *bNCModels, CTemplate **bCTemplates,
  CModel ***bCModels, PModel **bPModel, CTemplate **auxCTemplates,
  CModel ***auxCModels, PModel ***auxPModels, Symbol ***auxBuffers,
  unsigned *bSize)

	{
	int ctxArgc;
	char **ctxArgv;
	unsigned k, n, cModel, alphaNum, alphaDen, maxCount, update,
	  updateIC, modelOrder;

	// Create the data structures for the header component
	ctxArgc = StrToArgv(hCtx, &ctxArgv);

	// Get the context templates for the header component
	for(n = 0 ; n < ctxArgc ; n++)
		if(strcmp("-c", ctxArgv[n]) == 0)
			*hCTemplate = CreateCTemplate2D(atoi(ctxArgv[n+1]));

	cModel = 0;
	for(n = 0 ; n < ctxArgc ; n++)
		{
		if(strcmp("-c", ctxArgv[n]) == 0)
			{
			if(sscanf(ctxArgv[n+2], "%d/%d", &alphaNum, &alphaDen) != 2)
				{
				fprintf(stderr, "Error: couldn't get alpha value\n");
				exit(1);
				}

			if(sscanf(ctxArgv[n+3], "%d", &maxCount) != 1)
				{
				fprintf(stderr, "Error: couldn't get maxCount value\n");
				exit(1);
				}

			*hCModel = CreateCModel(hCTemplate->size, hAlphabet->nSymbols,
			  alphaNum, alphaDen, maxCount);

#ifdef DEBUG_1
			printf("Creating %u hPModels (tSize: %d, alpha = %d/%d, "
			  "maxCount = %d)\n", (*hCModel)->nPModels, (*hCModel)->ctxSize,
			  alphaNum, alphaDen, maxCount);
#endif // DEBUG_1

			cModel++;
			}

		}

	*hPModel = CreatePModel(hAlphabet->nSymbols);

	// Create the data structures for the sequence component
	ctxArgc = StrToArgv(bCtx, &ctxArgv);

	for(n = 0 ; n < ctxArgc ; n++)
		if(strcmp("-bs", ctxArgv[n]) == 0)
			*bSize = atoi(ctxArgv[n+1]);

	*bNCModels = 0;
	for(n = 0 ; n < ctxArgc ; n++)
		if(strcmp("-c", ctxArgv[n]) == 0)
			(*bNCModels)++;

	if(!(*bCTemplates = (CTemplate *)Calloc(*bNCModels, sizeof(CTemplate))) ||
	   !(*bCModels = (CModel **)Calloc(*bNCModels, sizeof(CModel *))))
		{
		fprintf(stderr, "Error: out of memory\n");
		exit(1);
		}

	// Get the context templates for the sequence component
	cModel = 0;
	for(n = 0 ; n < ctxArgc ; n++)
		if(strcmp("-c", ctxArgv[n]) == 0)
			ParseListOfPositions(ctxArgv[n+1], &(*bCTemplates)[cModel++]);

	cModel = 0;
	for(n = 0 ; n < ctxArgc ; n++)
		{
		if(strcmp("-c", ctxArgv[n]) == 0)
			{
			if(sscanf(ctxArgv[n+2], "%d/%d", &alphaNum, &alphaDen) != 2)
				{
				fprintf(stderr, "Error: couldn't get alpha value\n");
				exit(1);
				}

			if(sscanf(ctxArgv[n+3], "%d", &maxCount) != 1)
				{
				fprintf(stderr, "Error: couldn't get maxCount value\n");
				exit(1);
				}

			if(sscanf(ctxArgv[n+4], "%d", &update) != 1)
				{
				fprintf(stderr, "Error: couldn't get update value\n");
				exit(1);
				}

			if(sscanf(ctxArgv[n+5], "%d", &updateIC) != 1)
				{
				fprintf(stderr, "Error: couldn't get updateIC value\n");
				exit(1);
				}

			(*bCModels)[cModel] = CreateCModel((*bCTemplates)[cModel].size,
			  4, alphaNum, alphaDen, maxCount);

			(*bCModels)[cModel]->update = update;
			(*bCModels)[cModel]->updateIC = updateIC;

			(*bCTemplates)[cModel].deepestPosition =
			  (*bCTemplates)[cModel].position[0];
			for(k = 1 ; k < (*bCTemplates)[cModel].size ; k++)
				if((*bCTemplates)[cModel].position[k] >
				  (*bCTemplates)[cModel].deepestPosition)
					(*bCTemplates)[cModel].deepestPosition =
					  (*bCTemplates)[cModel].position[k];

#ifdef DEBUG_1
			printf("Creating %u bPModels (tSize: %d, alpha = %d/%d, "
			  "maxCount = %d, update = %d)\n", (*bCModels)[cModel]->nPModels,
			  (*bCModels)[cModel]->ctxSize, alphaNum, alphaDen, maxCount,
			  (*bCModels)[cModel]->update);
#endif // DEBUG_1

			cModel++;
			}

		}

	*bPModel = CreatePModel(4);

	ctxArgc = StrToArgv(iCtx, &ctxArgv);

	// Create the data structures for auxiliary information
	if(!(*auxCTemplates = (CTemplate *)Calloc(3, sizeof(CTemplate))) ||
	   !(*auxCModels = (CModel **)Calloc(3, sizeof(CModel *))) ||
	   !(*auxPModels = (PModel **)Calloc(3, sizeof(PModel *))) ||
	   !(*auxBuffers = (Symbol **)Calloc(3, sizeof(Symbol *))))
		{
		fprintf(stderr, "Error: out of memory\n");
		exit(1);
		}

	// Side information (indicates which fcm was used in each data block)
	for(n = 0 ; n < ctxArgc ; n++)
		{
		if(strcmp("-cs", ctxArgv[n]) == 0)
			{
			if(sscanf(ctxArgv[n+1], "%d", &modelOrder) != 1)
				{
				fprintf(stderr, "Error: couldn't get model order\n");
				exit(1);
				}

			if(sscanf(ctxArgv[n+2], "%d", &maxCount) != 1)
				{
				fprintf(stderr, "Error: couldn't get maxCount value\n");
				exit(1);
				}

			}

		}

	if(!((*auxBuffers)[SIDE_INFO] = (Symbol *)Calloc(modelOrder + 2,
	  sizeof(Symbol))) || !((*auxCTemplates)[SIDE_INFO].position =
	  (int *)Malloc(modelOrder * sizeof(int))))
		{
		fprintf(stderr, "Error: out of memory\n");
		exit(1);
		}

	(*auxCTemplates)[SIDE_INFO].size = modelOrder;
	for(n = 1 ; n <= modelOrder ; n++)
		(*auxCTemplates)[SIDE_INFO].position[modelOrder - n] = n;

	(*auxCModels)[SIDE_INFO] = CreateCModel(modelOrder,
	  *bNCModels, 1, 1, maxCount);
	(*auxPModels)[SIDE_INFO] = CreatePModel(*bNCModels);

	// Extra chars information
	for(n = 0 ; n < ctxArgc ; n++)
		{
		if(strcmp("-cx", ctxArgv[n]) == 0)
			{
			if(sscanf(ctxArgv[n+1], "%d", &modelOrder) != 1)
				{
				fprintf(stderr, "Error: couldn't get model order\n");
				exit(1);
				}

			if(sscanf(ctxArgv[n+2], "%d", &maxCount) != 1)
				{
				fprintf(stderr, "Error: couldn't get maxCount value\n");
				exit(1);
				}

			}

		}

	if(bAlphabet->nXchrSymbols)
		{
		if(!((*auxBuffers)[XCHR_INFO] = (Symbol *)Calloc(modelOrder + 2,
		  sizeof(Symbol))) || !((*auxCTemplates)[XCHR_INFO].position =
		  (int *)Malloc(modelOrder * sizeof(int))))
			{
			fprintf(stderr, "Error: out of memory\n");
			exit(1);
			}

		(*auxCTemplates)[XCHR_INFO].size = modelOrder;
		for(n = 1 ; n <= modelOrder ; n++)
			(*auxCTemplates)[XCHR_INFO].position[modelOrder - n] = n;

		(*auxCModels)[XCHR_INFO] = CreateCModel(modelOrder,
		  bAlphabet->nXchrSymbols, 1, 1, maxCount);
		(*auxPModels)[XCHR_INFO] = CreatePModel(bAlphabet->nXchrSymbols);
		}

	// Char case information
	for(n = 0 ; n < ctxArgc ; n++)
		{
		if(strcmp("-cc", ctxArgv[n]) == 0)
			{
			if(sscanf(ctxArgv[n+1], "%d", &modelOrder) != 1)
				{
				fprintf(stderr, "Error: couldn't get model order\n");
				exit(1);
				}

			}

		}

	if(bAlphabet->caseType == CASE_BOTH)
		{
		if(!((*auxBuffers)[CASE_INFO] = (Symbol *)Calloc(modelOrder + 2,
		  sizeof(Symbol))) || !((*auxCTemplates)[CASE_INFO].position =
		  (int *)Malloc(modelOrder * sizeof(int))))
			{
			fprintf(stderr, "Error: out of memory\n");
			exit(1);
			}

		(*auxCTemplates)[CASE_INFO].size = modelOrder;
		for(n = 1 ; n <= modelOrder ; n++)
			(*auxCTemplates)[CASE_INFO].position[modelOrder - n] = n;

		(*auxCModels)[CASE_INFO] = CreateCModel2(modelOrder);
		(*auxPModels)[CASE_INFO] = CreatePModel(2);
		}

	}

