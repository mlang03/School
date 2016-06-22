CC = gcc

CFLAGS = 

OBJS = lex.yy.o stack.o main.o

scanner: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o scanner -ll

main.o: main.c globals.h stack.h
	$(CC) $(CFLAGS) -c main.c

stack.o: stack.c globals.h stack.h
	$(CC) $(CFLAGS) -c stack.c

lex.yy.o: lex.yy.c globals.h stack.h
	$(CC) $(CFLAGS) -c lex.yy.c

lex.yy.c: SGML.l
	flex SGML.l

clean:
	rm -f scanner $(OBJS) lex.yy.c