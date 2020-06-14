#ifndef __THREAD_STRUCT_H__
#define __THREAD_STRUCT_H__

#include <pthread.h>

/* ��������Ľṹ */
typedef struct worker {
	void * pList_Task;                /* void * ���͵�ָ�� */
	struct worker *next;              /* ����ʹ�� */
}CList_Task;

/* �̳߳صĽṹ */
typedef struct {
	pthread_mutex_t queue_lock_F;     /* ������ */
	pthread_cond_t queue_ready_F;     /* �������� */

	CList_Task * queue_head_F;        /* ָ���������ͷ��㣬�ٽ��� */
	int cur_queue_size_F;             /* ��¼�����й������������ٽ��� */

	int max_thread_num_F;             /* ����߳��� */
	pthread_t *add_threadid_F;        /* ��������߳�ID */
	int        add_thread_num_F;      /* ��������̵߳���Ŀ */
	pthread_t *do_threadid_F;         /* ִ�������߳�ID */
	int        do_thread_num_F;       /* ִ�������̵߳���Ŀ */

	int shutdown_F;                   /* ���� */
}CThread_pool;


#endif
