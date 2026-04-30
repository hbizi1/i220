CC = gcc
CFLAGS = -g -Wall
LIBS = -lcapstone

TARGET = capstone-play

capstone-play:		capstone-play.o fns.o
			$(CC) $(CFLAGS) $^ $(LIBS) -o $@

fns.o:			fns.s
			gcc -c $< -o $@

.PHONY:			clean
clean:
			rm -f $(TARGET) *.o *~ *.dump
