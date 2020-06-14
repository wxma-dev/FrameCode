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
/* 线程池结构指针 */
CThread_pool *pool;

/*********************************************************************
* Function：                                                         *
*    CThread_pool * ThreadPool_Init( int addwork_thread_num, (void *)(*FunctionName_Addwork)(void *), int dowork_thread_num, (void *)(*FunctionName_Dowork)(void *) ) *
* Description：                                                      *
*    线程池初始化,创建添加任务线程,创建工作线程,初始化线程池结构     *
* In Params:                                                         *
* Out Params:                                                        *
*                                                                    *
* Return:                                                            *
*    非NULL 线程池结构地址                                           *
*    NULL   失败                                                     *
*********************************************************************/
CThread_pool * ThreadPool_Init( CThread_pool **pPoint_CThread_pool, int addwork_thread_num, void *(*FunctionName_Addwork)(void *), void * FunctionArgv_Addwork, int dowork_thread_num, void *(*FunctionName_Dowork)(void *), void * FunctionArgv_Dowork )
{
	int     iRet = 0;
	int     i = 0;
	int     thread_num = 0;
	CThread_pool * pThread_pool;

	DEBUG_FRAME( "ThreadPool_Init开始." );

	if ( NULL == FunctionName_Addwork )
	{
		ERROR_FRAME( "添加任务函数不能空!" );
		return NULL;
	}
	else if ( NULL == FunctionName_Dowork )
	{
		ERROR_FRAME( "工作函数不为空!" );
		return NULL;
	}

	if ( 0 == addwork_thread_num )
	{
		ERROR_FRAME( "添加任务线程的数目不能为0!" );
		return NULL;
	}
	else if ( 0 == addwork_thread_num )
	{
		ERROR_FRAME( "工作线程的数目不能为0!" );
		return NULL;
	}

	DEBUG_FRAME( "分配CThread_pool的空间" );
	pThread_pool = (CThread_pool *) malloc( sizeof( CThread_pool ) ); 

	/* 给全局的该变量赋值 */
	*pPoint_CThread_pool = pThread_pool;

	/* 互斥锁初始化，参数为锁的地址 */
	DEBUG_FRAME( "初始化互斥锁CThread_pool->queue_lock_F" );
	iRet = pthread_mutex_init( &(pThread_pool->queue_lock_F), NULL );
	if ( 0 != iRet )
	{
		ERROR_FRAME( "初始化互斥锁CThread_pool->queue_lock_F失败." );
		return NULL;
	}

	/* 条件变量初始化，参数为变量地址 */
	DEBUG_FRAME( "初始化条件变量CThread_pool->queue_ready_F" );
	iRet = pthread_cond_init( &(pThread_pool->queue_ready_F), NULL );
	if ( 0 != iRet )
	{
		ERROR_FRAME( "初始化条件变量CThread_pool->queue_ready_F." );
		return NULL;
	}

	pThread_pool->add_thread_num_F = addwork_thread_num;
	pThread_pool->do_thread_num_F = dowork_thread_num;
	pThread_pool->max_thread_num_F = addwork_thread_num + dowork_thread_num;;
	pThread_pool->queue_head_F = NULL;
	pThread_pool->cur_queue_size_F = 0;
	pThread_pool->shutdown_F = 0;

	DEBUG_FRAME( "启动-添加任务线程，有线程数目=[%d].", pThread_pool->add_thread_num_F );
	pThread_pool->add_threadid_F = (pthread_t *) malloc( (pThread_pool->add_thread_num_F) * sizeof(pthread_t) );
	for( i = 0; i < (pThread_pool->add_thread_num_F); i++ )
	{
		pthread_create( &(pThread_pool->add_threadid_F[i]), NULL, FunctionName_Addwork, FunctionArgv_Addwork );
		DEBUG_FRAME( "启动-添加任务线程,序号[%d]。", i );
	}
	DEBUG_FRAME( "启动-添加任务线程完成." );

	DEBUG_FRAME( "启动-执行工作线程，有线程数目=[%d].", pThread_pool->do_thread_num_F );
	pThread_pool->do_threadid_F = (pthread_t *) malloc( (pThread_pool->do_thread_num_F) * sizeof(pthread_t) );
	for( i = 0; i < (pThread_pool->do_thread_num_F); i++ )
	{
		pthread_create( &(pThread_pool->do_threadid_F[i]), NULL, FunctionName_Dowork, FunctionArgv_Dowork );
		DEBUG_FRAME( "启动-执行工作线程,序号[%d]。", i );
	}
	DEBUG_FRAME( "启动-执行任务线程完成." );

	DEBUG_FRAME( "ThreadPool_Init结束." );

	return pThread_pool;
}


