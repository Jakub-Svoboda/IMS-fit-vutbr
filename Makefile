CC=g++
CFLAGS=-Wall -Wextra -pedantic -std=c++17 -lstdc++fs
DEPS=
FILES=main.c
OBJ=main.o

all: main.cpp	 
	$(CC) -Wall -pedantic -std=c++17 -pthread -o ims main.cpp -lstdc++fs

clean:
	rm -f output/*
	rm -f ims
	rm -f 07_xschmi08_xsvobo0z.zip

run: all
	rm -f output/*
	./ims -f data.csv -x 730 -y 730 -a 180 -V 10 -s 100 -d 1500

download:
	wget http://www.stud.fit.vutbr.cz/~xschmi08/data.csv

zip:
	rm -f output/*
	cp ./docs/dokumentace.pdf ./dokumentace.pdf && zip 07_xschmi08_xsvobo0z.zip Makefile main.cpp main.h output/ dokumentace.pdf README.md
