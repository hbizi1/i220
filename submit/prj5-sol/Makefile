COURSE = cs220

TARGET = cache-sim

CPPFLAGS = -I $(HOME)/$(COURSE)/include
CFLAGS = -g -Wall -std=gnu2x

LIBDIR = $$HOME/$(COURSE)/lib
LIB = cs220

LDFLAGS = -L $(LIBDIR)
LDLIBS = -l$(LIB)

OBJS = \
  cache-sim.o \
  main.o 

$(TARGET):	$(OBJS)
		$(CC) $(LDFLAGS) $(OBJS) $(LDLIBS) -Wl,-rpath=$(LIBDIR) -o $@


cache-sim.o:	cache-sim.c cache-sim.h

clean:		
		rm -f $(OBJS) $(TARGET) *~


