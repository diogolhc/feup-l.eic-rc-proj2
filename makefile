all: download

CC=gcc
CFLAGS=-Wall
CFLAGS_SANITAZE=-Wall -g -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer 

download: src/download.c src/parser.c src/parser.h src/defines.h
	$(CC) $(CFLAGS) -o download src/download.c src/parser.c

downloadd: src/download.c src/parser.c src/parser.h src/defines.h
	$(CC) $(CFLAGS_SANITAZE) -o download src/download.c src/parser.c

clean:
	rm -rf download downloadd
