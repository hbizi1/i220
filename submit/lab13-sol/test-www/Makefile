COURSE = cs220

TARGET = test-www

CPPFLAGS = -I ../io-lib
CC = gcc
CFLAGS = -g -O2 -Wall -std=c2x

LIB_DIR = $$HOME/$(COURSE)/lib 
LDFLAGS = -L$(LIB_DIR) -L../io-lib
LIBS = -lZydis -l$(COURSE) -lio

.PHONY:			clean all

all:			$(TARGET)

$(TARGET):		$(TARGET).o
			$(CC) $(LDFLAGS) $^ $(LIBS) \
			  -Wl,-rpath=$(LIB_DIR) \
                          -Wl,-rpath='$$ORIGIN/../io-lib' -o $@

clean:
			rm -f $(TARGETS) $(TEST_FILES) *.o *.dump *~
