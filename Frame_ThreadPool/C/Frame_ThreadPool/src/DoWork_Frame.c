	#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include <pthread.h>

#include "DoWork_Frame.h"

#include "Log_Frame.h"
//#include "Application_Demo/Application.h"
#include "Common.h"

#include "Struct_Frame.h"
extern CThread_pool *pool;

/* 
DoWork_Appliction函数框架：
(1)获取”存放地址值的内存块“中的值,Get_Task_Addr
(2)执行具体的任务,DoWork
(3)释放”存放地址值的内存块“指向的内存区域,Free_Task_Addr
*/
int DoWork_Appliction( void * pPoint )
{
	void * pPoint_Task_Addr;

	pPoint_Task_Addr = NULL;

	Get_Task_Addr( pPoint, &pPoint_Task_Addr );

	DoWork( pPoint_Task_Addr );
	pPoint_Task_Addr = NULL;

	/* 这个暂时不需要的吧，在applition.c文件中进行添加free动作
	Free_Task_Addr( pPoint_Task_Addr );
	*/
	
}

/*********************************************************************
* Function：                                                         *
*    void *Call_Dowork_Thread( void *arg )                           *
* Description：                                                      *
*    为线程池的工作函数,从CList_Task链表中取的任务（函数，参数), *
*    然后调用对应的函数                                              *
* In Params:                                                         *
*                                                                    *
* Out Params:                                                        *
*                                                                    *
* Return:                                                            *
*                                                                    *
*********************************************************************/
void *Call_Dowork_Thread( void *arg )
{
	INFO_FRAME( "starting thread 0x%x\n", (int)pthread_self() );
	while ( 1 )
	{
		/* 从工作链表中取工作，要先加互斥锁，参数为锁地址 */
		pthread_mutex_lock( &(pool->queue_lock_F) );

		/* 链表为空 */
		while( pool->cur_queue_size_F == 0 && !pool->shutdown_F )
		{
			DEBUG_FRAME( "thread 0x%x is waiting\n", (int)pthread_self() );

			/* 等待资源，信号量用于通知。会释放第二个参数的锁，以供添加；函数返回时重新加锁。 */
			pthread_cond_wait( &(pool->queue_ready_F), &(pool->queue_lock_F) );
		}

		if ( pool->shutdown_F )
		{
			/* 结束开关开启，释放锁并退出线程 */
			pthread_mutex_unlock( &(pool->queue_lock_F) );  
			DEBUG_FRAME( "thread 0x%x will exit\n", (int)pthread_self() );
			pthread_exit( NULL );  /* 参数为void * */
		}

		DEBUG_FRAME( "thread 0x%x is starting to work\n", (int)pthread_self() );

		--pool->cur_queue_size_F;
		CList_Task *pWorker = pool->queue_head_F;
		pool->queue_head_F = pWorker->next;

		/* 获取一个任务后释放锁 */
		pthread_mutex_unlock( &(pool->queue_lock_F) );

		/* 执行任务、释放内存 */
		DoWork_Appliction( pWorker->pList_Task );

		/* 不需要free application.c中进行了free，原因：free内存的方式不知道是free还是delete
		free( pWorker->pList_Task );
		*/
		pWorker->pList_Task = NULL;

		free( pWorker );
		pWorker = NULL;
	}
	pthread_exit( NULL );

	return NULL;
}


