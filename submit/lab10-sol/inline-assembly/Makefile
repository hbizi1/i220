CFLAGS = -Wall -Og -std=c2x

TARGETS = \
  rip rip.s rip.dump \
  rsp rsp.s rsp.dump

all:		$(TARGETS)

# $(BASE): 	$(BASE).c

%.s:		%.c
		$(CC) $(CFLAGS) -S $<

%.dump:		%
		objdump -d $< > $@


.PHONY:		clean
clean:
		rm -f *~ *.o $(TARGETS)


