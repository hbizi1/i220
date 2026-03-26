CC = gcc
CFLAGS = -g -Wall -std=c2x
LDLIBS = -lm

all: 		inexact float-inexact

float-inexact:  inexact.c
		$(CC) $(CFLAGS) -D USE_FLOAT $< $(LDLIBS) -o $@


