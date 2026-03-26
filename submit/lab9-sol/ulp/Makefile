CFLAGS = -g -Wall -std=c2x
LDLIBS = -lm

TARGETS = ulp float-ulp indistinguishable

all: 		$(TARGETS)

float-ulp:  	ulp.c
		$(CC) $(CFLAGS) -D USE_FLOAT $< $(LDLIBS) -o $@

clean:
		rm -f $(TARGETS) *~ *.o


