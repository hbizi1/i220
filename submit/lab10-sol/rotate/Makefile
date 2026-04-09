CFLAGS = -Wall -Og -std=c2x

BASE = rotate

TARGETS = \
  rotate \
  rotate.s

all:		$(TARGETS)

%.s:		%.c
		$(CC) -S $(CFLAGS) $<


.PHONY:		clean
clean:
		rm -f *~ *.o $(TARGETS)


