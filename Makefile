CC_C    := gcc
CC_CPP  := g++
EXE     := cache
SRC_C   := cache.c
SRC_CPP := cache.cpp
OBJ     := cache.o


.PHONY: all

all:
	g++ -c cache.cpp -o cache.o;
	g++ cache.o -o cache;


clean:
	@rm -rf ${EXE} *.o
