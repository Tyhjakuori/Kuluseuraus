CC=gcc
CFLAGS=-std=c11 -Wextra -Wall
LIBS=-l sqlite3
RM=/bin/rm

all:main

main: main.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

clean:
	$(RM) main testi.db
