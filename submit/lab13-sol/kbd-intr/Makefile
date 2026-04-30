COURSE = cs220

TARGET = kbd-intr

CPPFLAGS = -I ../io-lib
CC = gcc
CFLAGS = -g -O2 -Wall -std=c2x

LIB_DIR = $$HOME/$(COURSE)/lib 
LDFLAGS = -L$(LIB_DIR) -L../io-lib
LIBS = -lZydis -l$(COURSE) -lio

.PHONY:			clean all

all:			$(TARGET)

kbd-intr:		kbd-intr.o
			$(CC) $(LDFLAGS) $^ $(LIBS) \
			  -Wl,-rpath=$(LIB_DIR) \
                          -Wl,-rpath='$$ORIGIN/../io-lib' -o $@

clean:
			rm -f $(TARGET) $(TEST_FILES) *.o *.dump *~
