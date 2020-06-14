#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <pthread.h> 
#include <signal.h>


#include "Common.h"

#include "AddWork_Frame.h"
#include "DoWork_Frame.h"
#include "Log_Frame.h"




#include "Struct_Frame.h"
/* �̳߳ؽṹָ�� */
CThread_pool *pool;

/*********************************************************************
* Function��                                                         *
*    CThread_pool * ThreadPool_Init( int addwork_thread_num, (void *)(*FunctionName_Addwork)(void *), int dowork_thread_num, (void *)(*FunctionName_Dowork)(void *) ) *
* Description��                                                      *
*    �̳߳س�ʼ��,������������߳�,���������߳�,��ʼ���̳߳ؽṹ     *
* In Params:                                                         *
* Out Params:                                                        *
*                                                                    *
* Return:                                                            *
*    ��NULL �̳߳ؽṹ��ַ                                           *
*    NULL   ʧ��                                                     *
*********************************************************************/
CThread_pool * ThreadPool_Init( CThread_pool **pPoint_CThread_pool, int addwork_thread_num, void *(*FunctionName_Addwork)(void *), void * FunctionArgv_Addwork, int dowork_thread_num, void *(*FunctionName_Dowork)(void *), void * FunctionArgv_Dowork )
{
	int     iRet = 0;
	int     i = 0;
	int     thread_num = 0;
	CThread_pool * pThread_pool;

	DEBUG_FRAME( "ThreadPool_Init��ʼ." );

	if ( NULL == FunctionName_Addwork )
	{
		ERROR_FRAME( "������������ܿ�!" );
		return NULL;
	}
	else if ( NULL == FunctionName_Dowork )
	{
		ERROR_FRAME( "����������Ϊ��!" );
		return NULL;
	}

	if ( 0 == addwork_thread_num )
	{
		ERROR_FRAME( "��������̵߳���Ŀ����Ϊ0!" );
		return NULL;
	}
	else if ( 0 == addwork_thread_num )
	{
		ERROR_FRAME( "�����̵߳���Ŀ����Ϊ0!" );
		return NULL;
	}

	DEBUG_FRAME( "����CThread_pool�Ŀռ�" );
	pThread_pool = (CThread_pool *) malloc( sizeof( CThread_pool ) ); 

	/* ��ȫ�ֵĸñ�����ֵ */
	*pPoint_CThread_pool = pThread_pool;

	/* ��������ʼ��������Ϊ���ĵ�ַ */
	DEBUG_FRAME( "��ʼ��������CThread_pool->queue_lock_F" );
	iRet = pthread_mutex_init( &(pThread_pool->queue_lock_F), NULL );
	if ( 0 != iRet )
	{
		ERROR_FRAME( "��ʼ��������CThread_pool->queue_lock_Fʧ��." );
		return NULL;
	}

	/* ����������ʼ��������Ϊ������ַ */
	DEBUG_FRAME( "��ʼ����������CThread_pool->queue_ready_F" );
	iRet = pthread_cond_init( &(pThread_pool->queue_ready_F), NULL );
	if ( 0 != iRet )
	{
		ERROR_FRAME( "��ʼ����������CThread_pool->queue_ready_F." );
		return NULL;
	}

	pThread_pool->add_thread_num_F = addwork_thread_num;
	pThread_pool->do_thread_num_F = dowork_thread_num;
	pThread_pool->max_thread_num_F = addwork_thread_num + dowork_thread_num;;
	pThread_pool->queue_head_F = NULL;
	pThread_pool->cur_queue_size_F = 0;
	pThread_pool->shutdown_F = 0;

	DEBUG_FRAME( "����-��������̣߳����߳���Ŀ=[%d].", pThread_pool->add_thread_num_F );
	pThread_pool->add_threadid_F = (pthread_t *) malloc( (pThread_pool->add_thread_num_F) * sizeof(pthread_t) );
	for( i = 0; i < (pThread_pool->add_thread_num_F); i++ )
	{
		pthread_create( &(pThread_pool->add_threadid_F[i]), NULL, FunctionName_Addwork, FunctionArgv_Addwork );
		DEBUG_FRAME( "����-��������߳�,���[%d]��", i );
	}
	DEBUG_FRAME( "����-��������߳����." );

	DEBUG_FRAME( "����-ִ�й����̣߳����߳���Ŀ=[%d].", pThread_pool->do_thread_num_F );
	pThread_pool->do_threadid_F = (pthread_t *) malloc( (pThread_pool->do_thread_num_F) * sizeof(pthread_t) );
	for( i = 0; i < (pThread_pool->do_thread_num_F); i++ )
	{
		pthread_create( &(pThread_pool->do_threadid_F[i]), NULL, FunctionName_Dowork, FunctionArgv_Dowork );
		DEBUG_FRAME( "����-ִ�й����߳�,���[%d]��", i );
	}
	DEBUG_FRAME( "����-ִ�������߳����." );

	DEBUG_FRAME( "ThreadPool_Init����." );

	return pThread_pool;
}


