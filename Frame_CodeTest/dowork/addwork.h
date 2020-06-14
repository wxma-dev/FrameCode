#ifndef __THREAD_ADDWORK_H__
#define __THREAD_ADDWORK_H__

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <signal.h>

#include <pthread.h>

#include "struct.h"
#include "log.h"

extern CThread_pool *pool ;

/* 负责向工作链表中添加工作 */
void *thread_routine_addwork( void *(*process)(void *), void *arg );  

#endif
