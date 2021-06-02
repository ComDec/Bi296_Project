############################
# Author: Xi Wang
# Version: 1.0.1
# Last edit time: 2021.4.11
# Functions: Complie the c program file and pack up as static library
############################
CC = gcc
PATH_LIB = ./lib
PATH_SRC = ./src
PATH_BIN = ./bin
PATH_HEAD = ./include
NAME_LIB = Project

all: $(PATH_SRC)/*.c 
	$(CC) -c ./src/*.c -I $(PATH_HEAD) 
	ar -rsv $(PATH_LIB)/lib$(NAME_LIB).a *.o 
.PHONY: install
install:
	$(CC) -o $(PATH_BIN)/DNACom $(PATH_BIN)/main.c -L $(PATH_LIB) -l$(NAME_LIB) -I $(PATH_HEAD) -g
.PHONY: clean
clean:
	rm -rf *.o
