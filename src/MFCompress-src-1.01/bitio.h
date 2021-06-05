/******************************************************************************
File:      bitio.h

Authors:   John Carpinelli   (johnfc@ecr.mu.oz.au)          
           Wayne Salamonsen  (wbs@mundil.cs.mu.oz.au)       
           Lang Stuiver      (langs@cs.mu.oz.au)

Purpose:   Data compression using a revised arithmetic coding method.
Based on:  A. Moffat, R. Neal, I.H. Witten, "Arithmetic Coding Revisted",
           Proc. IEEE Data Compression Conference, Snowbird, Utah, March 1995.

           Low-Precision Arithmetic Coding Implementation by Radford M. Neal

Copyright 1996 Lang Stuiver, All Rights Reserved.
Based on work by
Copyright 1995 John Carpinelli and Wayne Salamonsen, All Rights Reserved.

******************************************************************************

Code adapted by Armando J. Pinho
ap@ua.pt
University of Aveiro, DETI/IEETA, 3810-193 Aveiro, Portugal
December 1999

******************************************************************************
 
  Bit and byte input output functions.
  Input/Output to stdin/stdout 1 bit at a time.
  Also byte i/o and fread/fwrite, so can keep a count of bytes read/written
   
  Once bit functions are used for either the input or output stream,
  byte based functions are NOT safe to use, unless a
  flush_{input,output}stream is first called. (Since bits are buffered a
  char at a time, while bytes are output immediately)
 
******************************************************************************/

#ifndef BITIO_H
#define BITIO_H


#define BYTE_SIZE 8

/* As declared in bitio.c */
//extern unsigned int	_bytes_input, _bytes_output;
extern unsigned long long	_bytes_input, _bytes_output;

extern int 	_in_buffer;		// Input buffer
extern int	_in_bit_ptr;	// Input bit pointer
extern int	_in_garbage;	// # of bytes read past EOF

extern int	_out_buffer;	// Output buffer
extern int	_out_bits_to_go;// Output bits in buffer

#ifndef FAST_BITIO
extern int	_bitio_tmp;		// Used by i/o macros to keep function ret values
#endif


/*
 * OUTPUT_BIT(b)
 *
 * Outputs bit 'b'. (Builds up a buffer, writing a byte at a time.)
 *
 */
#define OUTPUT_BIT(b, s)				\
{										\
	_out_buffer <<= 1;					\
	if(b)								\
		_out_buffer |= 1;				\
	if(!--_out_bits_to_go)				\
		{								\
		OUTPUT_BYTE(_out_buffer, s);	\
		_out_bits_to_go = BYTE_SIZE;	\
		_out_buffer = 0;				\
		}								\
}

/* 
 * ADD_NEXT_INPUT_BIT(v, garbage_bits)
 * 
 * Returns a bit from stdin, by shifting 'v' left one bit, and adding
 * next bit as lsb (possibly reading upto garbage_bits extra bits beyond
 * valid input)
 * 
 * garbage_bits:  Number of bits (to nearest byte) past end of file to
 * be allowed to 'read' before printing an error message and halting.
 * This is needed by our arithmetic coding module when the FRUGAL_BITS
 * option is defined, as upto B_bits extra bits may be needed to keep
 * the code buffer full (although the actual bitvalue is not important)
 * at the end of decoding.
 * 
 * The buffer is not shifted, instead a bit flag (_in_bit_ptr) is moved
 * to point to the next bit that is to be read.  When it is zero, the
 * next byte is read, and it is reset to point to the msb.
 * 
 */
#define ADD_NEXT_INPUT_BIT(v, garbage_bits, s)				\
{															\
    if(!_in_bit_ptr)										\
		{													\
		_in_buffer = GetC(s);								\
		if(_in_buffer == EOF) 								\
			{												\
			_in_garbage++;									\
			if((_in_garbage - 1) * 8 >= garbage_bits)		\
				{											\
				fprintf(stderr, "Bad input file - attempted"\
				  " read past end of file.\n");				\
				exit(1);									\
				}											\
			}												\
		else												\
			{												\
			_bytes_input++;									\
			}												\
		_in_bit_ptr = (1<<(BYTE_SIZE-1));					\
		}													\
	v <<= 1;												\
	if(_in_buffer & _in_bit_ptr)							\
		v++;												\
	_in_bit_ptr >>= 1;										\
}


/*#define FAST_BITIO*/
/* Normally count input and output bytes so program can report stats
 * With FAST_BITIO set, no counting is maintained, which means file sizes
 * reported with the '-v' option will be meaningless, but should improve
 * speed slightly.
 */
#ifdef FAST_BITIO
#  define OUTPUT_BYTE(x, s)	PutC(x, s)
#  define INPUT_BYTE(s)		GetC(s)
#  define BITIO_FREAD(ptr, size, nitems, s)		fread(ptr, size, nitems, s)
#  define BITIO_FWRITE(ptr, size, nitems, s)	fwrite(ptr, size, nitems, s)
#else
#  define OUTPUT_BYTE(x, s)			\
	(_bytes_output++, PutC(x, s))

#  define INPUT_BYTE(s)				\
	(_bitio_tmp = GetC(s), 			\
	_bytes_input += (_bitio_tmp == EOF) ? 0 : 1, _bitio_tmp)

#  define BITIO_FREAD(ptr, size, nitems, s)			\
	(_bitio_tmp = fread(ptr, size, nitems, s),		\
	 _bytes_input += _bitio_tmp * size, _bitio_tmp)	// Return result of fread

#  define BITIO_FWRITE(ptr, size, nitems, s)		\
	(_bitio_tmp = fwrite(ptr, size, nitems, s),		\
	 _bytes_output += _bitio_tmp * size, _bitio_tmp)// Return result of fwrite
#endif

void startoutputtingbits(void);
void startinputtingbits(void);
void doneoutputtingbits(Stream *);
void doneinputtingbits(void);

#endif		/* ifndef bitio_h */

