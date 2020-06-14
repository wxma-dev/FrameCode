

#功能：
#1、编译当前目录的源文件，编译成静态库、动态库文件
#2、执行当前目录的子目录的makefile，编译成静态库、动态库文件
#3、集合1/2的.o文件，编译成一个总的静态库、动态库文件


#注意：
#1、该Makefile先编译子目录，在编译当前目录
#2、若子目录下的编译 依赖当前目录的动态库，则如下makefile不适用
#3、该makefile只规定了编译规格的make all、make clear 格则，其他的make install格式，需在具体的makefile中具体规定


#用法:
#在具体的makefile安装如下格式引用即可
#    PATH_MAKEALLCHILDDIR = ../../../Frame_Makefile               #MakeAllChildDir.mk所在的目录
#    include $(PATH_MAKEALLCHILDDIR)/MakeAllChildDir.mk           #直接引用
#    .PHONY:all clean


#################################################################################################

#列举当前目录的子目录
SUBDIRS = $(shell find . -maxdepth 1 -type d | grep -v 'bin' | grep -v 'lib' | grep -v 'include' | grep -v 'conf' | sed -n '2,$$p' )

#子目录的个数
CHILD_DIR_COUNT = $(shell find . -maxdepth 1 -type d | grep -v 'bin' | grep -v 'lib' | grep -v 'include' | grep -v 'conf' | sed -n '2,$$p' | wc -l )

#编译子目录
define make_subdir
 @echo "---------------start make child dir----------------`pwd` " ; \
 for subdir in $(SUBDIRS) ; do \
  if [ -f $$subdir/Makefile -o -f $$subdir/makefile ];then \
   ( cd $$subdir && make $1) \
  fi; \
 done; \
 echo "---------------end make child dir----------------`pwd` " ;
endef

#编译子目录 make clean
define make_subdir_clean
 @echo "---------------start make clean child dir----------------`pwd` " ; \
 for subdir in $(SUBDIRS) ; do \
  if [ -f $$subdir/Makefile -o -f $$subdir/makefile ];then \
   ( cd $$subdir && make $1) \
  fi; \
 done; \
 echo "---------------end make clean child dir----------------`pwd` " ;
endef

#获取当前目录名
LOCAL_DIR_NAME = $(shell basename $(shell pwd))

#当前目录下编译的lib存在目录，注意比较
LOCAL_LIB_DIR = .

#以当前目录名作为lib名
LOCAL_LIBNAME_SO = lib$(LOCAL_DIR_NAME).so
LOCAL_LIBNAME_A  = lib$(LOCAL_DIR_NAME).a
LOCAL_LIBLIST_SO = $(LOCAL_LIB_DIR)/$(LOCAL_LIBNAME_SO)
LOCAL_LIBLIST_A  = $(LOCAL_LIB_DIR)/$(LOCAL_LIBNAME_A)

#集合的动态库、静态库
LOCAL_PUB_NAME_SO = libPUB$(LOCAL_DIR_NAME).so
LOCAL_PUB_NAME_A  = libPUB$(LOCAL_DIR_NAME).a
LOCAL_PUB_LIBLIST_SO = $(LOCAL_LIB_DIR)/$(LOCAL_PUB_NAME_SO)
LOCAL_PUB_LIBLIST_A  = $(LOCAL_LIB_DIR)/$(LOCAL_PUB_NAME_A)

#获取当前目录下的.o文件
LOCAL_SRC = ${wildcard *.c}
LOCAL_OBJ = ${patsubst %.c, %.o, $(LOCAL_SRC)}

#当前目录下要清理的文件
#LOCAL_CLEAN_FILES = ${LOCAL_OBJ} $(LOCAL_LIBLIST_SO) $(LOCAL_LIBLIST_A) $(LOCAL_PUB_LIBLIST_SO) $(LOCAL_PUB_LIBLIST_A)
LOCAL_CLEAN_FILES = *.o *.a *.so

#编译当前目录
define make_dir
  @echo "---------------start make local dir----------------`pwd` " ; \
  if [ -f Makefile -o -f makefile ];then \
    GET_C_FILE_COUNT=`find . -maxdepth 1 -type f -name '*.c' |wc -l`; \
    if [ $$GET_C_FILE_COUNT -gt 0 ];then \
      ( make $(join local_make_ , $1 ) ); \
    fi; \
  fi; \
  echo "---------------end make local dir----------------`pwd` " ;
endef

#当前目录执行 make clean
define make_dir_clean
  @echo "---------------start make clean local dir----------------`pwd` " ; \
  if [ -f Makefile -o -f makefile ];then \
      ( make $(join local_make_clean_ , $1 ) ); \
  fi; \
  echo "---------------end make clean local dir----------------`pwd` " ;
