
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char inStr[100000], chSet[5]="ATCG"; 
float P[4]={0.1,0.2,0.3,0.4};                    
float pZone[5];             
int strLen;         
int chNum = 4;          
int binary[100000];
float infoLen;          
void compress(int len, char *inStr, char *outfile);        
void uncompress(int len);      
double log(double a);
int main(int argc, char **argv)
{
    int i,j;
    int len;
    char filename[200];
    char outfile[200]; 
    pZone[0] = 0;
    strcpy(filename,argv[1]);
    strcpy(outfile,argv[1]);
    strcat(outfile,".num");
    for (i=1;i<chNum;i++)
        pZone[i] =pZone[i-1]+P[i-1];
	FILE *fp=NULL;
    fp=fopen(filename,"r");
        fscanf(fp,"%s",inStr);
        len=strlen(inStr);
    //printf("%s\n",inStr);
        

        printf("*********** compress **************\n");
        compress(len, inStr, outfile);
        //printf("\n*********** uncompress **************\n");
        uncompress(len);

    return 0;

}


void compress(int len, char *inStr, char *outfile)
{
    float low=0,high=1;
    float L, H, zlen=1;
    float cp;       
    float result;       
    int i, j;
    FILE *fp = fopen(outfile,"w");

    for (i=0;i<len;i++)
    {
        for(j=0;j<chNum;j++)
        {
            if(inStr[i]==chSet[j])
            {
                cp=P[j];
                L=pZone[j];
                low=low+zlen*pZone[j];
                zlen*=P[j];
                break;
            }
        }
    }
    result = low;

    printf("the result is %f\n", result);

    infoLen = log(1/zlen) / log(2);    

    if(infoLen > (int)infoLen)
        infoLen = (int)infoLen + 1;
    else
        infoLen = (int)infoLen;

    for (i=0;i<infoLen; i++)
    {
        result*=2;

        if (result > 1) {
            result = result - 1;
            binary[i] = 1;

        } else if (result < 1) {
                binary[i] = 0;

        } else {
            break;
        }

    }

    if (i >= infoLen) {
        for (j=i; j >= 1; j--) {
            binary[j-1] = (binary[j-1]+1)%2;
            if (binary[j-1] == 1)
                break;
        }
    }

   // printf("****************** the binary compress result*****************\n");
    for (j=0; j < i; j++)
        fprintf(fp, "%d ", binary[j]);

    fclose(fp);
}

void uncompress(int len)
{
    int i,j;
    float w = 0.5;
    float deResult=0;

    float newLow,newLen;
    float low=0,zlen=1;


    /*************** binary to ten *******************/
    for (i=0;i<infoLen;i++) {
        deResult += w*binary[i];
        w *= 0.5;
    }
    printf("uncompress to ten:%f\n", deResult);

    printf("uncompress result:\n");
    for (i=0;i<len;i++) {
        for (j=chNum; j > 0; j--) {
            newLow = low;
            newLen = zlen;
                newLow += newLen * pZone[j-1];
            newLen *= P[j-1];
            if (deResult >= newLow) {
                low=newLow;
                zlen=newLen;
                printf("%c",chSet[j-1]);
                break;
            }
        }
    }

}
double log(double a)
{
   int N = 15;
   int k,nk;
   double x,xx,y;
   x = (a-1)/(a+1);
   xx = x*x;
   nk = 2*N+1;
   y = 1.0/nk;
   for(k=N;k>0;k--)
   {
     nk = nk - 2;
     y = 1.0/nk+xx*y;
     
   }
   return 2.0*x*y;
   
}
