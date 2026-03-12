CFLAGS = -g -Wall -std=c18

# this is the standard make variable to specify flags needed by the linker.
# in particular, the -L DIR telling the linker to search DIR for libraries.
LDFLAGS =

# this is the standard make variable to specify needed libraries using -l
LDLIBS = 

TARGETS = hello

all:		$(TARGETS)	

# link objects into hello executable

greet:		greet.o greet.h
		$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

main:		main.o greet.h
		$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

hello:		main.o greet.o
		$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

.PHONY:		clean
clean:
		rm -f *.o *~ $(TARGETS)

