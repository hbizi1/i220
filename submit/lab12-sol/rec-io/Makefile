CC = gcc
CFLAGS = -g -Wall -std=c2x
LDLIBS = 

TARGETS = read-points write-points xread-points

all:			$(TARGETS)

read-points:		read-points.c point2.h
write-points:		write-points.c point2.h

xread-points:		read-points.c point2.h
			$(CC) -m32 $(CFLAGS) $< -o $@

clean:
			rm -f $(TARGETS) *.o *~ points.dat points.text
