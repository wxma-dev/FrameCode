include ../unix/env.mk

CFLAGS +=
LDFLAGS += $(SO_LDFLAGS)

LIB1=libim.so
OBJ1=im_hashtable.o im_vector.o

LIBS=$(LIB1)
OBJS=$(OBJ1)

all:check_depend .lib

$(LIB1):$(OBJ1)
	$(CC) -o $(LIB1) $(OBJ1) $(LDFLAGS)

include ../unix/rule.mk

