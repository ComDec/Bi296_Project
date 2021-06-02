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
#include <inttypes.h>

#include "io.h"
#include "bitio.h"
#include "arith.h"
#include "arith_aux.h"
#include "defs.h"
#include "mem.h"
#include "fasta.h"
#include "fcm.h"
 
/*----------------------------------------------------------------------------*/

void DecodeHeader(FastaData *fd, HeaderAlphabet *alphabet, CModel *cModel,
  PModel *pModel, CTemplate2D *cTemplate, Stream *sIn, Stream *sOut)

	{
	unsigned col = 0;
	Symbol symbol;

	PutC('>', sOut);
	do
		{
		cModel->pModelIdx = GetPModelIdx2D(fd->header, col, cTemplate,
		  alphabet, cModel);
		ComputePModel(cModel, pModel);
		symbol = ArithDecodeSymbol(alphabet->nSymbols,(int *)pModel->cum, sIn);
		fd->header[1][col] = alphabet->chars[symbol];
		PutC(fd->header[1][col], sOut);
		UpdateCModelCounter(cModel, cModel->pModelIdx, symbol);
		}
	while(fd->header[1][col++] != '\n');

	fd->header[1][col] = '\0';
	}

/*----------------------------------------------------------------------------*/

void DecodeSeqBlock(FastaData *fd, SequenceAlphabet *alphabet,
  unsigned nCModels, CModel **cModels, PModel *pModel, CTemplate *cTemplates,
  Symbol **auxBuffers, CModel **auxCModels, PModel **auxPModels,
  CTemplate *auxCTemplates, uint64_t seqLineSize, Stream *sIn, Stream *sOut)

	{
	Symbol symbol4, symbol4C, xchrSymbol = 0, caseSymbol;
	unsigned n, cModel, blockCModel = 0;
	Char *symbolPtr;

	if(nCModels > 1)
		{
		ShiftBuffer(auxBuffers[SIDE_INFO], auxCModels[SIDE_INFO]->
		  ctxSize + 2, 0);
		GetInfoPModelIdx(auxBuffers[SIDE_INFO] + auxCModels[SIDE_INFO]->
		  ctxSize, auxCModels[SIDE_INFO]);
		ComputePModel(auxCModels[SIDE_INFO], auxPModels[SIDE_INFO]);
		blockCModel = ArithDecodeSymbol(nCModels,
		  (int *)auxPModels[SIDE_INFO]->cum, sIn);
		UpdateCModelCounter(auxCModels[SIDE_INFO],
		  auxCModels[SIDE_INFO]->pModelIdx, blockCModel);
		auxBuffers[SIDE_INFO][auxCModels[SIDE_INFO]->ctxSize + 1] = blockCModel;
		}

	// Decode this block
	symbolPtr = fd->seqBlock;
	for(n = 0 ; n < fd->nBases ; n++)
		{
		GetPModelIdx4(symbolPtr, cModels[blockCModel],&cTemplates[blockCModel]);
		ComputePModel4(cModels[blockCModel], pModel);
		symbol4 = ArithDecodeSymbol(4, (int *)pModel->cum, sIn);

		// It might not yet be the correct char...
		// If "symbol4" is '0', it might be an "extra" char. If it is not
		// an "extra" char, it may require to be case converted.
		// Case conversion is only required if both lower and upper case
		// chars occur; upper case is the default mode
		*symbolPtr = symbol4;

		UpdateCModelCounter4(cModels[blockCModel],
		  cModels[blockCModel]->pModelIdx, symbol4);

		// All extra chars map to symbol '0' of the coding alphabet.
		// Therefore, if there are extra chars and if the symbol being
		// encoded is '0', we have to send additional information for
		// disambiguation
		if(!symbol4 && alphabet->nXchrSymbols)
			{
			ShiftBuffer(auxBuffers[XCHR_INFO], auxCModels[XCHR_INFO]->
			  ctxSize + 2, 0);
			GetInfoPModelIdx(auxBuffers[XCHR_INFO] + auxCModels[XCHR_INFO]->
			  ctxSize, auxCModels[XCHR_INFO]);
			ComputePModel(auxCModels[XCHR_INFO], auxPModels[XCHR_INFO]);
			xchrSymbol = ArithDecodeSymbol(alphabet->nXchrSymbols,
			  (int *)auxPModels[XCHR_INFO]->cum, sIn);
			UpdateCModelCounter(auxCModels[XCHR_INFO],
			  auxCModels[XCHR_INFO]->pModelIdx, xchrSymbol);
			auxBuffers[XCHR_INFO][auxCModels[XCHR_INFO]->ctxSize + 1] =
			  xchrSymbol;
			if(xchrSymbol)
				*symbolPtr = (xchrSymbol + 1) << 2;

			}

		// If both upper and lower cases occur, we have to know if
		// it is required to convert to lower case
		if(alphabet->caseType == CASE_BOTH && (symbol4 ||
		  (alphabet->nXchrSymbols && !xchrSymbol) || !alphabet->nXchrSymbols))
			{
			ShiftBuffer(auxBuffers[CASE_INFO], auxCModels[CASE_INFO]->
			  ctxSize + 2, 0);
			GetPModelIdx2(auxBuffers[CASE_INFO] + auxCModels[CASE_INFO]->
			  ctxSize, auxCModels[CASE_INFO]);
			ComputePModel2(auxCModels[CASE_INFO], auxPModels[CASE_INFO]);
			caseSymbol = ArithDecodeSymbol(2,
			  (int *)auxPModels[CASE_INFO]->cum, sIn);
			UpdateCModelCounter2(auxCModels[CASE_INFO],
			  auxCModels[CASE_INFO]->pModelIdx, caseSymbol);
			auxBuffers[CASE_INFO][auxCModels[CASE_INFO]->ctxSize + 1] =
			  caseSymbol;
			if(caseSymbol) // 0: upper; 1: lower
				*symbolPtr = symbol4 + 4;

			}

		PutC(alphabet->chars[*symbolPtr], sOut);
		if(++fd->seqSize % seqLineSize == 0)
			PutC('\n', sOut);

		// Update the reverse complement
		if(cModels[blockCModel]->updateIC)
			{
			GetPModelIdxAux4(symbolPtr, cModels[blockCModel],
			  &cTemplates[blockCModel]);
			symbol4C = 3 - (0x03 & *(symbolPtr -
			  cTemplates[blockCModel].deepestPosition));
			UpdateCModelCounter4(cModels[blockCModel],
			  cModels[blockCModel]->pModelIdxAux, symbol4C);
			}

		// Update the remaining models
		for(cModel = 0 ; cModel < nCModels ; cModel++)
			{
			if(cModel == blockCModel)
				continue;

			GetPModelIdx4(symbolPtr, cModels[cModel], &cTemplates[cModel]);
			if(!cModels[cModel]->update)
				{
				cModels[cModel]->validPModelIdx = 0;
				cModels[cModel]->validPModelIdxAux = 0;
				continue;
				}

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

void ReadFileHeader(FastaRecords *frs, HeaderAlphabet *hAlphabet,
  SequenceAlphabet *bAlphabet, char *hCtx, char *bCtx, char *iCtx, Stream *sIn)

	{
	unsigned n;

	if(ReadNBits(32, sIn) != CODED_FILE_SIGNATURE)
		{
		fprintf(stderr, "Coded file signature not found... "
		  "Aborting decompression.\n");
		exit(1);
		}

	frs->nParts = ReadNBits(STORAGE_BITS_N_PARTS, sIn) + 1;
	if(frs->nParts > 1)
		for(n = 0 ; n < frs->nParts ; n++)
			{
			frs->recordRange[n][0] = ReadNBits(STORAGE_BITS_RECORD, sIn);
			frs->recordRange[n][1] = ReadNBits(STORAGE_BITS_RECORD, sIn);
			frs->partSize[n] = ReadNBits(STORAGE_BITS_PART_SIZE, sIn) + 1;
			}

	frs->nRecords = ReadNBits(STORAGE_BITS_N_RECORDS, sIn) + 1;
	frs->nSeqSizeBits = ReadNBits(STORAGE_BITS_N_SIZE_BITS, sIn) + 1;
	frs->nSeqLineSizeBits = ReadNBits(STORAGE_BITS_N_SIZE_BITS, sIn);
	frs->longestHeader = ReadNBits(STORAGE_BITS_LONGEST_HEADER, sIn);
	hAlphabet->nSymbols = ReadNBits(STORAGE_BITS_N_SYMBOLS, sIn) + 1;
	bAlphabet->nSymbols = ReadNBits(STORAGE_BITS_N_SYMBOLS, sIn) + 1;

	for(n = 0 ; n < hAlphabet->nSymbols ; n++)
		hAlphabet->usage[ReadNBits(STORAGE_BITS_SYMBOL, sIn)] = 1;

	for(n = 0 ; n < bAlphabet->nSymbols ; n++)
		bAlphabet->usage[ReadNBits(STORAGE_BITS_SYMBOL, sIn)] = 1;

	n = 0;
	while((hCtx[n++] = ReadNBits(8, sIn)))
		;

	n = 0;
	while((bCtx[n++] = ReadNBits(8, sIn)))
		;

	n = 0;
	while((iCtx[n++] = ReadNBits(8, sIn)))
		;

	}

/*----------------------------------------------------------------------------*/

int main(int argc, char *argv[])

	{
	Symbol **auxBuffers;
	char hCtx[512], bCtx[512], iCtx[512], *fNameOut = NULL,
	  *tmpFName[MAX_PARTS];
	off_t fileHeaderSize, initialFilePos;
	unsigned n, record, block, verbose, maxProcs, cModel, bNCModels,
	  recordRange[2], info, bSize;
	uint64_t seqSize, seqLineSize;
	FILE *fpIn, *fpOut = NULL;
	FastaRecords *frs = CreateFastaRecords();
	FastaData *fd;
	HeaderAlphabet *hAlphabet;
	SequenceAlphabet *bAlphabet;
	CModel *hCModel = NULL, **bCModels, **auxCModels;
	CTemplate2D hCTemplate;
	CTemplate *bCTemplates, *auxCTemplates;
	PModel *hPModel, *bPModel, **auxPModels;
	Char *tmp;
#ifndef WINDOWS
	pid_t pid = 0;
#endif
	Stream *sIn = CreateStream();
	Stream *sOut = CreateStream();
	Stream *sNull = CreateStream();
	Stream *s;

	verbose = 0;
	info = 0;
	maxProcs = 2;
	recordRange[0] = 0;
	recordRange[1] = UINT_MAX; // If not changed, then extract all records

	if(argc < 2)
		{
		fprintf(stderr, "Usage: MFCompressD [ -o MultiFastaFile ]\n");
		fprintf(stderr, "                   [ -v (verbose) ]\n");
		fprintf(stderr, "                   [ -V (verbose) ]\n");
		fprintf(stderr, "                   [ -t maxProcs (def 2) ]\n");
		fprintf(stderr, "                   [ -r recordRange (i:j or i) ]\n");
		fprintf(stderr, "                   [ -i (display file info) ]\n");
		fprintf(stderr, "                   EncodedFile\n");
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
			printf("MFCompressD version 1.01\n");
			printf(COPYRIGHT_NOTICE);
			return 0;
			}

	for(n = 1 ; n < argc ; n++)
		if(!strcmp("-i", argv[n]))
			{
			info = 1;
			break;
			}

	for(n = 1 ; n < argc ; n++)
		if(!strcmp("-t", argv[n]))
			{
			maxProcs = atoi(argv[n+1]);
			break;
			}

	for(n = 1 ; n < argc ; n++)
		if(!strcmp("-r", argv[n]))
			{
			if(sscanf(argv[n+1], "%d:%d", &recordRange[0],&recordRange[1]) == 2)
				{
				recordRange[0]--;
				recordRange[1]--;
				break;
				}

			if(sscanf(argv[n+1], "%d", &recordRange[0]) == 1)
				{
				recordRange[1] = --recordRange[0];
				break;
				}

			break;
			}

	if(!(fpIn = fopen(argv[argc - 1], "rb")))
		{
		fprintf(stderr, "Error: unable to open the encoded file\n");
		return 1;
		}

	// Build the output file name in case it was not given
	if(!fNameOut)
		{
		fNameOut = Malloc(strlen(argv[argc - 1]) + 3); // .d + \0
		sprintf(fNameOut, "%s.d", argv[argc - 1]);
		}

	InitStream(sIn, fpIn, "r");
	InitStream(sNull, fopen(DEV_NULL, "wb"), "w");

	if(!(hAlphabet = (HeaderAlphabet *)Calloc(1, sizeof(HeaderAlphabet))) ||
	   !(bAlphabet = (SequenceAlphabet *)Calloc(1, sizeof(SequenceAlphabet))))
		{
		fprintf(stderr, "Error: out of memory\n");
		return 1;
		}

	startinputtingbits();
	start_decode(sIn);

	ReadFileHeader(frs, hAlphabet, bAlphabet, hCtx, bCtx, iCtx, sIn);

	if(recordRange[1] == UINT_MAX) // If UINT_MAX, then extract all records
		{
		recordRange[0] = 0;
		recordRange[1] = frs->nRecords - 1;
		}

	else
		{
		if(recordRange[1] > frs->nRecords - 1)
			recordRange[1] = frs->nRecords - 1;

		if(recordRange[0] > recordRange[1])
			recordRange[0] = recordRange[1];

		}

	fileHeaderSize = Tell(sIn);
	fclose(fpIn);

	if(maxProcs > frs->nParts)
		maxProcs = frs->nParts;

	if(frs->nParts > 1) // This is a multi-part file
		{
		for(n = 0 ; n < frs->nParts ; n++)
			{
			// Create temporary file names
			if(!(tmpFName[n] = tempnam(NULL, "mfd-")))
				{
				perror("tempnam");
				return 1;
				}

			}

		}

	AssignHeaderSymbols(hAlphabet);
	AssignSequenceSymbols(bAlphabet);

	if(info)
		{
		printf("File has %u part(s) and %u record(s)\n",
		  frs->nParts, frs->nRecords);
		if(frs->nParts > 1)
			for(n = 0 ; n < frs->nParts ; n++)
				printf("    Part %d has %"PRId64" bytes (records %u:%u)\n", n+1,
				  frs->partSize[n], frs->recordRange[n][0] + 1,
				  frs->recordRange[n][1] + 1);

		printf("Got %u different header chars:\n", hAlphabet->nSymbols);
		PrintAlphabet(hAlphabet->usage);
		printf("Got %u different sequence chars:\n", bAlphabet->nSymbols);
		PrintAlphabet(bAlphabet->usage);
		printf("Header context string: %s\n", hCtx);
		printf("Sequence context string: %s\n", bCtx);
		printf("AuxInfo context string: %s\n", iCtx);
		return 1;
		}

	if(verbose)
		{
		printf("File has %u part(s) and %u record(s)\n",
		  frs->nParts, frs->nRecords);
		if(frs->nParts > 1)
			for(n = 0 ; n < frs->nParts ; n++)
				printf("    Part %d has %"PRId64" bytes (records %u:%u)\n", n+1,
				  frs->partSize[n], frs->recordRange[n][0] + 1,
				  frs->recordRange[n][1] + 1);

		printf("Decoding a total of %u records (%u:%u)\n",
		  recordRange[1] - recordRange[0] + 1, recordRange[0] + 1,
		  recordRange[1] + 1);
		printf("Got %u different header chars:\n", hAlphabet->nSymbols);
		PrintAlphabet(hAlphabet->usage);
		printf("Got %u different sequence chars:\n", bAlphabet->nSymbols);
		PrintAlphabet(bAlphabet->usage);
		printf("Header context string: %s\n", hCtx);
		printf("Sequence context string: %s\n", bCtx);
		printf("AuxInfo context string: %s\n", iCtx);
		}

	if(frs->nParts > 1) // Multi-part
		{
#ifndef WINDOWS
		unsigned nProcsRunning = 0;
#endif
		unsigned k;

		for(n = 0 ; n < frs->nParts ; n++)
			{
			if(recordRange[0] > frs->recordRange[n][1] ||
			  recordRange[1] < frs->recordRange[n][0])
				continue;

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
				initialFilePos = fileHeaderSize;

				InitDataStructs(hAlphabet, bAlphabet, hCtx, bCtx, iCtx,
				  &hCTemplate, &hCModel, &hPModel, &bNCModels, &bCTemplates,
				  &bCModels, &bPModel, &auxCTemplates, &auxCModels, &auxPModels,
				  &auxBuffers, &bSize);

				// +1 to include the '\n'
				fd = CreateFastaData(frs->longestHeader + 1, bSize);

				for(k = 0 ; k < n ; k++)
					initialFilePos += frs->partSize[k];

				if((fpIn = fopen(argv[argc - 1], "rb")) == NULL)
					{
					perror("fopen");
					return 1;
					}

				InitStream(sIn, fpIn, "r");
				SeekSet(sIn, initialFilePos);

				if(verbose)
					printf("Part %d: decoding %"PRId64" bytes starting "
					  "at %"PRId64"\n", n+1, frs->partSize[n], initialFilePos);

				startinputtingbits();
				start_decode(sIn);

				if(!(fpOut = fopen(tmpFName[n], "wb")))
					{
					perror("fopen");
					return 1;
					}

				InitStream(sOut, fpOut, "w");
				record = frs->recordRange[n][0];

				while(Tell(sIn) < initialFilePos + frs->partSize[n])
					{
					if(record < recordRange[0] || record > recordRange[1])
						s = sNull;

					else
						s = sOut;

					DecodeHeader(fd, hAlphabet, hCModel, hPModel,
					  &hCTemplate, sIn, s);

					seqSize = ReadNBits(frs->nSeqSizeBits, sIn) + 1;
					if(!(seqLineSize = ReadNBits(frs->nSeqLineSizeBits, sIn)))
						seqLineSize = seqSize;

					fd->seqSize = 0; // Number of bases decoded in this record

					for(block = 0 ; block < (seqSize - 1) / bSize + 1 ; block++)
						{
						fd->nBases = (block + 1) * bSize > seqSize ?
						  seqSize - block * bSize : bSize;
						memcpy(fd->seqBlock - LEFT_GUARD, fd->seqBlock +
						  bSize - LEFT_GUARD, LEFT_GUARD);
						DecodeSeqBlock(fd, bAlphabet, bNCModels, bCModels,
						  bPModel, bCTemplates, auxBuffers, auxCModels,
						  auxPModels, auxCTemplates, seqLineSize, sIn, s);
						}

					if(fd->seqSize % seqLineSize != 0)
						PutC('\n', s);

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
					}

				finish_decode();
				doneinputtingbits();
				fclose(fpIn);

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

		if(!(fpOut = fopen(fNameOut, "wb")))
			{
			perror("fopen");
			return 1;
			}

		for(n = 0 ; n < frs->nParts ; n++)
			{
			if(recordRange[0] > frs->recordRange[n][1] ||
			  recordRange[1] < frs->recordRange[n][0])
				continue;

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

	else // Single part
		{
		InitDataStructs(hAlphabet, bAlphabet, hCtx, bCtx, iCtx, &hCTemplate,
		  &hCModel, &hPModel, &bNCModels, &bCTemplates, &bCModels, &bPModel,
		  &auxCTemplates, &auxCModels, &auxPModels, &auxBuffers, &bSize);

		// +1 to include the '\n'
		fd = CreateFastaData(frs->longestHeader + 1, bSize);

		if((fpIn = fopen(argv[argc - 1], "rb")) == NULL)
			{
			perror("fopen");
			return 1;
			}

		InitStream(sIn, fpIn, "r");
		SeekSet(sIn, fileHeaderSize);

		if(!(fpOut = fopen(fNameOut, "wb")))
			{
			perror("fopen");
			return 1;
			}

		InitStream(sOut, fpOut, "w");

		for(record = 0 ; record < frs->nRecords ; record++)
			{
			DecodeHeader(fd, hAlphabet, hCModel, hPModel, &hCTemplate,
			  sIn, sOut);

			seqSize = ReadNBits(frs->nSeqSizeBits, sIn) + 1;
			if(!(seqLineSize = ReadNBits(frs->nSeqLineSizeBits, sIn)))
				seqLineSize = seqSize;

			fd->seqSize = 0; // Number of bases decoded in this record
			for(block = 0 ; block < (seqSize - 1) / bSize + 1 ; block++)
				{
				fd->nBases = (block + 1) * bSize > seqSize ?
				  seqSize - block * bSize : bSize;
				memcpy(fd->seqBlock - LEFT_GUARD, fd->seqBlock + bSize -
				  LEFT_GUARD, LEFT_GUARD);
				DecodeSeqBlock(fd, bAlphabet, bNCModels, bCModels, bPModel,
				  bCTemplates, auxBuffers, auxCModels, auxPModels,
				  auxCTemplates, seqLineSize, sIn, sOut);
				}

			if(fd->seqSize % seqLineSize != 0)
				PutC('\n', sOut);

			tmp = fd->header[1];
			fd->header[1] = fd->header[0];
			fd->header[0] = tmp;

			ResetFastaDataBuffers(fd);
			for(cModel = 0 ; cModel < bNCModels ; cModel++)
				{
				bCModels[cModel]->pModelIdx = 0;
				bCModels[cModel]->pModelIdxAux = bCModels[cModel]->nPModels - 1;
				bCModels[cModel]->validPModelIdx = 1;
				bCModels[cModel]->validPModelIdxAux = 1;
				}

			}

		finish_decode();
		doneinputtingbits();
		fclose(fpIn);

		FlushStream(sOut);
		fclose(fpOut);
		}

	return 0;
	}

