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
#include <limits.h>
#include <float.h>
#include <ctype.h>
#include <time.h>
#include <malloc.h>
#ifndef WINDOWS
#include <sys/wait.h>
#endif
#include <unistd.h>

#include "io.h"
#include "bitio.h"
#include "arith.h"
#include "arith_aux.h"
#include "defs.h"
#include "mem.h"
#include "fasta.h"
#include "fcm.h"

/*----------------------------------------------------------------------------*/
//
// This function collects the alphabets for the headers and sequences,
// the size of each of the headers and for each of the corresponding
// sequences. It also determines the size of the line if the sequence
// is written in more than one line
//
void CollectFileStatistics(FastaRecords *frs, HeaderAlphabet *hAlphabet,
  SequenceAlphabet *bAlphabet, Stream *sIn)

	{
	int c = GetC(sIn);

	while(c == '>')
		{
		NewFastaRecord(frs);
		frs->records[frs->nRecords - 1].start = Tell(sIn) - 1;

		// Get header information
		while((c = GetC(sIn)) != EOF)
			{
			hAlphabet->usage[c]++;
			frs->records[frs->nRecords - 1].headerSize++;
			if(c == '\n')
				break;

			}

		if(c == EOF)
			{
			fprintf(stderr, "Error: unexpected eof...\n");
			exit(1);
			}

		if(frs->records[frs->nRecords - 1].headerSize > frs->longestHeader)
			frs->longestHeader = frs->records[frs->nRecords - 1].headerSize;

		// Get sequence information
		// 
		// A sequence may be at a single line or be spread over several
		// lines. In the latter case, we assume that the lines are all
		// the same length, defined by the length of the first one
		while((c = GetC(sIn)) != EOF && c != '>')
			{
			if(c == '\n')
				{
				// Define the size of the lines
				if(!frs->records[frs->nRecords - 1].seqLineSize)
					frs->records[frs->nRecords - 1].seqLineSize =
					  frs->records[frs->nRecords - 1].seqSize;
				}

			else
				{
				bAlphabet->usage[c]++;
				frs->records[frs->nRecords - 1].seqSize++;
				}

			}

		if(frs->records[frs->nRecords - 1].seqSize > frs->longestSequence)
			frs->longestSequence = frs->records[frs->nRecords - 1].seqSize;

		// By convenience, the size of the line is set to zero if the
		// sequence is all in one line
		if(frs->records[frs->nRecords - 1].seqLineSize ==
		  frs->records[frs->nRecords - 1].seqSize)
			frs->records[frs->nRecords - 1].seqLineSize = 0;

		if(frs->records[frs->nRecords - 1].seqLineSize > frs->longestSeqLine)
			frs->longestSeqLine = frs->records[frs->nRecords - 1].seqLineSize;

#ifdef DEBUG_2
		printf("Record %u: hSize = %u, sSize = %u, lSize = %u\n",
		  frs->nRecords, frs->records[frs->nRecords - 1].headerSize,
		  frs->records[frs->nRecords - 1].seqSize,
		  frs->records[frs->nRecords - 1].seqLineSize);
#endif // DEBUG_2
		}

	if(!frs->nRecords)
		{
		fprintf(stderr, "Error: got '%c' while expecting '>' "
		  "at the beginning of the file...\n", c);
		exit(1);
		}

	// Set the number of bits required for writing the sequence sizes
	frs->nSeqSizeBits = 1;
	while(frs->longestSequence >> frs->nSeqSizeBits)
		frs->nSeqSizeBits++;

#ifdef DEBUG_2
	printf("Longest sequence: %u (using %u bits)\n", frs->longestSequence,
	  frs->nSeqSizeBits);
#endif // DEBUG_2

	// Set the number of bits required for writing the size of the
	// sequence lines. Note that although for a sequence the line size
	// is assumed constant, different sequences may have different
	// line sizes
	frs->nSeqLineSizeBits = 1;
	while(frs->longestSeqLine >> frs->nSeqLineSizeBits)
		frs->nSeqLineSizeBits++;

#ifdef DEBUG_2
	printf("Longest sequence line: %u (using %u bits)\n", frs->longestSeqLine,
	  frs->nSeqLineSizeBits);
#endif // DEBUG_2
	}

/*----------------------------------------------------------------------------*/

