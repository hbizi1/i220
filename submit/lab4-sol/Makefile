COURSE = cs220
COURSE_DIR = $$HOME/$(COURSE)

CC = gcc
CPPFLAGS = -g -Wall -I $(COURSE_DIR)/include -Wuse-after-free=0 -std=gnu2x

# add $(COURSE_DIR)/lib to directories to search for libraries
LDFLAGS = -L $(COURSE_DIR)/lib

# specify that course library be used
LDLIBS = -l$(COURSE)

#produce a list of all C files (assume each C file contains a main())
C_FILES = $(wildcard *.c)

#produce a list of all *.o files by substituting cc in CXX_FILES with o.
OUTFILES = $(C_FILES:.c=)

TARGET = $(OUTFILES)

#default target
all:		$(TARGET)


.PHONY:		clean
clean:
		rm -rf *~ *.o $(TARGET)

.PHONY:		.gitignore
.gitignore:
		rm -f $@
		for f in $(OUTFILES) ; \
		do \
		  echo $$f >> $@ ; \
		done
