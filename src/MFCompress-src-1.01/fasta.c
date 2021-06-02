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
#include <ctype.h>
#include <string.h>
#include "defs.h"
#include "io.h"
#include "mem.h"
#include "fasta.h"

/*----------------------------------------------------------------------------*/

FastaRecords *CreateFastaRecords()

	{
	FastaRecords *frs;

	if(!(frs = (FastaRecords *)Calloc(1, sizeof(FastaRecords))))
		{
		fprintf(stderr, "Error: out of memory\n");
		exit(1);
		}

	return frs;
	}

/*----------------------------------------------------------------------------*/

void NewFastaRecord(FastaRecords *frs)

	{
	if(!frs->nRecords)
		{
		if(!(frs->records = (FastaRecord *)Malloc(sizeof(FastaRecord))))
			{
			fprintf(stderr, "Error: out of memory\n");
			exit(1);
			}

		}

	else
		{
		if(!(frs->records = (FastaRecord *)Realloc(frs->records,
		  (frs->nRecords + 1) * sizeof(FastaRecord), sizeof(FastaRecord))))
			{
			fprintf(stderr, "Error: out of memory\n");
			exit(1);
			}

		}

	frs->records[frs->nRecords].headerSize = 0;
	frs->records[frs->nRecords].seqSize = 0;
	frs->records[frs->nRecords].seqLineSize = 0;
	frs->records[frs->nRecords].start = 0;
	frs->nRecords++;
	}

/*----------------------------------------------------------------------------*/

FastaData *CreateFastaData(unsigned longestHeader, unsigned bSize)

	{
	FastaData *fd;

	if(!(fd = (FastaData *)Calloc(1, sizeof(FastaData))))
		{
		fprintf(stderr, "Error: out of memory\n");
		exit(1);
		}

	// The header from the previous record
	if(!(fd->header[0] = (Char *)Calloc(longestHeader + LEFT_GUARD,
	  sizeof(Char))))
		{
		fprintf(stderr, "Error: out of memory\n");
		exit(1);
		}

	// The header from the current record
	if(!(fd->header[1] = (Char *)Calloc(longestHeader + LEFT_GUARD,
	  sizeof(Char))))
		{
		fprintf(stderr, "Error: out of memory\n");
		exit(1);
		}

	// The sequence is compressed in blocks
	if(!(fd->seqBlock = (Char *)Calloc(bSize + LEFT_GUARD, sizeof(Char))))
		{
		fprintf(stderr, "Error: out of memory\n");
		exit(1);
		}

	fd->header[0] += LEFT_GUARD;
	fd->header[1] += LEFT_GUARD;

	// This left guard must be set to a constant value (note that '\n'
	// always belongs to the header alphabet)
	memset(fd->header[0] - LEFT_GUARD, '\n', longestHeader + LEFT_GUARD - 1);
	memset(fd->header[1] - LEFT_GUARD, '\n', longestHeader + LEFT_GUARD - 1);

	fd->seqBlock += LEFT_GUARD;

	// The block is initialized with a constant value (including the
	// left guard)
	memset(fd->seqBlock - LEFT_GUARD, 0, bSize + LEFT_GUARD);

	fd->longestHeader = longestHeader;
	fd->bSize = bSize;
	return fd;
	}

/*----------------------------------------------------------------------------*/

void ResetFastaDataBuffers(FastaData *fd)

	{
	// This left guard must be set to a constant value (note that '\n'
	// always belongs to the header alphabet)
	memset(fd->header[1] - LEFT_GUARD, '\n',fd->longestHeader + LEFT_GUARD - 1);

	// The block is initialized with a constant value (including the
	// left guard)
	memset(fd->seqBlock - LEFT_GUARD, 0, fd->bSize + LEFT_GUARD);
	}

/*----------------------------------------------------------------------------*/
//
// This function reads all chars until the next newline, including it.
// The initial '>' is not included, and it is expected to have been
// already removed from the stream.
//
FastaData *GetHeader(Stream *s, FastaData *fd)

	{
	if(!GetS((char *)fd->header[1], fd->longestHeader, s))
		return NULL;

	return fd;
	}

/*----------------------------------------------------------------------------*/
//
//	This function reads everything until a '>' or until EOF, but at most
//	bSize chars
//
int GetSeqBlock(Stream *s, FastaData *fd, SequenceAlphabet *alphabet,
  unsigned nBases)

	{
	int c;

	memcpy(fd->seqBlock - LEFT_GUARD, fd->seqBlock + fd->bSize - LEFT_GUARD,
	  LEFT_GUARD);

	if(nBases > fd->bSize)
		{
		fprintf(stderr, "Error: requesting a sequence block too long...\n");
		exit(1);
		}

	fd->nBases = 0;
	while(fd->nBases < nBases)
		if((c = GetC(s)) == EOF)
			return EOF;

		else
			if(c != '\n')
				fd->seqBlock[fd->nBases++] = alphabet->symbols[c];

	return fd->nBases;
	}

