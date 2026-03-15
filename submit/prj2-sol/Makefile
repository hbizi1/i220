COURSE = cs220
COURSE_DIR = $$HOME/$(COURSE)

CC = gcc
CFLAGS = -g -Wall -I $(COURSE_DIR)/include
LDFLAGS = -L $(COURSE_DIR)/lib
LDLIBS = -l $(COURSE)

TARGETS = bcd-0 bcd-1 bcd-2 bcd-3 bcd-4 bcd


all:		$(TARGETS)


bcd:		bcd-2
		ln -s $< $@

bcd-%:		main-%.o bcd-%.o
		$(CC) $^ -o $@


main-%.o:	main.c
		$(CC) -c $(CFLAGS) -DBCD_BASE=$* $< -o $@

bcd-%.o:	bcd.c
		$(CC) -c $(CFLAGS) -DBCD_BASE=$* $< -o $@

.PHONY:		strip
strip:		$(TARGETS)
		strip $^

.PHONY:		clean
clean:
		rm -f $(TARGETS) *.o *~
