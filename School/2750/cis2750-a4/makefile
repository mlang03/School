CC = gcc
CPPFLAGS = -I/usr/include/python3.1 -DNDEBUG
CFLAGS = -Wall -std=c99 -g -fPIC
LDFLAGS = -lpcre -L/usr/lib/python3.1 -lpython3.1

all: vcftool.o Vcf.so


vcftool: vcutil.o vcutil.o

vcutil.o: vcutil.c vcutil.h

vcftool.o: vcftool.c vcftool.h vcutil.h

Vcf.so: extend.o vcutil.o
	$(CC) $^ -shared -o $@

extend.o: extend.c vcutil.c vcutil.h

clean:
	rm -f *.o *.so vcftool