/*********************************************************************
* Function：                                                         *
*    int threadpool_destroy()                                        *
* Description：                                                      *
*    广播所有线程，通知结束，然后等待所有线程退出                    *
* In Params:                                                         *
*                                                                    *
* Out Params:                                                        *
*                                                                    *
* Return:                                                            *
*       -1 入参错误                                                  *
*********************************************************************/
int threadpool_destroy( CThread_pool * pThread_Pool )
{
	int i;

	if ( NULL == pThread_Pool )
	{
		ERROR_FRAME( "入参pThread_Pool为空!" );
		return -1;
	}

	if ( NULL == pThread_Pool->add_threadid_F || NULL == pThread_Pool->do_threadid_F )
	{
		ERROR_FRAME( "入参pThread_Pool->threadid为空!" );
		return -1;
	}

	/* 检测结束开关是否开启，若开启，则所有线程会自动退出 */
	if( 1 == pThread_Pool->shutdown_F )
	{
		ERROR_FRAME( "pThread_Pool->shutdown_F为1" );
		return -1;
	}
	else if ( pThread_Pool->shutdown_F )
	{
		ERROR_FRAME( "pThread_Pool->shutdown_F不为1，且也不为0" );
		return -1;
	}
	pThread_Pool->shutdown_F = 1;

	 /* 广播，唤醒所有线程，准备退出 */
	pthread_cond_broadcast( &(pThread_Pool->queue_ready_F) );  

	for( i = 0; i < pThread_Pool->add_thread_num_F; ++i )
	{
		/* 主线程等待所有线程退出，只有join第一个参数不是指针，第二个参数类型是void **，接收exit的返回值，需要强制转换 */
		pthread_join( pThread_Pool->add_threadid_F[i], NULL );
	}
	free( pThread_Pool->add_threadid_F );
	pThread_Pool->add_threadid_F = NULL;
	
	for( i = 0; i < pThread_Pool->do_thread_num_F; ++i )
	{
		/* 主线程等待所有线程退出，只有join第一个参数不是指针，第二个参数类型是void **，接收exit的返回值，需要强制转换 */
		pthread_join( pThread_Pool->do_threadid_F[i], NULL );
	}
	free( pThread_Pool->do_threadid_F );
	pThread_Pool->do_threadid_F = NULL;
	
	CList_Task *head = NULL;
	/* 释放未执行的工作链表剩余结点 */
	while(pThread_Pool->queue_head_F != NULL)
	{
		head = pThread_Pool->queue_head_F;
		pThread_Pool->queue_head_F = pThread_Pool->queue_head_F->next;
		free(head);
	}

	/* 销毁锁和条件变量 */
	pthread_mutex_destroy( &(pThread_Pool->queue_lock_F) );
	pthread_cond_destroy( &(pThread_Pool->queue_ready_F) );

	/* 释放线程池结构 */
	free( pThread_Pool );
	pThread_Pool = NULL;

	return 0;
}

/* 
	池销毁
	现在暂时只有线程池的优化，后续如果需要添加功能或者什么架构，可在此函数里面进行具体的销毁
*/
void pool_destroy( int iSignal )
{
	/* 线程池销毁 */
	threadpool_destroy( pool );
}


/************************************************************
* Copyright (c) 2015 xxxxxxxxxxxxxxxxxx公司                 *
* Source File:    GetString_HHMMSS                          *
* Description:    获取当前时间的计算机的时间（秒数）        *
* Author:         mawx                                      *
* Create Date:    2015/07/09                                *
* In:             pPoint_Log_PathName  获取到的path放在该路径中*
*                 pPoint_Log_PathName  若是内存不够，结果未可知*
* Out:            char *  结果的字符串                      *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:       XXX                                       *
* Modify Note:    XXXXXXXXXXXXXX                            *
************************************************************/
char * Get_Log_PathName( char * pPoint_Log_PathName )
{
	char  sStr_YYYYMMDD[ 8 + 1 ];               /* YYYYMMDD格式的字符串 */
	char  sStr_HHMMSS[ 6 + 1 ];                 /* HHMMSS格式的字符串 */
	char  sStr_ComputerSec[ 20 + 1 ];           /* 计算机时间-秒数 */
	char  sFile_PathName[ 256 + 1];             /* log的path名字 */

	/* 变量初始化 */
	memset( sStr_YYYYMMDD, 0x00, sizeof( sStr_YYYYMMDD ) );
	memset( sStr_HHMMSS, 0x00, sizeof( sStr_HHMMSS ) ) ;
	memset( sFile_PathName, 0x00, sizeof( sFile_PathName ) );
	memset( sStr_ComputerSec, 0x00, sizeof( sStr_ComputerSec ) );

	/* 获取跟时间有关的 */
	GetString_YYYYMMDD( sStr_YYYYMMDD );
	GetString_HHMMSS( sStr_HHMMSS );
	GetString_ComputerSec( sStr_ComputerSec );

	/* log of path name */
	sprintf( pPoint_Log_PathName, "%s%s%s%s%c", "../../log/frame_log.", sStr_YYYYMMDD, sStr_YYYYMMDD, sStr_ComputerSec, '\0' );

	return pPoint_Log_PathName;
}


