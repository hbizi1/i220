CFLAGS = -g -Wall  -std=c2x

all:	hello hello.s

hello:

hello.s:	hello.c
		$(CC) $(CFLAGS) -S $<

clean:
		rm -f hello hello.s *.objdump *~
