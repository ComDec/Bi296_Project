# DNACom

* A Linux software for comprehensive DNA sequences compression

Version 1.1.0
Lasted edit time: 2021.6.2

Project for bi296 course

Dongchang Road 800, Minhang district, Shanghai, China.

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
        
  -f, --MFC          Compress .fa via MFC algorithm. The output file will be notes as suffix .mfc
  
  -m, --huffman      Compress .fa.ex or file containing only DNA sequences via huffman algorithm.
   
  -o, --optimize     Compare multipy algorithm to compress your file. 
                     Proffer a rank of compression rate and compression time.

[Parameter]

  > MFC algorithm

  -p, --parallel    Choose the parallel progression of compression. Default progression is 2 (if parameter is not used).

  -s, --speed       Choose the compression speed (Default speed: 2). 

			Speed 1: The fastest mode. Low compression rate and less memory cost.

			Speed 2: The balanced mode. Medium compression rate and medium memory cost.

			Speed 3: The slowest mode. High compression rate and high memory cost.

  > Huffman algorithm

  [Encodefile]	   Compressing [Encodefile] with two output files: [Encodefile_bin] and [Encodefile_table].

  [Decodefile]	   Decompressing [Decodefile]. To simplify users input, users are allowed to input the filename 
	  	   of [Encodefile_bin] or [Encodefile_table] only.

[Examples]

  `./main -c -s 3 -p 10 -f test.fa`
  Utilized MFC algorithm to compress test.fa With speed 3 and parallel progression 10.
  Combinated parameters input style is also allowed. See: `./main -cs 3 -p 10 -f test.fa`

  `./main -d -f -p 10 test.fa.mfc`
  Utilized MFC algorithm to decompress test.fa.mfc With parallel progression 10.

  `./main -c -m test.fa.ex`
  Compressing [test.fa.ex] (plain DNA sequences file) with two modes.

  `./main -d -m test.fa.ex`
  Decompressing compressed file [test.fa.ex] as output file [test.fa.ex.hufde]. 

