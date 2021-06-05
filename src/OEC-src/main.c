/* Optimal Eight Codens -- Wang Zhongyi*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define RDFILELEN 999

int codens[64] = {0};
int ocodens[8] = {0};
struct OEC{
	char bascd[4];
}OECodens[8];
struct Base{
	char bas[2];
}base[4];

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
	FILE *fp1 = NULL, *fp2 = NULL, *fp3 = NULL;
	char binfile[100], dicfile[100], *line = NULL, cod[4], c;
	int i, j, length, index;
	size_t len = 0; 
	ssize_t read; // The parameters len and read are used in function getline.

	strcpy(binfile, iptfile);
	strcpy(dicfile, iptfile);
	strcat(binfile, "_obin");
	strcat(dicfile, "_dic");

	fp1 = fopen(iptfile, "r");
	fp2 = fopen(binfile, "w+");
	fp3 = fopen(dicfile, "w+");
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
					case 1:
						fputs("1000", fp2);
						break;
					case 2:
						fputs("1001", fp2);
						break;
					case 3:
						fputs("1010", fp2);
						break;
					case 4:
						fputs("1011", fp2);
						break;
					case 5:
						fputs("1100", fp2);
						break;
					case 6:
						fputs("1101", fp2);
						break;
					case 7:
						fputs("1110", fp2);
						break;
					case 8:
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
		fputs("\n", fp2);
	}
	
	// Write the dicfile
	char cods[4];
	cods[3] = '\0';
	int k, d;
	for(i=0; i<8; ++i){
		d = ocodens[i];
		for(j=0; j<3; ++j){
			k = d%4;
			d /= 4;
			switch(k){
				case 0:
					cods[2-j]='A';
					break;
				case 1:
					cods[2-j]='G';
					break;
				case 2:
					cods[2-j]='C';
					break;
				case 3:
					cods[2-j]='T';
					break;
			}
		}
		fputs(cods, fp3);
		fputs("\n", fp3);
	}
	
	fclose(fp1);
	fclose(fp2);
	fclose(fp3);
	if(line)
		free(line);
	return 0;
}

int decode(char* dbinfile, char* dicfile)
{
	FILE *fp1 = NULL, *fp2 = NULL, *fp3 = NULL;
	char optfile[100], *line = NULL, c, bit3[5], bit2[4], buff[4];
	int i, j, d, length, cods[8] = {0};
	size_t len = 0;
	ssize_t read;

	bit3[4] = '\0'; 
	bit2[3] = '\0';
	buff[3] = '\0';

	strcpy(optfile, dbinfile);
	strcat(optfile, ".OECd");		// The name of optfile is "*.d"

	fp1 = fopen(dbinfile, "r");
	if(fp1 == NULL){
		printf("Error opening file %s.\n", dbinfile);
		return -1;	
	}
	fp2 = fopen(dicfile, "r");
	if(fp2 == NULL){
		printf("Error opening file %s.\n", dicfile);
		return -2;	
	}
	fp3 = fopen(optfile, "w+");

	// Record the Optimal Eight Codens with array cods[8].
	i = 0;
	while(fscanf(fp2, "%s", buff)){ // The break statement is essential!
		if(i>=8){
			break;
		}
		strcpy(OECodens[i].bascd, buff);
		++i;
	}

	while((read = getline(&line, &len, fp1)) != -1){
		length = strlen(line);
		i = 0;
		while(i<length){
			if((c=line[i]) == '1'){ // read a coden
				++i;
				d = 0;
				for(j=0; j<3; ++j){
					if(line[i+j]=='0')
						d = d*2;
					else
						d = d*2 + 1;
				}
				i+=3;
				fputs(OECodens[d].bascd, fp3);
			}else{ // read a base
				++i;
				d = 0;
				for(j=0; j<2; ++j){
					if(line[i+j]=='0')
						d = d*2;
					else
						d = d*2 + 1;
				}
				i+=2;
				fputs(base[d].bas, fp3);
			}
		}
		printf("\n");
	}

	fclose(fp1);
	fclose(fp2);
	fclose(fp3);
	if(line)
		free(line);
	return 0;
}

int main(int argc, char* argv[])
{
	int i;
	char *filename = NULL;
	char *binfile = NULL;
	char *dbinfile = NULL;
	char *dicfile = NULL;

	// store bases 'A', 'G', 'C', 'T'
	strcpy(base[0].bas, "A");
	strcpy(base[1].bas, "G");
	strcpy(base[2].bas, "C");
	strcpy(base[3].bas, "T");

	if(argc!=3 && argc!=5){
		printf("Usage:\n\tCompression: ./main -e filename\n\tDecompression: ./main -b binfile -d dicfile\n");
		return -1;
	}
	if(argc==3){
		filename = argv[2];
		OECcount(filename);
/*		for(i=0; i<64; ++i)
			printf("%d ", codens[i]);
		printf("\n");  */
		OECselct();
/*		for(i=0; i<8; ++i)
			printf("%d ", ocodens[i]);
		printf("\n");  */
		encode(filename);
	}
	else{
		dbinfile = argv[2];
		dicfile = argv[4];
		decode(dbinfile, dicfile);
/*		for(i=0; i<8; ++i)
			printf("%s ", OECodens[i].bascd);
		printf("\n");  */
	}
	
	return 0;
}









