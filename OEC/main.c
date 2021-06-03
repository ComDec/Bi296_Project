#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define RDFILELEN 999

int codens[64] = {0};
int ocodens[8] = {0};
struct OEC{
	int freqcy;
	int index;
	char bincd[5];
}OECodens[8];

// Convert a base-char to a digit. 
int ctod(char c, int d)
{
	if(c>='a' && c<='z'){
		c = c + 'A' - 'a';
	}
	switch(c){
		case 'A':
			d = d*4 + 0;
			break;
		case 'G':
			d = d*4 + 1;
			break;
		case 'C':
			d = d*4 + 2;
			break;
		case 'T':
			d = d*4 + 3;
			break;
	}
	return d;
}

// This function will count all the codens in any possible orders.
void OECcount(char* filename)
{
	FILE* fp = NULL;
	char buff[RDFILELEN+1];
	int i, j, digit, len;
	char c;

	fp = fopen(filename, "r");
	// printf("Yes\n");
	fgets(buff, RDFILELEN+1, fp);
	len = strlen(buff);
	for(i=0; i<=len-2; ++i){
		digit = 0;
		for(j=0; j<3; ++j){
			c = buff[i+j];
			digit = ctod(c, digit);
		}
		codens[digit]++;
	}
	fclose(fp);		
}

// This function will select the Optimal Eight Codens in all 64 codens.
void OECselct()
{
	int tmp=0, i, j, k;
	
	for(i=0; i<64; ++i){
		if(codens[i]>tmp){
			tmp = codens[i];
			ocodens[0] = i;
		}
	}
	codens[ocodens[0]] = 0;
	for(i=1; i<8; ++i){
		tmp = 0;
		for(j=0; j<64; ++j){
			if(codens[j]>tmp){
				tmp = codens[j];
				ocodens[i] = j;
			}		
		}
		codens[ocodens[i]]=0;
	}
}

int ifOEC(char* cod)
{
	int i, d=0;

	for(i=0; i<3; ++i){
		d = ctod(cod[i], d);
	}
	for(i=0; i<8; ++i){
		if(ocodens[i]==d){
			return i+1;
		}
	}

	return 0;
}

// This function will encode the DNA sequences in input file to binary sequence and generate a binfile. 
int encode(char* iptfile)
{
	FILE *fp1 = NULL, *fp2 = NULL;
	char binfile[30], *line = NULL, cod[4], c;
	int i, length, index;
	size_t len = 0; 
	ssize_t read; // The parameters len and read are used in function getline.

	strcpy(binfile, iptfile);
	strcat(binfile, "_bin");

	fp1 = fopen(iptfile, "r");
	fp2 = fopen(binfile, "a+");
	if(fp1 == NULL){
		printf("Error opening file %s.\n", iptfile);
		return -1;	
	}
	// Then read the lines of DNA sequences in iptfile.
	while((read = getline(&line, &len, fp1)) != -1){
		i = 0;
		length = strlen(line);
		while(i<length-2){
			strncpy(cod, line+i, 3);
			if(index = ifOEC(cod)){
				switch(index){
					case '1':
						fputs("1000", fp2);
						break;
					case '2':
						fputs("1001", fp2);
						break;
					case '3':
						fputs("1010", fp2);
						break;
					case '4':
						fputs("1011", fp2);
						break;
					case '5':
						fputs("1100", fp2);
						break;
					case '6':
						fputs("1101", fp2);
						break;
					case '7':
						fputs("1110", fp2);
						break;
					case '8':
						fputs("1111", fp2);
						break;
				}
				i+=3;
				continue;
			}
			// When cod isn't the OEC, we transfer one base to binary.
			c = line[i];
			switch(c){
				case 'A':
					fputs("000", fp2);
					break;
				case 'G':
					fputs("001", fp2);
					break;
				case 'C':
					fputs("010", fp2);
					break;
				case 'T':
					fputs("011", fp2);
					break;
			}
			++i;
		}
	}
	
	fclose(fp1);
	fclose(fp2);
	if(line)
		free(line);
	return 0;
}

int main(int argc, char* argv[])
{
	int i;
	char *filename = NULL;

	if(argc!=3 && argc!=5){
		printf("Usage:\n\tCompression: ./main -e filename\n\tDecompression: ./main -b binfile -d dicfile\n");
		return -1;
	}
	if(argv[1]){
		filename = argv[2];
		OECcount(filename);
//		for(i=0; i<64;++i){
//			printf("%d ", codens[i]);
//		printf("\n");
		OECselct();
//		for(i=0; i<8; ++i)
//			printf("%d ", ocodens[i]);
//		printf("\n");
		encode(filename);
	}
	
	return 0;
}