/*----------------------------------------------------------------------------*/
//
// The alphabet struct should be all initialized with zeros. This allows
// non-existing elements of the alphabet to be mapped into the "0" symbol
//
unsigned AssignHeaderSymbols(HeaderAlphabet *alphabet)

    {
    unsigned n;

	alphabet->nSymbols = 0; // The decoder needs this initialization...
    for(n = 0 ; n < 256 ; n++)
        if(alphabet->usage[n])
			{
			alphabet->chars[alphabet->nSymbols] = n;
            alphabet->symbols[n] = alphabet->nSymbols++;
			}

    return alphabet->nSymbols;
    }

/*----------------------------------------------------------------------------*/

unsigned AssignSequenceSymbols(SequenceAlphabet *alphabet)

    {
    unsigned n, hasUppers = 0, hasLowers = 0;

	alphabet->nSymbols = 0; // The decoder needs this initialization...
	for(n = 0 ; n < 256 ; n++)
		if(alphabet->usage[n])
			{
			// A sequence can only have alphabetic chars
			// Well... let's relax this...
			//if(!isalpha(n))
			//	{
			//	fprintf(stderr, "Error: unexpected sequence char '%c'...\n", n);
			//	exit(1);
			//	}

			switch(n)
				{
				case 'A':
					alphabet->symbols['A'] = 0;
					hasUppers = 1;
					break;

				case 'C':
					alphabet->symbols['C'] = 1;
					hasUppers = 1;
					break;

				case 'G':
					alphabet->symbols['G'] = 2;
					hasUppers = 1;
					break;

				case 'T':
					alphabet->symbols['T'] = 3;
					hasUppers = 1;
					break;

				case 'a':
					alphabet->symbols['a'] = 4;
					hasLowers = 1;
					break;

				case 'c':
					alphabet->symbols['c'] = 5;
					hasLowers = 1;
					break;

				case 'g':
					alphabet->symbols['g'] = 6;
					hasLowers = 1;
					break;

				case 't':
					alphabet->symbols['t'] = 7;
					hasLowers = 1;
					break;

				default:
					// If the char is not in "ACGTacgt", then it is considered
					// an "extra" char. Symbol '0' is reserved for "non-extra"
					alphabet->nXchrSymbols++;
					alphabet->symbols[n] = (alphabet->nXchrSymbols + 1) << 2;
					alphabet->chars[(alphabet->nXchrSymbols + 1) << 2] = n;
					alphabet->xchrSymbols[n] = alphabet->nXchrSymbols;
					alphabet->xchrChars[alphabet->nXchrSymbols] = n;
					break;

				}

			// Number of different sequence chars
			alphabet->nSymbols++;
			}

	if(alphabet->nXchrSymbols)
		// Number of extra symbols + 1
		alphabet->nXchrSymbols++;

	if(hasUppers && hasLowers)
		{
		alphabet->caseType = CASE_BOTH;
		alphabet->chars[0] = alphabet->xchrChars[0] = 'A';
		alphabet->chars[1] = 'C';
		alphabet->chars[2] = 'G';
		alphabet->chars[3] = 'T';
		alphabet->chars[4] = 'a';
		alphabet->chars[5] = 'c';
		alphabet->chars[6] = 'g';
		alphabet->chars[7] = 't';
		}

	else
		if(hasUppers)
			{
			alphabet->caseType = CASE_UPPER;
			alphabet->chars[0] = alphabet->xchrChars[0] = 'A';
			alphabet->chars[1] = 'C';
			alphabet->chars[2] = 'G';
			alphabet->chars[3] = 'T';
			}

		else
			{
			alphabet->caseType = CASE_LOWER;
			alphabet->chars[0] = alphabet->xchrChars[0] = 'a';
			alphabet->chars[1] = 'c';
			alphabet->chars[2] = 'g';
			alphabet->chars[3] = 't';
			}

    return alphabet->nSymbols;
    }

/*----------------------------------------------------------------------------*/

void PrintAlphabet(unsigned *usage)

    {
    unsigned n;

    for(n = 0 ; n < 256 ; n++)
        if(usage[n])
            {
            if(isprint(n))
                putchar(n);

            else
                if(n == '\n')
                    printf("\\n");

            }

    putchar('\n');
    }

/*----------------------------------------------------------------------------*/

Base Complement(Base base)

	{
	switch(base)
		{
		case 'a':
		case 'A':
			return 'T';

		case 'c':
		case 'C':
			return 'G';

		case 'g':
		case 'G':
			return 'C';

		case 't':
		case 'T':
			return 'A';

		default:
			return 'T';

		}

	}

/*----------------------------------------------------------------------------*/

void ShiftBuffer(Symbol *buf, int bufSize, Symbol newSymbol)

	{
	memmove(buf, buf + 1, bufSize * sizeof(Symbol));
	buf[bufSize - 1] = newSymbol;
	}

