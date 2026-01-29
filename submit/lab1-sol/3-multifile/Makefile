PROJECT = quadr
SRC_FILES =  main.c \
		quadr.c \
		quadr.h \
		README \
		test.data \
		Makefile

CFLAGS = -g -Wall -std=c18
LDFLAGS = -lm

$(PROJECT):	quadr.o main.o
		$(CC)  quadr.o main.o $(LDFLAGS) -o $@

clean:		rm -f *.o $(PROJECT) $(PROJECT).tar.gz

dist:		$(PROJECT).tar.gz

$(PROJECT).tar.gz:	$(SRC_FILES)
			tar -cf $(PROJECT).tar $(SRC_FILES)
			gzip -f $(PROJECT).tar

main.o:		main.c quadr.h

quadr.o:	quadr.c quadr.h
