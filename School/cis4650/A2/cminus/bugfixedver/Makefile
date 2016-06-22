#
# Makefile for CMINUS
# Using flex and yacc
#

CC = gcc

CFLAGS = 

OBJS = y.tab.o lex.yy.o util.o main.o

tiny: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o cm -ly -ll

main.o: main.c globals.h util.h scan.h parse.h
	$(CC) $(CFLAGS) -c main.c

util.o: util.c util.h globals.h y.tab.h
	$(CC) $(CFLAGS) -c util.c

lex.yy.c: cminus.l
	flex cminus.l

lex.yy.o: lex.yy.c globals.h util.h y.tab.h
	$(CC) $(CFLAGS) -c lex.yy.c

y.tab.c: cminus.y
	yacc -d -v cminus.y

y.tab.o: y.tab.c globals.h util.h scan.h parse.h
	$(CC) $(CFLAGS) -c y.tab.c

clean:
	rm -f cm $(OBJS) lex.yy.c y.tab.h y.tab.c y.output
