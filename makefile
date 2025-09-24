CC = gcc
CFLAGS = -std=c11 -O2 -Wall -Wextra -I.
SRCDIR = .
LIBDIR = lib
OBJ = main.o lib/pexpense.o

all: smartexpense

smartexpense: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

main.o: main.c lib/pexpense.h
	$(CC) $(CFLAGS) -c main.c

lib/pexpense.o: lib/pexpense.c lib/pexpense.h
	$(CC) $(CFLAGS) -c lib/pexpense.c -o lib/pexpense.o

clean:
	rm -f *.o lib/*.o smartexpense
