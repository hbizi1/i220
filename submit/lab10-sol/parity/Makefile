TARGET = parity

CFLAGS = -g -Wall -std=c2x -Og

$(TARGET):	main.o parity.o
		$(CC) $^ -o $@

parity.o:	parity.s
		$(CC) -c $(CFLAGS) $< -o $@

.PHONY:		clean
clean:
		rm -f *.o $(TARGET)