/*
	主线程的职责：
	初始化工作线程，初始化添加任务的线程，等待关闭信号，通知所有线程退出
*/
int main( int argc, char **argv )
{
	char     sLog_PathName[ 256 + 1];                   /* log的path名字 */
	int      iThreadNum_Init_Add_Work;                  /* 添加任务的线程数目 */
	char     sThreadNum_Init_Add_Work[1024];
	int      iThreadNum_Init_Do_Add_Work;               /* 执行工作的线程数目 */
	char     sThreadNum_Init_Do_Add_Work[1024];

	/* 变量初始化 */
	memset( sLog_PathName, 0x00, sizeof( sLog_PathName ) );
	iThreadNum_Init_Add_Work = 0;
	memset( sThreadNum_Init_Add_Work, 0x00, sizeof( sThreadNum_Init_Add_Work ) );
	iThreadNum_Init_Do_Add_Work = 0;
	memset( sThreadNum_Init_Do_Add_Work, 0x00, sizeof( sThreadNum_Init_Do_Add_Work ) );

	/* 获取日志的文件名、打开日志文件 */
	memset( sLog_PathName, 0x00, sizeof( sLog_PathName ) );
	Get_Log_PathName( sLog_PathName );
	DEBUG_OPEN_FRAME( sLog_PathName );
	DEBUG_FRAME( "获取日志的文件名、打开日志文件=[%s].", sLog_PathName );

	/* 
	   初始化工作线程池，创建执行任务的线程，创建创建任务的线程
	   这2个类型的线程的数目都是可配置化的，在配置文件中进行配置
	   可配置化，为以后的具体的任务的进行线程数目的优化提供了便利
	*/
	/* 从配置文件中，获取添加任务的线程数目 */
	memset( sThreadNum_Init_Add_Work, 0x00, sizeof( sThreadNum_Init_Add_Work ) );
	if ( 0 != Get_ValuseString_ByFieldKey_InConfFile( "conf/frame_threadpool.conf", "THREAD_POOL_SET", "SET_ADD_WORK_THREAD_NUM_OF_POOL", sThreadNum_Init_Add_Work ) )
	{
		ERROR_FRAME( "Get_ValuseString_ByFieldKey_InConfFile error" );
		return -1;
	}
	iThreadNum_Init_Add_Work = atoi( sThreadNum_Init_Add_Work );
	DEBUG_FRAME( "从配置文件中，获取添加任务的线程数目sThreadNum_Init_Add_Work\
			=[%s],iThreadNum_Init_Add_Work=[%d].", sThreadNum_Init_Add_Work, iThreadNum_Init_Add_Work );

	/* 从配置文件中，获取执行任务的线程数目 */
	memset( sThreadNum_Init_Do_Add_Work, 0x00, sizeof( sThreadNum_Init_Do_Add_Work) );
	if ( 0 != Get_ValuseString_ByFieldKey_InConfFile( "conf/frame_threadpool.conf", "THREAD_POOL_SET", "SET_DO_WORK_THREAD_NUM_OF_POOL", sThreadNum_Init_Do_Add_Work ) )
	{
		ERROR_FRAME( "Get_ValuseString_ByFieldKey_InConfFile error" );
		return -1;
	}
	iThreadNum_Init_Do_Add_Work = atoi( sThreadNum_Init_Do_Add_Work );
	DEBUG_FRAME( "从配置文件中，获取执行任务的线程数目sThreadNum_Init_Do_Add_Work\
			=[%s],iThreadNum_Init_Do_Add_Work=[%d].", sThreadNum_Init_Do_Add_Work, iThreadNum_Init_Do_Add_Work );

	DEBUG_FRAME( "准备开始除初始化线程池，调用函数pool_init." );
	ThreadPool_Init( &pool, iThreadNum_Init_Add_Work, Call_Addwork_Thread, NULL, 
						iThreadNum_Init_Do_Add_Work, Call_Dowork_Thread, NULL );
	DEBUG_FRAME( "调用函数pool_init结束." );

	/* 注册信号;等待结束整个进程的信号，多线程环境中，信号的接受默认由主线程来接受的 */
	signal( SIGKILL, pool_destroy );
	signal( SIGTERM, pool_destroy );

	/* pause()会令目前的进程暂停(进入睡眠状态), 直到被信号(signal)所中断 */
	pause();

	DEBUG_FRAME( "主线程退出" );

	DEBUG_CLOSE_FRAME();
	return 0;
}


