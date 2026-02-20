COURSE = cs220
COURSE_DIR = $$HOME/$(COURSE)

CC = gcc
CPPFLAGS = -g -Wall -I $(COURSE_DIR)/include -Wuse-after-free=0 -std=gnu2x

# add $(COURSE_DIR)/lib to directories to search for libraries
LDFLAGS = -L $(COURSE_DIR)/lib

# specify that course library be used
LDLIBS = -l$(COURSE)

TARGETS = morse-encode morse-decode
OBJS = main.o morse.o files.utils.o

all:		$(TARGET)

morse-encode:   $(OBJS)
	        $(CC)
                $(CPPFLAGS)
                $(LDFLAGS)
                $(OBJS)
                $(LDLIBS) -o $@

morse-decode:	morse-encode
                ln -s -f $< $@

.PHONY:		clean
clean:
		rm -rf *~ *.o $(TARGET)

.PHONY:		.gitignore
.gitignore:
		rm -f $@
		for f in $(TARGET) ; \
		do \
		  echo $$f >> $@ ; \
		done
