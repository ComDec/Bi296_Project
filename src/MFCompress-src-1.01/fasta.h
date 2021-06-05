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

#ifndef FASTA_H_INCLUDED
#define FASTA_H_INCLUDED

#define LEFT_GUARD		20

#define SIDE_INFO		0
#define XCHR_INFO		1
#define CASE_INFO		2

#define CASE_UPPER		0
#define CASE_LOWER		1
#define CASE_BOTH		2

typedef struct
	{
	unsigned	headerSize;		// Size of the header
	uint64_t	seqSize;		// Size of the sequence
	uint64_t	seqLineSize;	// Line size
	off_t		start;			// Start position in file given by ftello
	}
FastaRecord;

typedef struct
	{
	off_t		fileSize;
	unsigned	nParts;
	unsigned	recordRange[MAX_PARTS][2];
	off_t		partSize[MAX_PARTS];
	unsigned	nRecords;
	FastaRecord	*records;
	unsigned	longestHeader;
    uint64_t	longestSequence;
	unsigned	nSeqSizeBits;
    uint64_t	longestSeqLine;
	unsigned	nSeqLineSizeBits;
	}
FastaRecords;

typedef struct
	{
	Char		*header[2];
	Symbol		*seqBlock;
	unsigned	nBases;
	uint64_t	seqSize;
	unsigned	longestHeader;
	unsigned	bSize;
	}
FastaData;

typedef struct
	{
	unsigned	nSymbols;
	Symbol		symbols[256];	// char -> symbol
	Char        chars[256];		// symbol -> char
	unsigned	usage[256];		// number of times that a char appears
	}
HeaderAlphabet;

typedef struct
	{
	unsigned	nSymbols;		 // total number of different sequence chars
	Symbol		symbols[256];	 // char -> symbol
	Char        chars[256];		 // symbol -> char
	unsigned	nXchrSymbols;	 // number of "extra" chars
	Symbol		xchrSymbols[256];// char -> symbol
	Char        xchrChars[256];	 // symbol -> char
	unsigned	usage[256];		 // number of times that a char appears
	unsigned	caseType;
	}
SequenceAlphabet;

/*----------------------------------------------------------------------------*/

FastaRecords *CreateFastaRecords(void);
void NewFastaRecord(FastaRecords *frs);
FastaData *CreateFastaData(unsigned longestHeader, unsigned bSize);
void ResetFastaDataBuffers(FastaData *fd);
FastaData *GetHeader(Stream *s, FastaData *fd);
int GetSeqBlock(Stream *s, FastaData *fd, SequenceAlphabet *alphabet,
  unsigned nBases);
unsigned AssignHeaderSymbols(HeaderAlphabet *alphabet);
unsigned AssignSequenceSymbols(SequenceAlphabet *alphabet);
void PrintAlphabet(unsigned *usage);
Base Complement(Base base);
void ShiftBuffer(Symbol *buf, int bufSize, Symbol newSymbol);

#endif /* FASTA_H_INCLUDED */

