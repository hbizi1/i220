CFLAGS = -g -Wall -std=c2x

# this is the standard make variable to specify flags needed by the linker.
# in particular, the -L DIR telling the linker to search DIR for libraries.
LDFLAGS =

# this is the standard make variable to specify needed libraries using -l
LDLIBS = 

multiple-symbols:	def1.o def2.o main.o
			$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

.PHONY:			clean
clean:
			rm -f *.o multiple-symbols *~


