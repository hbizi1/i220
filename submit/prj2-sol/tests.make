COURSE = cs220
COURSE_DIR = $$HOME/$(COURSE)

CC = gcc
CFLAGS = -g -Wall -I $(COURSE_DIR)/include
LDFLAGS = -L $(COURSE_DIR)/lib
LDLIBS = -l $(COURSE)

TESTS = \
  do-bcd-tests-0 do-bcd-tests-1 do-bcd-tests-2 do-bcd-tests-3 do-bcd-tests-4


all:			$(TESTS)

do-bcd-tests-%: 	bcd-tests-%
			./$<

bcd-tests-%:		bcd-tests-%.o bcd-%.o
			$(CC) $^ -o $@


bcd-tests-%.o:		bcd-tests.c
			$(CC) -c $(CFLAGS) -DBCD_BASE=$* $< -o $@

bcd-%.o:		bcd.c
			$(CC) -c $(CFLAGS) -DBCD_BASE=$* $< -o $@


.PHONY:		clean
clean:
		rm -f *~ bcd-tests-? *.o 
