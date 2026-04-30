COURSE = cs220


TARGETS = libio.so
TESTS = test-active-mem

CC = gcc
CPPFLAGS = -I $(HOME)/$(COURSE)/include

# -std=c2x causes problems on remote.cs
CFLAGS = -g -O2 -Wall -fPIC -std=c18

LIB_DIR = $$HOME/$(COURSE)/lib

LDFLAGS = -L$(LIB_DIR)
LIBS = -lZydis -l$(COURSE)

OFILES = \
  do-exercise.o \
  active-mem.o \
  ex-busy-data.o \
  ex-connect-data.o \
  ex-kbd-intr.o \
  ex-scan-keys.o

.PHONY:			clean all



all:			$(TARGETS) $(TESTS)

libio.so:		$(OFILES)
			$(CC) $(LDFLAGS) $^ $(LIBS) -shared \
			  -Wl,-rpath=$(LIB_DIR) -o $@

do-exercise:		$(OFILES)
			$(CC) $(LDFLAGS) $^ $(LIBS) -Wl,-rpath=$(LIB_DIR) -o $@

test-active-mem:	active-mem.c f.o active-mem.h 
			$(CC) $(LDFLAGS) $(CFLAGS) -D TEST_ACTIVE_MEM $< \
			   f.o $(LIBS) -o $@


active-mem.o:		active-mem.c active-mem.h

ex-%.o:			%.h exercises.h

clean:
			rm -f $(TARGETS) $(TEST_FILES) *.o *.dump *~
