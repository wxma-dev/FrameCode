#最终的目标文件
TARGET = $(RECV_TARGET) \
         $(SEND_TARGET)

RECV_TARGET = Frame_SocketRecv
SEND_TARGET = Frame_SocketSend

#本工程需要的所有对象文件
RECV_OBJ = Frame_SocketRecv.o
SEND_OBJ = Frame_SocketSend.o

#GCC参数
CFLAGS = -g -Wall

#其中-Wl,-rpath指定执行的时候寻找so的路径
LIB_CFLAGS = -L/home/mawx/Frame_Lib -lPUB -Wl,-rpath=/home/mawx/Frame_Lib

#clean内容
CLEANLIST = $(TARGET) \
            $(RECV_OBJ) \
            $(SEND_OBJ) \
            $(SOCKET_OBJ)

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
	rm -rf ${CLEANLIST}

${RECV_TARGET}:${RECV_OBJ}
	gcc ${RECV_OBJ} ${CFLAGS} -o ${RECV_TARGET} ${LIB_CFLAGS}

${SEND_TARGET}:${SEND_OBJ}
	gcc ${SEND_OBJ} ${CFLAGS} -o ${SEND_TARGET} ${LIB_CFLAGS}

${RECV_OBJ}:asynccom-server.c
	gcc -o ${RECV_OBJ} -c asynccom-server.c ${CFLAGS}

${SEND_OBJ}:asynccom-client.c
	gcc -o ${SEND_OBJ} -c asynccom-client.c ${CFLAGS}


