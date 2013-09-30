include config.mk

vpath %.h inc/
vpath %.c src/
vpath %.c test/

INC += -I ./
INC += -I inc/
OBJ := error.o set.o relation.o tools.o

TEST_OBJ := cu_main.o test_set.o test_relation.o test_tools.o

.PHONY : all clean
.PHONY : test
.PHONY : doc

TARGET = librelafix

TEST_TARGET = cu_test_$(TARGET)

.SUFFIXES:

all : shared static

shared : $(OBJ)
	$(CC) $(CFLAGS) -shared -Wl,-soname,$(TARGET).so.2 -o $(TARGET).so.2.0.0 $^

static : $(OBJ)
	ar rcs $(TARGET).a $^

%.o : %.c %.h
	$(CC) $(CFLAGS) -c $(INC) -o $@ $<

doc :
	$(DOXYGEN)

test : $(TEST_OBJ) $(OBJ)
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $^ $(LIBPATH) -lcunit

$(TEST_OBJ) : %.o : %.c
	$(CC) $(CFLAGS) -c $(INC) -o $@ $<

clean :
	rm -f $(OBJ)
	rm -f $(TEST_OBJ)
	rm -f $(TARGET).so.* $(TARGET).a
	rm -f $(TEST_TARGET)
	rm -rf doc