void EncodeHeader(FastaData *fd, HeaderAlphabet *alphabet, CModel *cModel,
  PModel *pModel, CTemplate2D *cTemplate, Stream *sOut)

	{
	unsigned col = 0;
	Symbol symbol;

	do
		{
		symbol = alphabet->symbols[fd->header[1][col]];
		cModel->pModelIdx = GetPModelIdx2D(fd->header, col, cTemplate,
		  alphabet, cModel);
		ComputePModel(cModel, pModel);
		ArithEncodeSymbol(alphabet->nSymbols, symbol,(int *)pModel->cum, sOut);
		UpdateCModelCounter(cModel, cModel->pModelIdx, symbol);

#ifdef DEBUG_1
		cModel->totalNats += PModelSymbolNatsLog(pModel, symbol);
#endif // DEBUG_1
		}
	while(fd->header[1][++col]);

	}

/*----------------------------------------------------------------------------*/

void EncodeSeqBlock(FastaData *fd, SequenceAlphabet *alphabet,
  unsigned nCModels, CModel **cModels, double *cModelNats, PModel *pModel,
  CTemplate *cTemplates, Symbol **auxBuffers, CModel **auxCModels,
  PModel **auxPModels, CTemplate *auxCTemplates, Stream *sOut)

	{
	Symbol symbol4, symbol4C, xchrSymbol = 0, caseSymbol;
	unsigned n, cModel, bestCModel = 0;
	double bestCModelNats;
	Symbol *symbolPtr;

	// Evaluate the coding efficiency for this block
	if(nCModels > 1)
		{
		for(cModel = 0 ; cModel < nCModels ; cModel++)
			{
			symbolPtr = fd->seqBlock;
			for(n = 0 ; n < fd->nBases ; n++)
				{
				symbol4 = *symbolPtr & 0x03;
				GetPModelIdx4(symbolPtr, cModels[cModel], &cTemplates[cModel]);
				ComputePModel4(cModels[cModel], pModel);
				cModelNats[cModel] += PModelSymbolNatsTable(pModel, symbol4);
				symbolPtr++;
				}

			// Force recalculation of pModelIdx
			cModels[cModel]->validPModelIdx = 0;
			}

		bestCModelNats = cModelNats[0];
		bestCModel = 0;
		for(cModel = 1 ; cModel < nCModels ; cModel++)
			if(cModelNats[cModel] < bestCModelNats)
				{
				bestCModelNats = cModelNats[cModel];
				bestCModel = cModel;
				}

		ShiftBuffer(auxBuffers[SIDE_INFO], auxCModels[SIDE_INFO]->ctxSize + 2,
		  bestCModel);
		GetInfoPModelIdx(auxBuffers[SIDE_INFO] + auxCModels[SIDE_INFO]->ctxSize,
		  auxCModels[SIDE_INFO]);
		ComputePModel(auxCModels[SIDE_INFO], auxPModels[SIDE_INFO]);
		ArithEncodeSymbol(nCModels, bestCModel,
		  (int *)auxPModels[SIDE_INFO]->cum, sOut);
		UpdateCModelCounter(auxCModels[SIDE_INFO],
		  auxCModels[SIDE_INFO]->pModelIdx, bestCModel);

#ifdef DEBUG_1
		auxCModels[SIDE_INFO]->totalNats +=
		  PModelSymbolNatsLog(auxPModels[SIDE_INFO], bestCModel);
#endif // DEBUG_1

		for(cModel = 0 ; cModel < nCModels ; cModel++)
			cModelNats[cModel] = 0;

		}

	// Encode this block
	symbolPtr = fd->seqBlock;
	for(n = 0 ; n < fd->nBases ; n++)
		{
		symbol4 = *symbolPtr & 0x03;
		GetPModelIdx4(symbolPtr, cModels[bestCModel], &cTemplates[bestCModel]);
		ComputePModel4(cModels[bestCModel], pModel);
		ArithEncodeSymbol(4, symbol4, (int *)(pModel->cum), sOut);
		UpdateCModelCounter4(cModels[bestCModel],
		  cModels[bestCModel]->pModelIdx, symbol4);

#ifdef DEBUG_1
		cModels[bestCModel]->totalNats += PModelSymbolNatsLog(pModel, symbol4);
#endif // DEBUG_1

		// All extra chars map to symbol "0" of the coding alphabet.
		// Therefore, if there are extra chars and if the symbol being
		// encoded is "0", we have to send additional information for
		// disambiguation
		if(!symbol4 && alphabet->nXchrSymbols)
			{
			xchrSymbol = alphabet->xchrSymbols[alphabet->chars[*symbolPtr]];
			ShiftBuffer(auxBuffers[XCHR_INFO], auxCModels[XCHR_INFO]->
			  ctxSize + 2, xchrSymbol);
			GetInfoPModelIdx(auxBuffers[XCHR_INFO] + auxCModels[XCHR_INFO]->
			  ctxSize, auxCModels[XCHR_INFO]);
			ComputePModel(auxCModels[XCHR_INFO], auxPModels[XCHR_INFO]);
			ArithEncodeSymbol(alphabet->nXchrSymbols, xchrSymbol,
			  (int *)auxPModels[XCHR_INFO]->cum, sOut);
			UpdateCModelCounter(auxCModels[XCHR_INFO],
			  auxCModels[XCHR_INFO]->pModelIdx, xchrSymbol);

#ifdef DEBUG_1
			auxCModels[XCHR_INFO]->totalNats +=
			  PModelSymbolNatsLog(auxPModels[XCHR_INFO], xchrSymbol);
#endif // DEBUG_1
			}

		// If both upper and lower cases occur, we have to know if
		// it is required to convert to lower case
		if(alphabet->caseType == CASE_BOTH && (symbol4 ||
		  (alphabet->nXchrSymbols && !xchrSymbol) || !alphabet->nXchrSymbols))
			{
			caseSymbol = isupper(alphabet->chars[*symbolPtr]) ? 0 : 1;
			ShiftBuffer(auxBuffers[CASE_INFO], auxCModels[CASE_INFO]->
			  ctxSize + 2, caseSymbol);
			GetPModelIdx2(auxBuffers[CASE_INFO] + auxCModels[CASE_INFO]->
			  ctxSize, auxCModels[CASE_INFO]);
			ComputePModel2(auxCModels[CASE_INFO], auxPModels[CASE_INFO]);
			ArithEncodeSymbol(2, caseSymbol,
			  (int *)auxPModels[CASE_INFO]->cum, sOut);
			UpdateCModelCounter2(auxCModels[CASE_INFO],
			  auxCModels[CASE_INFO]->pModelIdx, caseSymbol);

#ifdef DEBUG_1
			auxCModels[CASE_INFO]->totalNats +=
			  PModelSymbolNatsLog(auxPModels[CASE_INFO], caseSymbol);
#endif // DEBUG_1
			}

		// Update the reverse complement
		if(cModels[bestCModel]->updateIC)
			{
			GetPModelIdxAux4(symbolPtr, cModels[bestCModel],
			  &cTemplates[bestCModel]);
			symbol4C = 3 - (0x03 & *(symbolPtr -
			  cTemplates[bestCModel].deepestPosition));
			UpdateCModelCounter4(cModels[bestCModel],
			  cModels[bestCModel]->pModelIdxAux, symbol4C);
			}

		// Update the remaining models
		for(cModel = 0 ; cModel < nCModels ; cModel++)
			{
			if(cModel == bestCModel)
				continue;

			if(!cModels[cModel]->update)
				{
				cModels[cModel]->validPModelIdx = 0;
				cModels[cModel]->validPModelIdxAux = 0;
				continue;
				}

			GetPModelIdx4(symbolPtr, cModels[cModel], &cTemplates[cModel]);
			UpdateCModelCounter4(cModels[cModel],
			  cModels[cModel]->pModelIdx, symbol4);

			// Update the reverse complement
			if(cModels[cModel]->updateIC)
				{
				GetPModelIdxAux4(symbolPtr, cModels[cModel],
				  &cTemplates[cModel]);
				symbol4C = 3 - (0x03 & *(symbolPtr -
				  cTemplates[cModel].deepestPosition));
				UpdateCModelCounter4(cModels[cModel],
				  cModels[cModel]->pModelIdxAux, symbol4C);
				}

			}

		symbolPtr++;
		}

	}

