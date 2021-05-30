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

#ifndef DEFS_H_INCLUDED
#define DEFS_H_INCLUDED

typedef uint8_t Char;
typedef uint8_t Base;
typedef uint8_t Symbol;

#define CODED_FILE_SIGNATURE			17101997
#define MAX_PARTS						512
#define MIN_AUTO_PART_SIZE				250000000 // 250 M

#define STORAGE_BITS_N_PARTS			32
#define STORAGE_BITS_RECORD				32
#define STORAGE_BITS_PART_SIZE			40
#define STORAGE_BITS_N_RECORDS			32
#define STORAGE_BITS_N_SYMBOLS			8
#define STORAGE_BITS_SYMBOL				8
#define STORAGE_BITS_N_SIZE_BITS		6
#define STORAGE_BITS_LONGEST_HEADER		32

#ifdef WINDOWS
#define DEV_NULL "nul"
#else
#define DEV_NULL "/dev/null"
#endif

#define COPYRIGHT_NOTICE \
"Copyright 2013 IEETA/DETI/University of Aveiro, Portugal.\n\
All Rights Reserved.\n\n\
These programs are supplied free of charge for research purposes only,\n\
and may not be sold or incorporated into any commercial product. There is\n\
ABSOLUTELY NO WARRANTY of any sort, nor any undertaking that they are\n\
fit for ANY PURPOSE WHATSOEVER. Use them at your own risk. If you do\n\
happen to find a bug, or have modifications to suggest, please report\n\
the same to Armando J. Pinho, ap@ua.pt. The copyright notice above\n\
and this statement of conditions must remain an integral part of each\n\
and every copy made of these files.\n\n"

#endif /* DEFS_H_INCLUDED */

