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

# DO NOT DELETE THIS LINE -- make depend depends on it.
im_hashtable.o: im_hashtable.c /usr/include/stdc-predef.h \
 /usr/include/stdlib.h /usr/include/features.h /usr/include/sys/cdefs.h \
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
 /usr/include/xlocale.h im_hashtable.h im_types.h im_vector.h \
 prime_number.h
im_vector.o: im_vector.c /usr/include/stdc-predef.h /usr/include/stdlib.h \
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
 /usr/include/xlocale.h im_vector.h im_types.h im_hashtable.h
