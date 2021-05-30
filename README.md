# Bi296_Project

Version 1.1
Lasted edit time: 2021.5.24

Project for bi296 course
This is branch for dev software.
Please fix your bug in here.


## Install 

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
   
  -t, --transfer     Transfer file into fasta format
  
  -e, --extract      Extract sequences from standard fasta or transfered file (FILENAME.transfer)
        
  -z, --7zip         Compress dcm file into 7z format. The 7z filename is required.

  
  -g, --gzip         Compress dcm file into gzip format. The gzip filename is required.
   

  -o, --optimize     Compare multipy algorithm to compress your file. 
                     Proffer a rank of compression rate and compression time.
  
