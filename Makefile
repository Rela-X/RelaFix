include config.mk

vpath %.h inc/
vpath %.c src/
vpath %.y src/

INC += -I ./
INC += -I inc/
OBJ := array.o domain.o formula.o function.o lexer.o list.o negation.o operation.o parse.o parser.tab.o relation.o stack.o table.o tools.o

all : $(OBJ)
	ar rcs librelafix.a $(OBJ)

clean :
	rm -f *.o *.a *.tab.c *.tab.h

lexer.o : lexer.c parser.tab.h
	$(CC) $(CFLAGS) -c $(INC) -o $@ $<

%.o : %.c %.h
	$(CC) $(CFLAGS) -c $(INC) -o $@ $<

%.tab.c %.tab.h : %.y
	$(YACC) $(YFLAGS) -b $* $<
