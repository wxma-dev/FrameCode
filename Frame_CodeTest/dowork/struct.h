#ifndef __THREAD_STRUCT_H__
#define __THREAD_STRUCT_H__

#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <pthread.h>

#define FILE_IS   1
#define DIR_IS    1

#define THREADPOOL_ADDWORK_INIT_NUM    1
#define THREADPOOL_DOWORK_INIT_NUM     5

#define MAXLINE 40960

#define MAXEPOLLSIZE 10240

/* 工作链表的结构 */
typedef struct worker {
	char  worker_str_buf[4096];
	
	int   work_fd;
	
	char oper_name[1024];
	char oper_code[1024];

	void *(*process)(void *arg);    /* 工作函数 */
	void *arg;                      /* 函数的参数 */
	struct worker *next;
}CThread_worker;

/* 线程池的结构 */
typedef struct {
    pthread_mutex_t queue_lock;     /* 互斥锁 */
    pthread_cond_t queue_ready;     /* 条件变量/信号量 */

    CThread_worker *queue_head;     /* 指向工作链表的头结点，临界区 */
    int cur_queue_size;             /* 记录链表中工作的数量，临界区 */

    int max_thread_num;             /* 最大线程数 */
    int addwork_threadnum;          /* 添加任务线程数目 */
    int dowork_threadnum;           /* 工作线程数目 */
    pthread_t *addwork_threadid;    /* 添加任务线程ID */
    pthread_t *dowork_threadid;     /* 工作线程ID */

    int shutdown;                   /* 开关 */
}CThread_pool;

typedef struct dirfile {
	char    sSrcName[1024+1];
	char    sDstName[1024+1];
	int     iIsFileDir;             /* FILE_IS表示文件，DIR_IS表示目录 */
}St_dirFile;


typedef struct conf_struct {
	char   sJym[6+1];               /* 交易码 */
	void *(*pFunc)(void *arg);      /* 函数入口 */
}Conf_Struct;

#endif
