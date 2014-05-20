CC = gcc
CFLAGS = -Wall -ansi -pedantic -pthread  $(PNAME).c -o $(PNAME)
PNAME = proj02

all: $(PNAME).c
	$(CC) $(CFLAGS)

clean: $(PNAME).c
	rm -f $(PNAME)
