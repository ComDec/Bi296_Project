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

// Data structures for handling finite-context models.

#ifndef FCM_H_INCLUDED
#define FCM_H_INCLUDED

//
// Header model counters can be only of 16 or 8 bits
//
// -c modelIdx alphaNum/alphaDen maxCount
//
#define HEADER_CTX_1	"-c 3 1/50 2047"
#define HEADER_CTX_2	"-c 4 1/50 2047"

//
// Sequence model counters use:
//   16 bits if model order <= 9
//    8 bits if model order >= 10 and model order <= 13
//    4 bits if model order == 14 or model order == 15
//    2 bits if model order == 16
//
// Non-winning models or updated only if model order >= 10
//
// For avoiding overflow, the alpha values (n/d) should obey
//   (4 * maxCount * d + 4 * n) < (4 * 65536)
//
// For model counters of 4 bits, maxCount is fixed and equal to 15
//
// For model counters of 2 bits, maxCount is fixed and equal to 3
//
// -c start:end alphaNum/alphaDen maxCount update updateIC
//
// -bs blockSize
//
#define SEQUENCE_CTX_1	\
  "-c 1:6 1/1 16383 0 0 "	\
  "-c 1:14 1/50 15 1 1 "	\
  "-bs 150"
#define SEQUENCE_CTX_2	\
  "-c 1:3 1/1 65535 0 0 "	\
  "-c 1:8 1/1 2047 0 0 "	\
  "-c 1:14 1/50 15 1 1 "	\
  "-bs 75"
#define SEQUENCE_CTX_3	\
  "-c 1:3 1/1 65535 0 0 "	\
  "-c 1:7 1/1 1023 0 0 "	\
  "-c 1:11 1/5 255 0 1 "	\
  "-c 1:15 1/50 15 1 1 "	\
  "-c 1:13 1/500 0 1 0 "	\
  "-bs 75"

//
// -cs sideInfoOrder maxCount -cx xchrInfoOrder maxCount -cc caseInfoOrder
//
#define SIDE_INFO_CTX	"-cs 5 16383 -cx 7 16383 -cc 12"

#define LOG_TABLE_SIZE (4*65536)

#define COUNTERS_16		0 // 16-bit counters
#define COUNTERS_08		1 //  8-bit counters
#define COUNTERS_04		2 //  4-bit counters
#define COUNTERS_02		3 //  2-bit counters

typedef uint16_t	Counter16; // Context counters for small arrays
typedef uint8_t		Counter08; // Context counters for large arrays
typedef uint8_t 	Counter04; // Context counters for large arrays
typedef uint8_t		Counter02; // Context counters for very large arrays

typedef struct
	{
	Counter16	*counters16;
	Counter08	*counters08;
	Counter04	*counters04;
	Counter02	*counters02;
	unsigned	type;
	}
Array;

typedef struct
	{
	unsigned	nSymbols;
	uint32_t	*cum;
	}
PModel;

typedef struct
	{
	Array	 array;
	unsigned *multipliers;		// idx multipliers
	unsigned ctxSize;			// Depth of context template
	unsigned nSymbols;			// Number of coding symbols
	uint32_t pModelIdx;			// Index of the previous pModel
	uint32_t pModelIdxAux;		// Aux pModel index
	uint32_t kMinusOneMask;		// e.g. ...0001111111111, if ctxSize = 6
	uint32_t kMinusOneShift;	// 2 * (ctxSize - 1)
	unsigned alphaNum;			// Numerator of alpha
	unsigned alphaDen;			// Denominator of alpha
	unsigned maxCount;			// Divide by 2 if maxCount is attained
	double	 totalNats;
	uint32_t nPModels;			// Total number of probability models
	unsigned update;			// If true, update even if model didn't win
	unsigned updateIC;			// If true, update inverted complements
	unsigned validPModelIdx;
	unsigned validPModelIdxAux;
	}
CModel;

typedef struct
	{
	int		size;
	int		*position;
	int		deepestPosition;
	}
CTemplate;

typedef struct
	{
	int		row;
	int		col;
	}
Coords;

typedef struct
	{
	int		id;
	int		size;
	Coords	*position;
	}
CTemplate2D;

void FillLogTable(void);
PModel *CreatePModel(unsigned nSymbols);
void UpdateCModelCounter(CModel *cModel, uint32_t pModelIdx, unsigned symbol);
void UpdateCModelCounter2(CModel *cModel, uint32_t pModelIdx, unsigned symbol);
void UpdateCModelCounter4(CModel *cModel, uint32_t pModelIdx, unsigned symbol);
CModel *CreateCModel(unsigned ctxSize, unsigned nSymbols, unsigned alphaNum,
  unsigned alphaDen, unsigned maxCount);
CModel *CreateCModel2(unsigned ctxSize);
void ComputePModel(CModel *cModel, PModel *pModel);
void ComputePModel2(CModel *cModel, PModel *pModel);
void ComputePModel4(CModel *cModel, PModel *pModel);
double PModelSymbolNatsLog(PModel *pModel, unsigned symbol);
double PModelSymbolNatsTable(PModel *pModel, unsigned symbol);
inline void GetPModelIdx2(Symbol *symbolPtr, CModel *cModel);
inline void GetPModelIdx4(Symbol *symbolPtr, CModel *cModel,
  CTemplate *cTemplate);
inline void GetPModelIdxAux4(Symbol *symbolPtr, CModel *cModel,
  CTemplate *cTemplate);
inline void GetInfoPModelIdx(Symbol *symbolPtr, CModel *cModel);
unsigned GetPModelIdx2D(Symbol **charPtr, int col, CTemplate2D *cTemplate,
  HeaderAlphabet *alphabet, CModel *cModel);
void IncreaseTemplateStorage(CTemplate *cTemplate, int additionalSize);
CTemplate2D CreateCTemplate2D(int id);
int StrToArgv(char *str, char ***argv);
void ParseListOfPositions(char *str, CTemplate *cTemplate);
void InitDataStructs(HeaderAlphabet *hAlphabet, SequenceAlphabet *bAlphabet,
  char *hCtx, char *bCtx, char *iCtx, CTemplate2D *hCTemplate, CModel **hCModel,
  PModel **hPModel, unsigned *bNCModels, CTemplate **bCTemplates,
  CModel ***bCModels, PModel **bPModel, CTemplate **auxCTemplates,
  CModel ***auxCModels, PModel ***auxPModels, Symbol ***auxBuffers,
  unsigned *bSize);

#endif // FCM_H_INCLUDED

