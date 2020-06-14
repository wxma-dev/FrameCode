include ../../unix/env.mk

CFLAGS +=
LDFLAGS += $(SO_LDFLAGS) $(DYNAMIC_LDFLAGS) -L../../.lib -lim

LIB1=libBeai3Dict.so
OBJ1=Beai3Dict.o

LIBS=$(LIB1)
OBJS=$(OBJ1)

all:check_depend .lib

$(LIB1):$(OBJ1)
	$(CC) -o $(LIB1) $(OBJ1) $(LDFLAGS)

install:.header_install

.header_install:
	$(MD) -p $(PREFIX)/include
	$(CP) Beai3Dict.h $(PREFIX)/include

include ../../unix/rule.mk

