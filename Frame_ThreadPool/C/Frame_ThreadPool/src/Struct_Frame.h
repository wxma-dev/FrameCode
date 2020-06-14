#ifndef __THREAD_STRUCT_H__
#define __THREAD_STRUCT_H__

#include <pthread.h>

/* 工作链表的结构 */
typedef struct worker {
	void * pList_Task;                /* void * 类型的指针 */
	struct worker *next;              /* 链表使用 */
}CList_Task;

/* 线程池的结构 */
typedef struct {
	pthread_mutex_t queue_lock_F;     /* 互斥锁 */
	pthread_cond_t queue_ready_F;     /* 条件变量 */

	CList_Task * queue_head_F;        /* 指向工作链表的头结点，临界区 */
	int cur_queue_size_F;             /* 记录链表中工作的数量，临界区 */

	int max_thread_num_F;             /* 最大线程数 */
	pthread_t *add_threadid_F;        /* 添加任务线程ID */
	int        add_thread_num_F;      /* 添加任务线程的数目 */
	pthread_t *do_threadid_F;         /* 执行任务线程ID */
	int        do_thread_num_F;       /* 执行任务线程的数目 */

	int shutdown_F;                   /* 开关 */
}CThread_pool;


#endif
