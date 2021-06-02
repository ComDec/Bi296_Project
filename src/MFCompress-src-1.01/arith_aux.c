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
#include "io.h"
#include "arith.h"
#include "bitio.h"
#include "arith_aux.h"

/*----------------------------------------------------------------------------*/

inline unsigned GetSymbol2(int *low, int *high, int *cum, int target,
  unsigned nSymbols)

	{
	unsigned left = 0, right = nSymbols, mid;

	while(!(left > right))
		{
		mid = (left + right) >> 1;
		if(target >= cum[mid])
			left = ++mid;
		else
			right = --mid;

		}

	*high = cum[left];
	*low = cum[--left];
	return(left);
	}

/*----------------------------------------------------------------------------*/

inline unsigned GetSymbol(int *low, int *high, int *cum, int target,
  unsigned nSymbols)

	{
	unsigned n = 0;

	while(target >= cum[++n])
		;

	*high = cum[n];
	*low = cum[--n];
	return(n);
	}

/*----------------------------------------------------------------------------*/

void WriteNBits(uint64_t bits, int nBits, Stream *sOut)

	{
	while(nBits--)
		{
		if((bits >> nBits) & 0x1)
			arithmetic_encode(1, 2, 2, sOut);

		else
			arithmetic_encode(0, 1, 2, sOut);

		}

	}

/*----------------------------------------------------------------------------*/

uint64_t ReadNBits(int nBits, Stream *sIn)

	{
	uint64_t bits = 0;
	int target, low, high, cum[3] = {0, 1, 2};

	while(nBits--)
		{
		bits <<= 1;
		target = arithmetic_decode_target(2);
		bits |= GetSymbol(&low, &high, cum, target, 2);
		arithmetic_decode(low, high, 2, sIn);
		}

	return bits;
	}

/*----------------------------------------------------------------------------*/

inline void ArithEncodeSymbol(unsigned nSymbols, unsigned symbol,
  int *cum, Stream *sOut)

	{
	arithmetic_encode(cum[symbol], cum[symbol+1], cum[nSymbols], sOut);
	}

/*----------------------------------------------------------------------------*/

inline unsigned ArithDecodeSymbol(unsigned nSymbols, int *cum, Stream *sIn)

	{
	int low, high;
	unsigned symbol = GetSymbol(&low, &high, cum,
	  arithmetic_decode_target(cum[nSymbols]), nSymbols);

	arithmetic_decode(low, high, cum[nSymbols], sIn);
	return symbol;
	}

