# Bi296_Project

Version 1.0.0
Lasted edit time: 2021.5.31

Project for bi296 course
This is branch for dev software.
Please fix your bug in here.


## Install 

  Auto Install
  
  ./INSTALL

  Manual Install

  * make
  * make install
  * make clean
  * cd bin
  * ./main [Option] [Parameter] [Filename] ...

## Synopsis

  ./main [Option] [Parameter] [Filename] ...


[OPTION]

  -h, --help         Obtain help from our doc

  -c, --compress     Compress mode

  -d, --decompress   Decompress mode
   
  -t, --transfer     Transfer file into fasta format (FILENAME.fq --> FILENAME.fq.transfer)
  
  -e, --extract      Extract sequences from standard fasta or transfered file (FILENAME.transfer)
        
  -f, --MFC          Compress .fa or .transfer file via MFC algorithm. The output file will be notes as suffix .mfc
  
  -m, --huffman      Compress .fa or .transfer file via huffman algorithm.
   
  -o, --optimize     Compare multipy algorithm to compress your file. 
                     Proffer a rank of compression rate and compression time.

[Parameter]

  > MFC algorithm

  -p, --parallel    Choose the parallel progression of compression. Default progression is 2 (if parameter is not used).

  -s, --speed       Choose the compression speed (Default speed: 2). 

			Speed 1: The fastest mode. Low compression rate and less memory cost.

			Speed 2: The balanced mode. Medium compression rate and medium memory cost.

			Speed 3: The slowest mode. High compression rate and high memory cost.
[Examples]

  ./main -c -s 3 -p 10 -f test.fa
  Utilized MFC algorithm to compress test.fa With speed 3 and parallel progression 10.
  Combinated parameters input style is also allowed. See: ./main -cs 3 -p 10 -f test.fa

  ./main -d -f -p 10 test.fa.mfc
  Utilized MFC algorithm to decompress test.fa.mfc With parallel progression 10.
