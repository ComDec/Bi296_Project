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
#include "defs.h"
#include "mem.h"
#include "io.h"

/*----------------------------------------------------------------------------*/

Stream *CreateStream(void)

	{
	Stream *s;

	if(!(s = (Stream *)Calloc(1, sizeof(Stream))))
		{
		fprintf(stderr, "Error: out of memory\n");
        exit(1);
		}

	return s;
	}

/*----------------------------------------------------------------------------*/

void InitStream(Stream *s, FILE* fp, char *mode)

	{
	s->fp = fp;
	s->tell = 0;
	s->mode = *mode;
	if(*mode == 'r') // Open for reading
		{
		s->ptr = IO_BUF_SIZE;
		s->size = IO_BUF_SIZE;
		}

	else // Open for writing
		s->ptr = 0;

	}

/*----------------------------------------------------------------------------*/
//
// s->ptr points to the next free position
//
inline void PutC(int c, Stream *s)

	{
	s->buf[s->ptr] = c;
	s->tell++;

	if(++s->ptr == IO_BUF_SIZE) // buffer is full: write it
		{
		if(fwrite(s->buf, 1, IO_BUF_SIZE, s->fp) != IO_BUF_SIZE)
			{
			perror("fwrite");
			exit(1);
			}

		s->ptr = 0;
		}

	}

/*----------------------------------------------------------------------------*/
//
// s->ptr points to the next char
//
inline int GetC(Stream *s)

	{
	if(s->ptr == IO_BUF_SIZE) // buffer is empty: get another block
		{
		if(!(s->size = fread(s->buf, 1, IO_BUF_SIZE, s->fp)))
			return EOF;

		s->ptr = 0;
		}

	else
		{
		if(s->ptr == s->size)
			return EOF;

		}

	s->tell++;
	return s->buf[s->ptr++];
	}

/*----------------------------------------------------------------------------*/

char *GetS(char *str, int size, Stream *s)

	{
	int c, n = 0;

	while(n < size - 1 && (c = GetC(s)) != EOF)
		if((str[n++] = c) == '\n')
			break;

	str[n] = '\0';

	if(n)
		return str;

	else
		return NULL;

	}

/*----------------------------------------------------------------------------*/
//
// s->ptr points to a free position
//
void FlushStream(Stream *s)

	{
	if(s->ptr) // If buf is not empty
		{
		if(fwrite(s->buf, 1, s->ptr, s->fp) != s->ptr)
			{
			perror("fwrite");
			exit(1);
			}

		s->ptr = 0;
		}

	}

/*----------------------------------------------------------------------------*/

off_t Tell(Stream *s)

	{
	return s->tell;
	}

/*----------------------------------------------------------------------------*/

int SeekSet(Stream *s, off_t offset)

	{
	if(fseeko(s->fp, offset, SEEK_SET) == -1)
		{
		perror("fseek");
		exit(1);
		}

	s->tell = offset;
	if(s->mode == 'r') // Open for reading
		{
		s->ptr = IO_BUF_SIZE;
		s->size = IO_BUF_SIZE;
		}

	else // Open for writing
		s->ptr = 0;

	return 0;
	}

