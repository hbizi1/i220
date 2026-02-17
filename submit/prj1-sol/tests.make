COURSE = cs220
COURSE_INCLUDE_DIR = $$HOME/$(COURSE)/include

TARGETS =  do-tests8 do-tests16

CC = gcc
CFLAGS = -g -Wall -O1 -std=gnu2x -I $(COURSE_INCLUDE_DIR) -DDO_TESTS

C_SRCS =  tests.c  morse.c

SRCS = $(C_SRCS) morse.h inlines.h

all:		$(TARGETS)

do-tests8:	tests8
		./tests8

do-tests16:	tests16
		./tests16

tests8:		$(SRCS)
		$(CC) $(CFLAGS) $(C_SRCS) -o $@

tests16:	$(SRCS)
		$(CC) $(CFLAGS) -DBYTE_SIZE=2 \
                      $(C_SRCS) -o $@


clean:
		rm -f *~ tests8 tests16
