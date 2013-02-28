include config.mk

vpath %.h inc/
vpath %.c src/

INC += -I ./
INC += -I inc/
OBJ := array.o domain.o list.o negation.o relation.o stack.o table.o tools.o

.PHONY : all doc clean

TARGET = librelafix.a

all : $(OBJ)
	ar rcs $(TARGET) $(OBJ)

clean :
	rm -f $(OBJ) $(TARGET)
	rm -rf doc

%.o : %.c %.h
	$(CC) $(CFLAGS) -c $(INC) -o $@ $<

doc :
	$(DOXYGEN)