/*********************************************************************
* Function��                                                         *
*    int threadpool_destroy()                                        *
* Description��                                                      *
*    �㲥�����̣߳�֪ͨ������Ȼ��ȴ������߳��˳�                    *
* In Params:                                                         *
*                                                                    *
* Out Params:                                                        *
*                                                                    *
* Return:                                                            *
*       -1 ��δ���                                                  *
*********************************************************************/
int threadpool_destroy( CThread_pool * pThread_Pool )
{
	int i;

	if ( NULL == pThread_Pool )
	{
		ERROR_FRAME( "���pThread_PoolΪ��!" );
		return -1;
	}

	if ( NULL == pThread_Pool->add_threadid_F || NULL == pThread_Pool->do_threadid_F )
	{
		ERROR_FRAME( "���pThread_Pool->threadidΪ��!" );
		return -1;
	}

	/* �����������Ƿ��������������������̻߳��Զ��˳� */
	if( 1 == pThread_Pool->shutdown_F )
	{
		ERROR_FRAME( "pThread_Pool->shutdown_FΪ1" );
		return -1;
	}
	else if ( pThread_Pool->shutdown_F )
	{
		ERROR_FRAME( "pThread_Pool->shutdown_F��Ϊ1����Ҳ��Ϊ0" );
		return -1;
	}
	pThread_Pool->shutdown_F = 1;

	 /* �㲥�����������̣߳�׼���˳� */
	pthread_cond_broadcast( &(pThread_Pool->queue_ready_F) );  

	for( i = 0; i < pThread_Pool->add_thread_num_F; ++i )
	{
		/* ���̵߳ȴ������߳��˳���ֻ��join��һ����������ָ�룬�ڶ�������������void **������exit�ķ���ֵ����Ҫǿ��ת�� */
		pthread_join( pThread_Pool->add_threadid_F[i], NULL );
	}
	free( pThread_Pool->add_threadid_F );
	pThread_Pool->add_threadid_F = NULL;
	
	for( i = 0; i < pThread_Pool->do_thread_num_F; ++i )
	{
		/* ���̵߳ȴ������߳��˳���ֻ��join��һ����������ָ�룬�ڶ�������������void **������exit�ķ���ֵ����Ҫǿ��ת�� */
		pthread_join( pThread_Pool->do_threadid_F[i], NULL );
	}
	free( pThread_Pool->do_threadid_F );
	pThread_Pool->do_threadid_F = NULL;
	
	CList_Task *head = NULL;
	/* �ͷ�δִ�еĹ�������ʣ���� */
	while(pThread_Pool->queue_head_F != NULL)
	{
		head = pThread_Pool->queue_head_F;
		pThread_Pool->queue_head_F = pThread_Pool->queue_head_F->next;
		free(head);
	}

	/* ���������������� */
	pthread_mutex_destroy( &(pThread_Pool->queue_lock_F) );
	pthread_cond_destroy( &(pThread_Pool->queue_ready_F) );

	/* �ͷ��̳߳ؽṹ */
	free( pThread_Pool );
	pThread_Pool = NULL;

	return 0;
}

