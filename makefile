all: download downloadd

CC=gcc
CFLAGS=-Wall
CFLAGS_SANITAZE=-Wall -g -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer 

download: src/*.c src/*.h
	$(CC) $(CFLAGS) -o download src/*.c -lm

downloadd: src/*.c src/*.h
	$(CC) $(CFLAGS_SANITAZE) -o downloadd src/*.c -lm

clean:
	rm -rf download downloadd
