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
DoWork_Appliction������ܣ�
(1)��ȡ����ŵ�ֵַ���ڴ�顰�е�ֵ,Get_Task_Addr
(2)ִ�о��������,DoWork
(3)�ͷš���ŵ�ֵַ���ڴ�顰ָ����ڴ�����,Free_Task_Addr
*/
int DoWork_Appliction( void * pPoint )
{
	void * pPoint_Task_Addr;

	pPoint_Task_Addr = NULL;

	Get_Task_Addr( pPoint, &pPoint_Task_Addr );

	DoWork( pPoint_Task_Addr );
	pPoint_Task_Addr = NULL;

	/* �����ʱ����Ҫ�İɣ���applition.c�ļ��н������free����
	Free_Task_Addr( pPoint_Task_Addr );
	*/
	
}

/*********************************************************************
* Function��                                                         *
*    void *Call_Dowork_Thread( void *arg )                           *
* Description��                                                      *
*    Ϊ�̳߳صĹ�������,��CList_Task������ȡ�����񣨺���������), *
*    Ȼ����ö�Ӧ�ĺ���                                              *
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
		/* �ӹ���������ȡ������Ҫ�ȼӻ�����������Ϊ����ַ */
		pthread_mutex_lock( &(pool->queue_lock_F) );

		/* ����Ϊ�� */
		while( pool->cur_queue_size_F == 0 && !pool->shutdown_F )
		{
			DEBUG_FRAME( "thread 0x%x is waiting\n", (int)pthread_self() );

			/* �ȴ���Դ���ź�������֪ͨ�����ͷŵڶ��������������Թ���ӣ���������ʱ���¼����� */
			pthread_cond_wait( &(pool->queue_ready_F), &(pool->queue_lock_F) );
		}

		if ( pool->shutdown_F )
		{
			/* �������ؿ������ͷ������˳��߳� */
			pthread_mutex_unlock( &(pool->queue_lock_F) );  
			DEBUG_FRAME( "thread 0x%x will exit\n", (int)pthread_self() );
			pthread_exit( NULL );  /* ����Ϊvoid * */
		}

		DEBUG_FRAME( "thread 0x%x is starting to work\n", (int)pthread_self() );

		--pool->cur_queue_size_F;
		CList_Task *pWorker = pool->queue_head_F;
		pool->queue_head_F = pWorker->next;

		/* ��ȡһ��������ͷ��� */
		pthread_mutex_unlock( &(pool->queue_lock_F) );

		/* ִ�������ͷ��ڴ� */
		DoWork_Appliction( pWorker->pList_Task );

		/* ����Ҫfree application.c�н�����free��ԭ��free�ڴ�ķ�ʽ��֪����free����delete
		free( pWorker->pList_Task );
		*/
		pWorker->pList_Task = NULL;

		free( pWorker );
		pWorker = NULL;
	}
	pthread_exit( NULL );

	return NULL;
}


