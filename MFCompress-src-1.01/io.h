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

#define IO_BUF_SIZE 65536

typedef struct
	{
	uint8_t buf[IO_BUF_SIZE];
	int		ptr;
	int		size;
	char	mode;
	off_t	tell;
	FILE	*fp;
	}
Stream;


Stream *CreateStream(void);
void InitStream(Stream *s, FILE* fp, char *m);
void PutC(int c, Stream *s);
int GetC(Stream *s);
char *GetS(char *str, int size, Stream *s);
void FlushStream(Stream *s);
off_t Tell(Stream *s);
int SeekSet(Stream *s, off_t offset);

