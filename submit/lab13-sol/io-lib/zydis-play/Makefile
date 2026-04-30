CC = gcc
CFLAGS = -g -Wall
LIBS = -lZydis

TARGET = zydis-play

zydis-play:		zydis-play.o fns.o
			$(CC) $(CFLAGS) $^ $(LIBS) -o $@

fns.o:			fns.s
			gcc -c $< -o $@

.PHONY:			clean
clean:
			rm -f $(TARGET) *.o *~ *.dump
