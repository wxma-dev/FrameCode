include ../unix/env.mk

CFLAGS += -I../utils/beai3
LDFLAGS += $(DYNAMIC_LDFLAGS) -L../.lib -lBeai3Dict

BIN1=simple
OBJ1=simple.o

BIN2=nested
OBJ2=nested.o

BIN3=array
OBJ3=array.o

BINS=$(BIN1) $(BIN2) $(BIN3)
OBJS=$(OBJ1) $(OBJ2) $(OBJ3)

all:check_depend .bin

$(BIN1):$(OBJ1)
	$(CC) -o $(BIN1) $(OBJ1) $(LDFLAGS)

include ../unix/rule.mk

