#最终的目标文件
TARGET = Monitor_ChildProcess

CFLAGS = -g -Wall


#子目录中的对象文件
SUBDIR_OBJLIST = lib_time.o \
    lib_file.o \
    lib_conf_file.o \
    lib_string.o

#本工程需要的所有对象文件
OBJLIST = Monitor_ChildProcess.o \
    ${SUBDIR_OBJLIST}

#编译所有子目录
SUBDIRS=`ls -d */ | grep -v 'bin' | grep -v 'lib' | grep -v 'include'`
#编译指定子目录
#SUBDIRS=dir1 dir2 dir3

#编译子目录
define make_subdir
 @for subdir in $(SUBDIRS) ; do \
 ( cd $$subdir && make $1) \
 done;
 make $(join project_ , $1 )
endef


all:
	$(call make_subdir , all)

project_all:${TARGET}

debug:
	$(call make_subdir , debug)

clean:
	$(call make_subdir , clean)

project_clean:
	rm -rf ${OBJLIST}


${TARGET}:${OBJLIST}
	gcc ${OBJLIST} -o ${TARGET} ${CFLAGS}

Monitor_ChildProcess.o:Monitor_ChildProcess.c Monitor_ChildProcess.h
	gcc -c Monitor_ChildProcess.c ${CFLAGS}