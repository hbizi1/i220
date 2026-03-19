CFLAGS = -g -Wall -fPIC -std=c2x

OBJS = \
  rect.o\
  circ.o\

all:		libgeom.a libgeom.so testgeom-static testgeom-dynamic

libgeom.so:	$(OBJS)
		$(CC) -shared $(OBJS) -o $@

libgeom.a:	$(OBJS)
		ar rcs $@ $(OBJS)

testgeom-static: testgeom.o
		$(CC)  -static testgeom.o -L. -lgeom -o $@

testgeom-dynamic: testgeom.o
		$(CC) testgeom.o -L. -lgeom -o $@

.PHONY:		clean
clean:
		rm -f *.o *.so *.a testgeom-*