endef

#链接当前目录，所有子目录下的对象文件
#1、若当前目录有makefile文件，则继续编译
#2、循环判断当前目录的子目录下是否有makefile文件，
#       有则将该子目录下所有的.o文件列表取出
define link_StaticLib_To_DynamicLib
  @echo "---------------start [$(LOCAL_PUB_NAME_A)] link_StaticLib_To_DynamicLib----------------"; \
  if [ -f Makefile -o -f makefile ];then \
    echo "---------------link_StaticLib_To_DynamicLib, start, find static library in child dir.----------------"; \
    for subdir in $(SUBDIRS) ; do \
      echo "child dir : $$subdir" ; \
      if [ -f $$subdir/Makefile -o -f $$subdir/makefile ];then \
        GET_A_COUNT=`find $$subdir -maxdepth 1 -type f -name '*.a' |wc -l`; \
        echo "find number of static library file : $$GET_A_COUNT" ; \
        if [ $$GET_A_COUNT -gt 0 ];then \
          GET_A_LIST=`find $$subdir -maxdepth 1 -type f -name '*.a'`; \
          CHILD_DIR_A_LIST=$$CHILD_DIR_A_LIST" "$$GET_A_LIST ; \
        else \
          echo "error: not find static library file."; \
        fi; \
      fi; \
      printf "all static library file : \n$$CHILD_DIR_A_LIST\n"; \
    done; \
    echo "---------------link_StaticLib_To_DynamicLib, end, find static library in child dir.--------------------------------------------------------------------------------"; \
    if [ $(CHILD_DIR_COUNT) -gt 0 ];then \
      echo "---------------compile $(LOCAL_PUB_NAME_SO) start--------------------------------------------------------------------------------"; \
      echo "---------------compile  $$CHILD_DIR_A_LIST to $(LOCAL_PUB_NAME_SO) --------------------------------------------------------------------------------"; \
      printf "prepare comple static library file:\n"; \
      echo "$$CHILD_DIR_A_LIST"; \
      gcc -shared -fPIC -o $(LOCAL_PUB_NAME_SO) -Wl,--whole-archive $$CHILD_DIR_A_LIST  -Wl,--no-whole-archive; \
      echo "---------------compile $(LOCAL_PUB_NAME_SO) end--------------------------------------------------------------------------------"; \
      echo "---------------compile $(LOCAL_PUB_NAME_A) start--------------------------------------------------------------------------------"; \
      printf "prepare static library in child dir: \n$$CHILD_DIR_A_LIST\n"; \
      for a_list in $$CHILD_DIR_A_LIST ; do \
        cp $$a_list . && echo "cp $$a_list `pwd`" ; \
        A_FILE=`basename $$a_list` ; \
        ar x $$A_FILE  && echo "ar x $$A_FILE" ; \
        rm $$A_FILE && echo "rm $$A_FILE" ; \
      done; \
      ar rc $(LOCAL_PUB_NAME_A) *.o && echo "ar rc $(LOCAL_PUB_NAME_A) *.o"; \
      echo "---------------compile $(LOCAL_PUB_NAME_A) end--------------------------------------------------------------------------------"; \
    else \
      echo "info: `pwd` not have child dir,so don't create pub lib. "; \
    fi; \
  fi; \
  echo "---------------end [$(LOCAL_PUB_NAME_A)] link_StaticLib_To_DynamicLib--------------------------------------------------------------------------------" ;
endef




#编译子目录调用
all:
	$(call make_subdir , all)
	echo "end of call make_subdir"
	$(call make_dir , all)
	echo "end of call make_dir"
	$(call link_StaticLib_To_DynamicLib, all)
	echo "end of call link_StaticLib_To_DynamicLib"

debug:
	$(call make_subdir , debug)

clean:
	$(call make_subdir_clean , clean)
	$(call make_dir_clean , clean)

#当前目录下all规则
local_make_all:$(LOCAL_OBJ) $(LOCAL_LIBNAME_SO) $(LOCAL_LIBNAME_A)

#当前目录下.o对象文件规则
$(LOCAL_OBJ): %.o: %.c
	$(CC) -c -fPIC $^ -o $@

#当前目录下动态库规则
$(LOCAL_LIBNAME_SO):
	$(CC) -shared -fPIC -o $(LOCAL_LIBNAME_SO) $(LOCAL_OBJ)

#当前目录下静态库规则
$(LOCAL_LIBNAME_A):
	ar rcs $(LOCAL_LIBNAME_A) $(LOCAL_OBJ)

#当前目录下清理文件
local_make_clean_clean:
	$(RM) $(LOCAL_CLEAN_FILES)


.PHONY:all clean

.PHONY:local_make_all local_make_clean

#################################################################################################


