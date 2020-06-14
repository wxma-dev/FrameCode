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

# DO NOT DELETE THIS LINE -- make depend depends on it.
Beai3Dict.o: Beai3Dict.c /usr/include/stdc-predef.h /usr/include/stdlib.h \
 /usr/include/features.h /usr/include/sys/cdefs.h \
 /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h \
 /usr/include/gnu/stubs-64.h \
 /usr/lib/gcc/x86_64-redhat-linux/4.8.5/include/stddef.h \
 /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h \
 /usr/include/endian.h /usr/include/bits/endian.h \
 /usr/include/bits/byteswap.h /usr/include/bits/types.h \
 /usr/include/bits/typesizes.h /usr/include/bits/byteswap-16.h \
 /usr/include/sys/types.h /usr/include/time.h /usr/include/sys/select.h \
 /usr/include/bits/select.h /usr/include/bits/sigset.h \
 /usr/include/bits/time.h /usr/include/sys/sysmacros.h \
 /usr/include/bits/pthreadtypes.h /usr/include/alloca.h \
 /usr/include/bits/stdlib-float.h /usr/include/string.h \
 /usr/include/xlocale.h ../../src/im_hashtable.h ../../src/im_types.h \
 ../../src/im_vector.h Beai3Dict.h
