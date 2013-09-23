include config.mk

vpath %.h inc/
vpath %.c src/

INC += -I ./
INC += -I inc/
OBJ := error.o set.o relation.o tools.o

.PHONY : all doc clean

TARGET = librelafix

all : shared static

shared : $(OBJ)
	$(CC) $(CFLAGS) -shared -Wl,-soname,$(TARGET).so.2 -o $(TARGET).so.2.0.0 $^

static : $(OBJ)
	ar rcs $(TARGET).a $^

clean :
	rm -f $(OBJ) $(TARGET).so.* $(TARGET).a
	rm -rf doc

%.o : %.c %.h
	$(CC) $(CFLAGS) -c $(INC) -o $@ $<

doc :
	$(DOXYGEN)