/*----------------------------------------------------------------------------*/

void WriteFileHeader(FastaRecords *frs, HeaderAlphabet *hAlphabet,
  SequenceAlphabet *bAlphabet, char *hCtx, char *bCtx, char *iCtx, Stream *sOut)

	{
	unsigned n;

	WriteNBits(CODED_FILE_SIGNATURE, 32, sOut);
	WriteNBits(frs->nParts - 1, STORAGE_BITS_N_PARTS, sOut);
	if(frs->nParts > 1)
		for(n = 0 ; n < frs->nParts ; n++)
			{
			WriteNBits(frs->recordRange[n][0], STORAGE_BITS_RECORD, sOut);
			WriteNBits(frs->recordRange[n][1], STORAGE_BITS_RECORD, sOut);
			WriteNBits(frs->partSize[n] - 1, STORAGE_BITS_PART_SIZE, sOut);
			}

	WriteNBits(frs->nRecords - 1, STORAGE_BITS_N_RECORDS, sOut);
	WriteNBits(frs->nSeqSizeBits - 1, STORAGE_BITS_N_SIZE_BITS, sOut);
	WriteNBits(frs->nSeqLineSizeBits, STORAGE_BITS_N_SIZE_BITS, sOut);
	WriteNBits(frs->longestHeader, STORAGE_BITS_LONGEST_HEADER, sOut);
	WriteNBits(hAlphabet->nSymbols - 1, STORAGE_BITS_N_SYMBOLS, sOut);
	WriteNBits(bAlphabet->nSymbols - 1, STORAGE_BITS_N_SYMBOLS, sOut);

	for(n = 0 ; n < 256 ; n++)
		if(hAlphabet->usage[n])
			WriteNBits(n, STORAGE_BITS_SYMBOL, sOut);

	for(n = 0 ; n < 256 ; n++)
		if(bAlphabet->usage[n])
			WriteNBits(n, STORAGE_BITS_SYMBOL, sOut);

	do
		{
		WriteNBits(*hCtx, 8, sOut);
		}
	while(*hCtx++);

	do
		{
		WriteNBits(*bCtx, 8, sOut);
		}
	while(*bCtx++);

	do
		{
		WriteNBits(*iCtx, 8, sOut);
		}
	while(*iCtx++);

	}

