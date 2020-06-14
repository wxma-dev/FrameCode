

#功能：
#1、该makefile编译所有的应用bin


#################################################################################################

#引出变量 FRAME_OBJ_LIST
include ./Frame_Obj.mk

#列举当前目录的子目录
SUBDIRS = $(shell find . -maxdepth 1 -type d | grep -v 'bin' | grep -v 'lib' | grep -v 'include' | grep -v 'conf' | sed -n '2,$$p' )

#编译子目录
define make_subdir
 @echo "---------------start make child dir----------------`pwd` " ; \
 for subdir in $(SUBDIRS) ; do \
  if [ -f $$subdir/Makefile -o -f $$subdir/makefile ];then \
     ( cd $$subdir && make $1 ) ; \
  fi; \
 done; \
 echo "---------------end make child dir----------------`pwd` " ;
endef


define make_clean
 @echo "---------------start make clean child dir----------------`pwd` " ; \
 for subdir in $(SUBDIRS) ; do \
  if [ -f $$subdir/Makefile -o -f $$subdir/makefile ];then \
     ( cd $$subdir && make $1 ); \
  fi; \
 done; \
 echo "---------------end make clean child dir----------------`pwd` " ;
endef


all: ${FRAME_OBJ_LIST} make_app

make_app:
	$(call make_subdir , all)

clean:
	$(call make_clean , clean)
	$(RM) $(FRAME_OBJ_LIST)


Main_Thread_Pool.o:Main_Thread_Pool.c Struct_Frame.h Log_Frame.h
	gcc -o Main_Thread_Pool.o -c Main_Thread_Pool.c ${CFLAGS}
AddWork_Frame.o:AddWork_Frame.c AddWork_Frame.h Struct_Frame.h Log_Frame.h
	gcc -c AddWork_Frame.c ${CFLAGS}
DoWork_Frame.o:DoWork_Frame.c DoWork_Frame.h Struct_Frame.h Log_Frame.h
	gcc -c DoWork_Frame.c ${CFLAGS}
Common.o:Common.c Common.h
	gcc -c Common.c ${CFLAGS}


.PHONY:all clean

.PHONY:make_app clean_app

#################################################################################################


