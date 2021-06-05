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

#ifndef ARITH_AUX_H_INCLUDED
#define ARITH_AUX_H_INCLUDED

unsigned GetSymbol(int *low, int *high, int *cum, int target, unsigned nSymbols);
void WriteNBits(uint64_t bits, int nBits, Stream *sOut);
uint64_t ReadNBits(int nBits, Stream *sIn);
void ArithEncodeSymbol(unsigned nSymbols, unsigned symbol, int *cum,Stream *sOut);
unsigned ArithDecodeSymbol(unsigned nSymbols, int *cum, Stream *sIn);

#endif /* ARITH_AUX_H_INCLUDED */

