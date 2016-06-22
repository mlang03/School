CC = gcc
EX = vcftool
CFILES = vcutil.c vcftool.c
CFLAGS = -c -g -Wall -std=c99
OBJS = vcutil.o vcftool.o

vcftool: compile
	$(CC) $(OBJS) -o $(EX) -lpcre -I/opt/local/include -L/opt/local/lib

compile:
	$(CC) $(CFLAGS) $(CFILES)

clean:
	rm *.o
	rm vcftool