#include "struct.h"

#include "dowork.h"


void read_config_cache()
{
	
}


void select_transcode()
{
	
}


void mq_send_info_ywjc( void * arg )
{
	char    sTransCode[ 6 + 1 ];
	char   *pPoint = NULL;

	memset( sTransCode, 0x00, sizeof( sTransCode ) );
	pPoint = NULL;

	pPoint = (char*) arg;
	memcpy( sTransCode, pPoint, 6 );
	
	/* 读取共享内存的配置内容 */
	
	
	
}

/*********************************************************************
* Function：                                                         *
*    void *thread_routine_dowork( void *arg )                        *
* Description：                                                      *
*    为线程池的工作函数,从CThread_worker链表中取的任务（函数，参数), *
*    然后调用对应的函数                                              *
* In Params:                                                         *
*                                                                    *
* Out Params:                                                        *
*                                                                    *
* Return:                                                            *
*                                                                    *
*********************************************************************/
void *thread_routine_dowork( void *arg )
{
	INFO( "thread of dowrk [%d] start", pthread_self() );
	while ( 1 )
	{
		/* 从工作链表中取工作，要先加互斥锁，参数为锁地址 */
		pthread_mutex_lock( &(pool->queue_lock) );

		/* 链表为空 */
		while( pool->cur_queue_size == 0 && !pool->shutdown )
		{
			INFO( "thread 0x%x is waiting\n", (int)pthread_self() );
			/* 等待资源，信号量用于通知。会释放第二个参数的锁，以供添加；函数返回时重新加锁。 */
			pthread_cond_wait( &(pool->queue_ready), &(pool->queue_lock) );
		}

		if ( pool->shutdown ) 
		{
			/* 结束开关开启，释放锁并退出线程 */
			pthread_mutex_unlock( &(pool->queue_lock) );  
			INFO( "thread 0x%x will exit\n", (int)pthread_self() );
			pthread_exit( NULL );  /* 参数为void * */
		}

		DEBUG( "thread 0x%x is starting to work\n", (int)pthread_self() );

		--pool->cur_queue_size;
		CThread_worker *worker = pool->queue_head;
		pool->queue_head = worker->next;

		/* 获取一个工作后释放锁 */
		pthread_mutex_unlock( &(pool->queue_lock) );

		/* 做工作 */
		(*(worker->process))( worker->arg );  

		free( worker );
		worker = NULL;
	}

	INFO( "thread of dowrk [%d] end", pthread_self() );
	pthread_exit( NULL );

	return NULL;
}


