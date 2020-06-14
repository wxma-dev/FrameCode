PROJECTDIR = ../../../../..

include $(PROJECTDIR)/unix/env.mk

CFLAGS += -I$(PROJECTDIR)/include $(ORA_INCLUDE) $(TONG_INCLUDE)
LDFLAGS += $(SO_LDFLAGS)

LIB1=libBeaiLog.so
OBJ1=loglib.o

LIBS=$(LIB1)
OBJS=$(OBJ1) 

all:check_depend .lib

$(LIB1):$(OBJ1)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJ1)

include $(PROJECTDIR)/unix/rule.mk