/*----------------------------------------------------------------------------*/

int main(int argc, char *argv[])

	{
	int c;
	Symbol **auxBuffers;
	char *hCtx, *bCtx, *iCtx, *fNameOut = NULL, *tmpFName[MAX_PARTS];
	unsigned n, record, block, verbose, maxProcs, cModel, bNCModels, bSize;
	uint64_t seqSize;
	FILE *fpIn, *fpOut = NULL;
	FastaRecords *frs = CreateFastaRecords();
	FastaData *fd;
	HeaderAlphabet *hAlphabet;
	SequenceAlphabet *bAlphabet;
	CModel *hCModel = NULL, **bCModels, **auxCModels;
	CTemplate2D hCTemplate;
	CTemplate *bCTemplates, *auxCTemplates;
	PModel *hPModel, *bPModel, **auxPModels;
	double *bCModelNats;
	Char *tmp;
#ifndef WINDOWS
	pid_t pid = 0;
#endif
	Stream *sIn = CreateStream();
	Stream *sOut = CreateStream();
#ifdef DEBUG_1
	double totalNats = 0;
#endif // DEBUG_1

	FillLogTable();

	bCtx = strdup(SEQUENCE_CTX_2);
	verbose = 0;
	maxProcs = 2;
	frs->nParts = MAX_PARTS + 1; // Indicates that the number of parts was
								 // not provided.

	if(argc < 2)
		{
		fprintf(stderr, "Usage: MFCompressC [ -o EncodedFile ]\n");
		fprintf(stderr, "                   [ -v (verbose) ]\n");
		fprintf(stderr, "                   [ -V (version) ]\n");
		fprintf(stderr, "                   [ -1 (fast mode, less mem) ]\n");
		fprintf(stderr, "                   [ -2 (default mode) ]\n");
		fprintf(stderr, "                   [ -3 (best mode, more mem) ]\n");
		fprintf(stderr, "                   [ -p nParts ]\n");
		fprintf(stderr, "                   [ -t maxProcs (def 2) ]\n");
		fprintf(stderr, "                   MultiFastaFile\n");
		return 1;
		}

	for(n = 1 ; n < argc ; n++)
		if(!strcmp("-o", argv[n]))
			{
			fNameOut = argv[n+1];
			break;
			}

	for(n = 1 ; n < argc ; n++)
		if(!strcmp("-v", argv[n]))
			{
			verbose = 1;
			break;
			}

	for(n = 1 ; n < argc ; n++)
		if(!strcmp("-V", argv[n]))
			{
			printf("MFCompressC version 1.01\n");
			printf(COPYRIGHT_NOTICE);
			return 0;
			}

	for(n = 1 ; n < argc ; n++)
		if(!strcmp("-1", argv[n]))
			{
			bCtx = strdup(SEQUENCE_CTX_1);
			break;
			}

	for(n = 1 ; n < argc ; n++)
		if(!strcmp("-2", argv[n]))
			{
			bCtx = strdup(SEQUENCE_CTX_2);
			break;
			}

	for(n = 1 ; n < argc ; n++)
		if(!strcmp("-3", argv[n]))
			{
			bCtx = strdup(SEQUENCE_CTX_3);
			break;
			}

	for(n = 1 ; n < argc ; n++)
		if(!strcmp("-p", argv[n]))
			{
			frs->nParts = atoi(argv[n+1]);
			if(frs->nParts > MAX_PARTS)
				{
				fprintf(stderr, "Requested to create %u parts, but current "
				  "maximum is %u\n", frs->nParts, MAX_PARTS);
				fprintf(stderr, "To change it, please redefine MAX_PARTS and "
				  "rebuild the program\n");
				return 1;
				}

			break;
			}

	for(n = 1 ; n < argc ; n++)
		if(!strcmp("-t", argv[n]))
			{
			maxProcs = atoi(argv[n+1]);
			break;
			}

	// Find out if the file can be open for reading
	if(!(fpIn = fopen(argv[argc - 1], "rb")))
		{
		fprintf(stderr, "Error: unable to open file \"%s\"\n", argv[argc - 1]);
		return 1;
		}

	// Build the output file name in case it was not given
	if(!fNameOut)
		{
		fNameOut = Malloc(strlen(argv[argc - 1]) + 5); // .mfc + \0
		sprintf(fNameOut, "%s.mfc", argv[argc - 1]);
		}

	// Get file size
	fseeko(fpIn, 0, SEEK_END);
	frs->fileSize = ftello(fpIn);
	rewind(fpIn);

	// If the number of parts has not yet been defined, optimize it
	// according to the maximum number of parallel processes
	if(frs->nParts > MAX_PARTS)
		{
		for(n = 1 ; n < maxProcs ; n++)
			if(frs->fileSize / (n + 1) < MIN_AUTO_PART_SIZE)
				break;

		frs->nParts = n;
		}

	InitStream(sIn, fpIn, "r");

	if(!(hAlphabet = (HeaderAlphabet *)Calloc(1, sizeof(HeaderAlphabet))) ||
	   !(bAlphabet = (SequenceAlphabet *)Calloc(1, sizeof(SequenceAlphabet))))
		{
		fprintf(stderr, "Error: out of memory\n");
		return 1;
		}

	CollectFileStatistics(frs, hAlphabet, bAlphabet, sIn);

	// Parallel processing only applies to multi-fasta files
	if(frs->nRecords < frs->nParts)
		frs->nParts = frs->nRecords;

	if(frs->nParts > 1) // Create a multi-part file
		{
		frs->recordRange[0][0] = 0;
		for(n = 0 ; n < frs->nParts ; n++)
			{
			if(n)
				// Starting of part n is equal to end of n-1 part plus 1 record
				frs->recordRange[n][0] = frs->recordRange[n - 1][1] + 1;

			frs->recordRange[n][1] = frs->recordRange[n][0];
			while(frs->recordRange[n][1] < frs->nRecords - 1 &&
			  (frs->records[frs->recordRange[n][1] + 1].start -
			  frs->records[frs->recordRange[n][0]].start) <
			  frs->fileSize / frs->nParts)
				frs->recordRange[n][1]++;

			// Create temporary file names
			if(!(tmpFName[n] = tempnam(NULL, "mfc-")))
				{
				perror("tempnam");
				return 1;
				}

			if(frs->recordRange[n][1] == frs->nRecords - 1)
				{
				frs->nParts = n + 1;
				break;
				}

			}

		frs->recordRange[frs->nParts - 1][1] = frs->nRecords - 1;
		}

	if(frs->nParts == 0) // Create a part per record
		{
		if((frs->nParts = frs->nRecords) > MAX_PARTS)
			{
			fprintf(stderr, "Requested to create %u parts, but current "
			  "maximum is %u\n", frs->nParts, MAX_PARTS);
			fprintf(stderr, "To change it, please redefine MAX_PARTS and "
			  "rebuild the program\n");
			return 1;
			}

		for(n = 0 ; n < frs->nParts ; n++)
			{
			// Compute the record index range of each part
			frs->recordRange[n][0] = (frs->nRecords/frs->nParts) * n;
			if(n < frs->nParts - 1)
				frs->recordRange[n][1] =
				  (frs->nRecords / frs->nParts) * (n + 1) - 1;

			else
				frs->recordRange[n][1] = frs->nRecords - 1;

			// Create temporary file names
			if(!(tmpFName[n] = tempnam(NULL, "mfc-")))
				{
				perror("tempnam");
				return 1;
				}

			}

		}

	if(maxProcs > frs->nParts)
		maxProcs = frs->nParts;

	AssignHeaderSymbols(hAlphabet);
	AssignSequenceSymbols(bAlphabet);

	iCtx = SIDE_INFO_CTX;

	if(hAlphabet->nSymbols <= 50)
		hCtx = strdup(HEADER_CTX_2);

	else
		hCtx = strdup(HEADER_CTX_1);

	if(verbose)
		{
		printf("The file has %u records\n", frs->nRecords);
		printf("Creating %u part(s)\n", frs->nParts);
		printf("Found %u different header chars:\n", hAlphabet->nSymbols);
		PrintAlphabet(hAlphabet->usage);
		printf("Found %u different sequence chars:\n", bAlphabet->nSymbols);
		PrintAlphabet(bAlphabet->usage);
		}

	// Close the input file and reopen it below for encoding
	fclose(fpIn);

	if(frs->nParts > 1) // Multi-part
		{
#ifndef WINDOWS
		unsigned nProcsRunning = 0;
#endif
		unsigned k;

		for(n = 0 ; n < frs->nParts ; n++)
			{
#ifndef WINDOWS
			pid = fork();
			if(pid == -1)
				{
				perror("fork");
				return 1;
				}

			if(pid == 0)
				{
#endif
				if(verbose)
					printf("Part %d: encoding records from %d to %d\n", n + 1,
					  frs->recordRange[n][0], frs->recordRange[n][1]);

				InitDataStructs(hAlphabet, bAlphabet, hCtx, bCtx, iCtx,
				  &hCTemplate, &hCModel, &hPModel, &bNCModels, &bCTemplates,
				  &bCModels, &bPModel, &auxCTemplates, &auxCModels,
				  &auxPModels, &auxBuffers, &bSize);

				if(!(bCModelNats = (double *)Calloc(bNCModels, sizeof(double))))
					{
					fprintf(stderr, "Error: out of memory\n");
					return 1;
					}

				if((fpIn = fopen(argv[argc - 1], "rb")) == NULL)
					{
					perror("fopen");
					return 1;
					}

				InitStream(sIn, fpIn, "r");
				if(!(fpOut = fopen(tmpFName[n], "wb")))
					{
					perror("fopen");
					return 1;
					}

				// +1 to include the '\n'
				fd = CreateFastaData(frs->longestHeader + 1, bSize);

				InitStream(sOut, fpOut, "w");
				startoutputtingbits();
				start_encode();

				record = frs->recordRange[n][0];
				SeekSet(sIn, frs->records[record].start);
				GetC(sIn); // Discard the first '>'
				while(GetHeader(sIn, fd) && record <= frs->recordRange[n][1])
					{
					seqSize = frs->records[record].seqSize;
					EncodeHeader(fd, hAlphabet, hCModel, hPModel,
					  &hCTemplate, sOut);

					WriteNBits(seqSize - 1, frs->nSeqSizeBits, sOut);
					WriteNBits(frs->records[record].seqLineSize,
					  frs->nSeqLineSizeBits, sOut);

					for(block = 0 ; block < (seqSize - 1) / bSize + 1 ; block++)
						{
						GetSeqBlock(sIn, fd, bAlphabet, (block + 1) * bSize >
						  seqSize ? seqSize - block * bSize : bSize);
						EncodeSeqBlock(fd, bAlphabet, bNCModels, bCModels,
						  bCModelNats, bPModel, bCTemplates, auxBuffers,
						  auxCModels, auxPModels, auxCTemplates, sOut);

						}

					tmp = fd->header[1];
					fd->header[1] = fd->header[0];
					fd->header[0] = tmp;

					ResetFastaDataBuffers(fd);
					for(cModel = 0 ; cModel < bNCModels ; cModel++)
						{
						bCModels[cModel]->pModelIdx = 0;
						bCModels[cModel]->pModelIdxAux =
						  bCModels[cModel]->nPModels - 1;
						bCModels[cModel]->validPModelIdx = 1;
						bCModels[cModel]->validPModelIdxAux = 1;
						}

					record++;

					// Discard eventual blank lines between records.
					while((c = GetC(sIn)) == '\n')
						;

					if(c != '>')
						break;

					}

				finish_encode(sOut);
				doneoutputtingbits(sOut);
				FlushStream(sOut);
				fclose(fpOut);
#ifndef WINDOWS
				_exit(0);
				}

			else
				{
				nProcsRunning++;
				if(nProcsRunning == maxProcs)
					{
					wait(NULL); // Wait for one child to finish
					nProcsRunning--;
					}

				}
#else
			// Free as much memory as easily possible. NOTE: This is
			// just a workaround. Because in Windows we don't have
			// a process for each part of the file, we have to avoid
			// allocating too much memory... In the Linux implementation
			// that memory is automatically freed when the processes
			// terminate.
			switch(hCModel->array.type)
				{
				case COUNTERS_16:
					Free(hCModel->array.counters16, (size_t)hCModel->nPModels *
					  hCModel->nSymbols * sizeof(Counter16));
					break;

				case COUNTERS_08:
					Free(hCModel->array.counters08, (size_t)hCModel->nPModels *
					  hCModel->nSymbols * sizeof(Counter08));
					break;

				}

			for(k = 0 ; k < bNCModels ; k++)
				switch(bCModels[k]->array.type)
					{
					case COUNTERS_16:
						Free(bCModels[k]->array.counters16,(size_t)bCModels[k]->
						  nPModels * bCModels[k]->nSymbols * sizeof(Counter16));
						break;

					case COUNTERS_08:
						Free(bCModels[k]->array.counters08,(size_t)bCModels[k]->
						  nPModels * bCModels[k]->nSymbols * sizeof(Counter08));
						break;

					case COUNTERS_04:
						Free(bCModels[k]->array.counters04,(size_t)bCModels[k]->
						  nPModels * 2 * sizeof(Counter04));
						break;

					case COUNTERS_02:
						Free(bCModels[k]->array.counters02,(size_t)bCModels[k]->
						  nPModels * sizeof(Counter02));
						break;

					}

#endif

			}

#ifndef WINDOWS
		// Wait for the remaining children to finish
		for(k = 0 ; k < nProcsRunning ; k++)
			wait(NULL);
#endif

		unsigned nBytes;
		char buf[IO_BUF_SIZE];

		for(n = 0 ; n < frs->nParts ; n++)
			{
			if(!(fpIn = fopen(tmpFName[n], "rb")))
				{
				perror("fopen");
				return 1;
				}

			fseeko(fpIn, 0, SEEK_END);
			frs->partSize[n] = ftello(fpIn);
			fclose(fpIn);
			}

		if(!(fpOut = fopen(fNameOut, "wb")))
			{
			perror("fopen");
			return 1;
			}

		InitStream(sOut, fpOut, "w");
		startoutputtingbits();
		start_encode();

		WriteFileHeader(frs, hAlphabet, bAlphabet, hCtx, bCtx, iCtx, sOut);

		finish_encode(sOut);
		doneoutputtingbits(sOut);
		FlushStream(sOut);
		fclose(fpOut);

		if(!(fpOut = fopen(fNameOut, "ab")))
			{
			perror("fopen");
			return 1;
			}

		for(n = 0 ; n < frs->nParts ; n++)
			{
			if(!(fpIn = fopen(tmpFName[n], "rb")))
				{
				perror("fopen");
				return 1;
				}

			while((nBytes = fread(buf, 1, IO_BUF_SIZE, fpIn)))
				fwrite(buf, 1, nBytes, fpOut);

			fclose(fpIn);
			unlink(tmpFName[n]);
			}

		fclose(fpOut);
		}

	else // One part
		{
		InitDataStructs(hAlphabet, bAlphabet, hCtx, bCtx, iCtx, &hCTemplate,
		  &hCModel, &hPModel, &bNCModels, &bCTemplates, &bCModels, &bPModel,
		  &auxCTemplates, &auxCModels, &auxPModels, &auxBuffers, &bSize);

		if(!(bCModelNats = (double *)Calloc(bNCModels, sizeof(double))))
			{
			fprintf(stderr, "Error: out of memory\n");
			return 1;
			}

		if((fpIn = fopen(argv[argc - 1], "rb")) == NULL)
			{
			perror("fopen");
			return 1;
			}

		// +1 to include the '\n'
		fd = CreateFastaData(frs->longestHeader + 1, bSize);

		InitStream(sIn, fpIn, "r");
		if(!(fpOut = fopen(fNameOut, "wb")))
			{
			perror("fopen");
			return 1;
			}

		InitStream(sOut, fpOut, "w");
		startoutputtingbits();
		start_encode();

		WriteFileHeader(frs, hAlphabet, bAlphabet, hCtx, bCtx, iCtx, sOut);

		GetC(sIn); // Discard the first '>'
		record = 0;
		while(GetHeader(sIn, fd))
			{
			seqSize = frs->records[record].seqSize;

			EncodeHeader(fd, hAlphabet, hCModel, hPModel, &hCTemplate, sOut);

			WriteNBits(seqSize - 1, frs->nSeqSizeBits, sOut);
			WriteNBits(frs->records[record].seqLineSize,
			  frs->nSeqLineSizeBits, sOut);

			for(block = 0 ; block < (seqSize - 1) / bSize + 1 ; block++)
				{
				GetSeqBlock(sIn, fd, bAlphabet, (block + 1) * bSize > seqSize ?
				  seqSize - block * bSize : bSize);
				EncodeSeqBlock(fd, bAlphabet, bNCModels, bCModels,
				  bCModelNats, bPModel, bCTemplates, auxBuffers, auxCModels,
				  auxPModels, auxCTemplates, sOut);
				}

			tmp = fd->header[1];
			fd->header[1] = fd->header[0];
			fd->header[0] = tmp;

			ResetFastaDataBuffers(fd);
			for(cModel = 0 ; cModel < bNCModels ; cModel++)
				{
				bCModels[cModel]->pModelIdx = 0;
				bCModels[cModel]->pModelIdxAux =
				  bCModels[cModel]->nPModels - 1;
				bCModels[cModel]->validPModelIdx = 1;
				bCModels[cModel]->validPModelIdxAux = 1;
				}

			record++;

			// Discard eventual blank lines between records.
			while((c = GetC(sIn)) == '\n')
				;

			if(c != '>')
				break;

			}

		finish_encode(sOut);
		doneoutputtingbits(sOut);
		FlushStream(sOut);
		fclose(fpOut);
		}

#ifdef DEBUG_1
	if(frs->nParts == 1)
		{
		printf("Total header bytes: %u\n",
		  (unsigned)(hCModel->totalNats / M_LN2 / 8));

		for(cModel = 0 ; cModel < bNCModels ; cModel++)
			totalNats += bCModels[cModel]->totalNats;

		printf("Total ACGT bytes: %u\n", (unsigned)(totalNats / M_LN2 / 8));

		for(cModel = 0 ; cModel < bNCModels ; cModel++)
			printf("  Model %u ACGT bytes: %u\n", cModel,
			  (unsigned)(bCModels[cModel]->totalNats / M_LN2 / 8));

		if(bNCModels > 1)
			printf("Total side info bytes: %u\n",
			  (unsigned)(auxCModels[SIDE_INFO]->totalNats / M_LN2 / 8));

		if(bAlphabet->nXchrSymbols)
			printf("Total extra char bytes: %u\n",
			  (unsigned)(auxCModels[XCHR_INFO]->totalNats / M_LN2 / 8));

		if(bAlphabet->caseType == CASE_BOTH)
			printf("Total char case bytes: %u\n",
			  (unsigned)(auxCModels[CASE_INFO]->totalNats / M_LN2 / 8));
		}
#endif // DEBUG_1

	return 0;
	}

