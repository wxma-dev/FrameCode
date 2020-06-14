#最终的目标文件
TARGET = $(RECV_TARGET) \
         $(SEND_TARGET)

RECV_TARGET = Frame_SocketRecv
SEND_TARGET = Frame_SocketSend

#本工程需要的所有对象文件
RECV_OBJ = Frame_SocketRecv.o
SEND_OBJ = Frame_SocketSend.o
SOCKET_OBJ = yl_socket.o

#GCC参数
CFLAGS = -g -Wall

LIB_PATH_ZJ=/mnt/winFile/5技术理论与实践/Frame_Lib/lib

#其中-Wl,-rpath指定执行的时候寻找so的路径
LIB_CFLAGS = -L${LIB_PATH_ZJ} -lPUB -Wl,-rpath=${LIB_PATH_ZJ}

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

${RECV_TARGET}:${RECV_OBJ} ${SOCKET_OBJ}
	gcc ${RECV_OBJ} ${SOCKET_OBJ} ${CFLAGS} -o ${RECV_TARGET} ${LIB_CFLAGS}

${SEND_TARGET}:${SEND_OBJ} ${SOCKET_OBJ}
	gcc ${SEND_OBJ} ${SOCKET_OBJ} ${CFLAGS} -o ${SEND_TARGET} ${LIB_CFLAGS}

${RECV_OBJ}:Frame_SocketRecv.c
	gcc -o ${RECV_OBJ} -c Frame_SocketRecv.c ${CFLAGS}

${SEND_OBJ}:Frame_SocketSend.c
	gcc -o ${SEND_OBJ} -c Frame_SocketSend.c ${CFLAGS}

${SOCKET_OBJ}:yl_socket.c
	gcc -o ${SOCKET_OBJ} -c yl_socket.c ${CFLAGS}
