#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <pthread.h>

#include "Common.h"

#include "Struct_Frame.h"
extern CThread_pool *pool;

/*
将任务内存块的地址添加到链表中
*/
void Into_Queue( void * pPoint_Task )
{
	/* 分配一个链表节点的内存 */
	CList_Task *pNewWorker = ( CList_Task *) malloc( sizeof(CList_Task) );
	memset( pNewWorker, 0x00, sizeof( CList_Task ) );

	pNewWorker->pList_Task = pPoint_Task;

	/* 加锁 */
	pthread_mutex_lock( &(pool->queue_lock_F) );

	/* 插入链表头部 */
	CList_Task *pMember = pool->queue_head_F;
	if( NULL != pMember )
	{
		while( NULL != pMember->next )
		{
			pMember = pMember->next;
		}
		pMember->next = pNewWorker;
	}
	else
	{
		pool->queue_head_F = pNewWorker;
	}
	++(pool->cur_queue_size_F);

	/* 解锁 */
	pthread_mutex_unlock( &(pool->queue_lock_F) );

	/* 通知一个等待的线程 */
	//pthread_cond_signal( &(pool->queue_ready_F) );
	pthread_cond_broadcast( &(pool->queue_ready_F) );
}


/*
检查是否符合标准
*/
int Check_Task_Mem( void * const pPoint )
{
	if ( NULL ==  Get_Start_Addr( pPoint ) )
	{
		return -1;
	}

	return 0;
}


/*
获取到标志位的指针
*/
void * Get_Mark_Addr( void * const pPoint_Addr )
{
	void * pPoint_Mark_Addr;

	pPoint_Mark_Addr = NULL;

	/* '\n'开始地址 */
	pPoint_Mark_Addr = strchr( (char *)pPoint_Addr, '\n' );
	if ( NULL == pPoint_Mark_Addr )
	{
		return NULL;
	}

	return pPoint_Mark_Addr;
}

/*
计算”存放地址值的内存块“的首地址
*/
void * Get_Start_Addr( void * const pPoint_Addr )
{
	void * pPoint_Start_Addr;

	pPoint_Start_Addr = NULL;

	pPoint_Start_Addr = Get_Mark_Addr( pPoint_Addr );
	if ( NULL == pPoint_Start_Addr )
	{
		return NULL;
	}
	pPoint_Start_Addr++;

	return pPoint_Start_Addr;
}


/*
获取”存放地址值的内存块“中的值
入参：pPoint_Start_Addr   开始的地址
出参：pPoint_Task_Addr    ”存放地址值的内存块“中的值二级指针
*/
void Get_Task_Addr( void * const pPoint_Start_Addr, void ** pPoint_Task_Addr )
{
	memcpy( pPoint_Task_Addr, pPoint_Start_Addr, POINT_SIZE );
}


/*
释放”存放地址值的内存块“指向的内存区域
*/
void Free_Task_Addr( void * pPoint_Task_Addr )
{
	free( pPoint_Task_Addr ); 
	pPoint_Task_Addr = NULL;
}





