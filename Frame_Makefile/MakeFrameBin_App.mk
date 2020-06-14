

#功能：
#1、实现具体应用场景的makefile

#################################################################################################
#引出变量 GET_FRAME_OBJ_LIST
include ../Frame_Obj.mk

CFLAGS = -g -Wall 

LIBDIR = ../../../../../../Frame_Lib/

#获取当前目录名
LOCAL_DIR_NAME = $(shell basename $(shell pwd))

APP_BIN_NAME = bin_$(LOCAL_DIR_NAME)

#获取当前目录下的.o文件
LOCAL_SRC = ${wildcard *.c}
LOCAL_OBJ = ${patsubst %.c, %.o, $(LOCAL_SRC)}

all:$(LOCAL_OBJ) make_bin

make_bin:
	gcc ${LOCAL_OBJ} ${GET_FRAME_OBJ_LIST} ${CFLAGS} -o ${APP_BIN_NAME} -lpthread  -L${LIBDIR} -lPUBFrame_Lib -Wl,-rpath=${LIBDIR} 

#当前目录下.o对象文件规则
$(LOCAL_OBJ): %.o: %.c
	$(CC) -c -fPIC $^ -o $@

clean:
	$(RM) $(LOCAL_OBJ) $(APP_BIN_NAME)

.PHONY:all clean

#################################################################################################




