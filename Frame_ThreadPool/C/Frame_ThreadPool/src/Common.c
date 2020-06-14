#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <pthread.h>

#include "Common.h"

#include "Struct_Frame.h"
extern CThread_pool *pool;

/*
�������ڴ��ĵ�ַ��ӵ�������
*/
void Into_Queue( void * pPoint_Task )
{
	/* ����һ������ڵ���ڴ� */
	CList_Task *pNewWorker = ( CList_Task *) malloc( sizeof(CList_Task) );
	memset( pNewWorker, 0x00, sizeof( CList_Task ) );

	pNewWorker->pList_Task = pPoint_Task;

	/* ���� */
	pthread_mutex_lock( &(pool->queue_lock_F) );

	/* ��������ͷ�� */
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

	/* ���� */
	pthread_mutex_unlock( &(pool->queue_lock_F) );

	/* ֪ͨһ���ȴ����߳� */
	//pthread_cond_signal( &(pool->queue_ready_F) );
	pthread_cond_broadcast( &(pool->queue_ready_F) );
}


/*
����Ƿ���ϱ�׼
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
��ȡ����־λ��ָ��
*/
void * Get_Mark_Addr( void * const pPoint_Addr )
{
	void * pPoint_Mark_Addr;

	pPoint_Mark_Addr = NULL;

	/* '\n'��ʼ��ַ */
	pPoint_Mark_Addr = strchr( (char *)pPoint_Addr, '\n' );
	if ( NULL == pPoint_Mark_Addr )
	{
		return NULL;
	}

	return pPoint_Mark_Addr;
}

/*
���㡱��ŵ�ֵַ���ڴ�顰���׵�ַ
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
��ȡ����ŵ�ֵַ���ڴ�顰�е�ֵ
��Σ�pPoint_Start_Addr   ��ʼ�ĵ�ַ
���Σ�pPoint_Task_Addr    ����ŵ�ֵַ���ڴ�顰�е�ֵ����ָ��
*/
void Get_Task_Addr( void * const pPoint_Start_Addr, void ** pPoint_Task_Addr )
{
	memcpy( pPoint_Task_Addr, pPoint_Start_Addr, POINT_SIZE );
}


/*
�ͷš���ŵ�ֵַ���ڴ�顰ָ����ڴ�����
*/
void Free_Task_Addr( void * pPoint_Task_Addr )
{
	free( pPoint_Task_Addr ); 
	pPoint_Task_Addr = NULL;
}