/* 
	������
	������ʱֻ���̳߳ص��Ż������������Ҫ��ӹ��ܻ���ʲô�ܹ������ڴ˺���������о��������
*/
void pool_destroy( int iSignal )
{
	/* �̳߳����� */
	threadpool_destroy( pool );
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx��˾                 *
* Source File:    GetString_HHMMSS                          *
* Description:    ��ȡ��ǰʱ��ļ������ʱ�䣨������        *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             pPoint_Log_PathName  ��ȡ����path���ڸ�·����*
*                 pPoint_Log_PathName  �����ڴ治�������δ��֪*
* Out:            char *  ������ַ���                      *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
char * Get_Log_PathName( char * pPoint_Log_PathName )
{
	char  sStr_YYYYMMDD[ 8 + 1 ];               /* YYYYMMDD��ʽ���ַ��� */
	char  sStr_HHMMSS[ 6 + 1 ];                 /* HHMMSS��ʽ���ַ��� */
	char  sStr_ComputerSec[ 20 + 1 ];           /* �����ʱ��-���� */
	char  sFile_PathName[ 256 + 1];             /* log��path���� */

	/* ������ʼ�� */
	memset( sStr_YYYYMMDD, 0x00, sizeof( sStr_YYYYMMDD ) );
	memset( sStr_HHMMSS, 0x00, sizeof( sStr_HHMMSS ) ) ;
	memset( sFile_PathName, 0x00, sizeof( sFile_PathName ) );
	memset( sStr_ComputerSec, 0x00, sizeof( sStr_ComputerSec ) );

	/* ��ȡ��ʱ���йص� */
	GetString_YYYYMMDD( sStr_YYYYMMDD );
	GetString_HHMMSS( sStr_HHMMSS );
	GetString_ComputerSec( sStr_ComputerSec );

	/* log of path name */
	sprintf( pPoint_Log_PathName, "%s%s%s%s%c", "../../log/frame_log.", sStr_YYYYMMDD, sStr_YYYYMMDD, sStr_ComputerSec, '\0' );

	return pPoint_Log_PathName;
}


/*
	���̵߳�ְ��
	��ʼ�������̣߳���ʼ�����������̣߳��ȴ��ر��źţ�֪ͨ�����߳��˳�
*/
int main( int argc, char **argv )
{
	char     sLog_PathName[ 256 + 1];                   /* log��path���� */
	int      iThreadNum_Init_Add_Work;                  /* ���������߳���Ŀ */
	char     sThreadNum_Init_Add_Work[1024];
	int      iThreadNum_Init_Do_Add_Work;               /* ִ�й������߳���Ŀ */
	char     sThreadNum_Init_Do_Add_Work[1024];

	/* ������ʼ�� */
	memset( sLog_PathName, 0x00, sizeof( sLog_PathName ) );
	iThreadNum_Init_Add_Work = 0;
	memset( sThreadNum_Init_Add_Work, 0x00, sizeof( sThreadNum_Init_Add_Work ) );
	iThreadNum_Init_Do_Add_Work = 0;
	memset( sThreadNum_Init_Do_Add_Work, 0x00, sizeof( sThreadNum_Init_Do_Add_Work ) );

	/* ��ȡ��־���ļ���������־�ļ� */
	memset( sLog_PathName, 0x00, sizeof( sLog_PathName ) );
	Get_Log_PathName( sLog_PathName );
	DEBUG_OPEN_FRAME( sLog_PathName );
	DEBUG_FRAME( "��ȡ��־���ļ���������־�ļ�=[%s].", sLog_PathName );

	/* 
	   ��ʼ�������̳߳أ�����ִ��������̣߳���������������߳�
	   ��2�����͵��̵߳���Ŀ���ǿ����û��ģ��������ļ��н�������
	   �����û���Ϊ�Ժ�ľ��������Ľ����߳���Ŀ���Ż��ṩ�˱���
	*/
	/* �������ļ��У���ȡ���������߳���Ŀ */
	memset( sThreadNum_Init_Add_Work, 0x00, sizeof( sThreadNum_Init_Add_Work ) );
	if ( 0 != Get_ValuseString_ByFieldKey_InConfFile( "conf/frame_threadpool.conf", "THREAD_POOL_SET", "SET_ADD_WORK_THREAD_NUM_OF_POOL", sThreadNum_Init_Add_Work ) )
	{
		ERROR_FRAME( "Get_ValuseString_ByFieldKey_InConfFile error" );
		return -1;
	}
	iThreadNum_Init_Add_Work = atoi( sThreadNum_Init_Add_Work );
	DEBUG_FRAME( "�������ļ��У���ȡ���������߳���ĿsThreadNum_Init_Add_Work\
			=[%s],iThreadNum_Init_Add_Work=[%d].", sThreadNum_Init_Add_Work, iThreadNum_Init_Add_Work );

	/* �������ļ��У���ȡִ��������߳���Ŀ */
	memset( sThreadNum_Init_Do_Add_Work, 0x00, sizeof( sThreadNum_Init_Do_Add_Work) );
	if ( 0 != Get_ValuseString_ByFieldKey_InConfFile( "conf/frame_threadpool.conf", "THREAD_POOL_SET", "SET_DO_WORK_THREAD_NUM_OF_POOL", sThreadNum_Init_Do_Add_Work ) )
	{
		ERROR_FRAME( "Get_ValuseString_ByFieldKey_InConfFile error" );
		return -1;
	}
	iThreadNum_Init_Do_Add_Work = atoi( sThreadNum_Init_Do_Add_Work );
	DEBUG_FRAME( "�������ļ��У���ȡִ��������߳���ĿsThreadNum_Init_Do_Add_Work\
			=[%s],iThreadNum_Init_Do_Add_Work=[%d].", sThreadNum_Init_Do_Add_Work, iThreadNum_Init_Do_Add_Work );

	DEBUG_FRAME( "׼����ʼ����ʼ���̳߳أ����ú���pool_init." );
	ThreadPool_Init( &pool, iThreadNum_Init_Add_Work, Call_Addwork_Thread, NULL, 
						iThreadNum_Init_Do_Add_Work, Call_Dowork_Thread, NULL );
	DEBUG_FRAME( "���ú���pool_init����." );

	/* ע���ź�;�ȴ������������̵��źţ����̻߳����У��źŵĽ���Ĭ�������߳������ܵ� */
	signal( SIGKILL, pool_destroy );
	signal( SIGTERM, pool_destroy );

	/* pause()����Ŀǰ�Ľ�����ͣ(����˯��״̬), ֱ�����ź�(signal)���ж� */
	pause();

	DEBUG_FRAME( "���߳��˳�" );

	DEBUG_CLOSE_FRAME();
	return 0;
}


