# README
The algotithm is called Optimal Eight Codens.This algorithm is proposed and completed by Wang Zhongyi.

## Syntax
The main fuction will count the frequency of all possible codons in a certain length of DNA in the input file. Then the optimal eight codens will be selected. And these 8 codens with 4 bases will have binary codes to compress the DNA file into a binary file. And in turn, a binary file can be decompressed into a DNA file with the above code table.

## Usage
Compress: ./main -e Dnafile
	The format of the generated file is "Dnafile_bin"
Decompress: ./main -b binfile -d dicfile
	The format of the generated file is "binfile.d"
