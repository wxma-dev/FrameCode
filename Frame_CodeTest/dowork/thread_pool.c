#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <pthread.h> 

#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>

#include "log.h"
#include "struct.h"

#define JYM_FUNC_CONF_FILENAME ../etc/jym.conf
#define KEY_SHM_FILENAME ./key_shm_file


/* 日志文件指针 */
FILE* g_pLog_Handle;

/* 线程池结构指针 */
CThread_pool *pool;

extern void *thread_routine_addwork( void *(*process)(void *), void *arg );
extern void *thread_routine_dowork( void *arg );

CThread_pool * threadpool_init( int addwork_thread_num, void *(*addwork_run)(void *), void * argv1, int dowork_thread_num, void *(*dowork_run)(void *), void * argv2 );
CThread_pool * pool_init( int addwork_thread_num, void *(*addwork_run)(void *), int dowork_thread_num, void *(*dowork_run)(void *) );
int threadpool_destroy( CThread_pool * pThread_Pool );
void pool_destroy( int iSignal );

int load_conf_cache()
{
	int iFd, iFd_Conf;
	key_t kKey;
	int iShmID;
	char    read_buf[4096 + 1 ];

	iFd = 0;
	iFd_Conf = 0;
	kKey = 0;
	iShmID = 0;
	memset( read_buf, 0x00, sizeof( read_buf ) );

	/* 打开配置文件 */
	iFd_Conf = open( JYM_FUNC_CONF_FILENAME, O_RDONLY | O_EXCL );
	if ( iFd_Conf < 0 )
	{
		ERROR( "打开配置文件失败" );
		return -1;
	}


	while ( !read( iFd_Conf, read_buf, sizeof( read_buf) -1 ) )
	{
		memset( iFd_Conf, 0x00, sizeof( iFd_Conf ) );
	}

	/* 创建文件 */
	iFd = open( KEY_SHM_FILENAME, O_CREAT, 0666 );
	if ( iFd < 0 )  
	{
		ERROR( "创建文件失败" );
		return -1;  
	}
	close( iFd );
	
	/* 创建key */
	kKey = ftok( KEY_SHM_FILENAME, 0 );
	if ( kKey < 0 )
	{
		ERROR( "创建key失败" );
		return -1;
	}

	/* 创建共享内存 */
	iShmID = shmget( kKey, sizeof(int), IPC_CREAT | 0666 );
	if ( iShmID < 0 )
	{
		ERROR( "创建共享内存失败" );
		return -1;
	}
	
	/* 链接共享内存 */
	
	
	/* 使用工程内存
}


/*
	主线程的职责：
	初始化工作线程，初始化添加任务的线程，等待关闭信号，通知所有线程退出
*/
int main( int argc, char **argv )
{
	DEBUG_OPEN( "./applog.20160925" );

	DEBUG( "主线程开始工作" );

	
	/* 加载配置文件 */
	


	/* 初始化工作线程池 */
	if ( NULL == pool_init( THREADPOOL_ADDWORK_INIT_NUM, thread_routine_addwork, THREADPOOL_DOWORK_INIT_NUM, thread_routine_dowork ) )
	{
		ERROR( "初始化线程池失败." );	
	}
	DEBUG( "wait thread run 3s" );
	sleep( 3 );

	DEBUG( "signal" );
	/* 注册信号;等待结束整个进程的信号，多线程环境中，信号的接受默认由主线程来接受的 */
	signal( SIGKILL, pool_destroy );
	signal( SIGTERM, pool_destroy );
	signal( SIGINT, pool_destroy );

	DEBUG( "pause" );
	/* 阻塞 */
	pause();
	
	DEBUG( "主线程退出" );

	DEBUG_CLOSE();
	return 0;
}

/*********************************************************************
* Function：                                                         *
*    CThread_pool * threadpool_init( int addwork_thread_num, (void *)(*addwork_run)(void *), int dowork_thread_num, (void *)(*dowork_run)(void *) ) *
* Description：                                                      *
*    线程池初始化,创建添加任务线程,创建工作线程,初始化线程池结构     *
* In Params:                                                         *
* Out Params:                                                        *
*                                                                    *
* Return:                                                            *
*    非NULL 线程池结构地址                                           *
*    NULL   失败                                                     *
*********************************************************************/
CThread_pool * threadpool_init( int addwork_thread_num, void *(*addwork_run)(void *), void * argv1, int dowork_thread_num, void *(*dowork_run)(void *), void * argv2 )
{
	int i = 0;
	int thread_num = 0;

	DEBUG( "线程池初始化开始" );
	if ( NULL == addwork_run )
	{
		ERROR( "添加任务函数不能空!" );
		return NULL;
	}
	else if ( NULL == dowork_run)
	{
		ERROR( "工作函数不为空!" );
		return NULL;
	}
	
	if ( 0 == addwork_thread_num )
	{
		ERROR( "添加任务线程的数目不能为0!" );
		return NULL;
	}
	else if ( 0 == addwork_thread_num )
	{
		ERROR( "工作线程的数目不能为0!" );
		return NULL;
	}
	
	thread_num = addwork_thread_num + dowork_thread_num;

	pool = (CThread_pool *) malloc( sizeof( CThread_pool ) ); 

	/* 互斥锁初始化，参数为锁的地址 */
	pthread_mutex_init( &(pool->queue_lock), NULL );
	
	/* 条件变量初始化，参数为变量地址 */
	pthread_cond_init( &(pool->queue_ready), NULL );

	pool->queue_head = NULL;
	pool->cur_queue_size = 0;
	pool->shutdown = 0;
	pool->max_thread_num = thread_num;
	pool->addwork_threadnum = addwork_thread_num;
	pool->dowork_threadnum = dowork_thread_num;
	pool->addwork_threadid = (pthread_t *) malloc( addwork_thread_num * sizeof(pthread_t) );
	pool->dowork_threadid = (pthread_t *) malloc( dowork_thread_num * sizeof(pthread_t) );

	for( i = 0; i < addwork_thread_num; i++ )
	{
		pthread_create( &(pool->addwork_threadid[i]), NULL, addwork_run, argv1 );
		DEBUG( "启动添加任务线程,序号[%d]。", i );
	}

	for( i = 0; i < dowork_thread_num; i++ )
	{
		pthread_create( &(pool->dowork_threadid[i]), NULL, dowork_run, argv2 );
		DEBUG( "启动工作线程,序号[%d]。", i );
	}
	
	DEBUG( "线程池初始化结束" );
	return pool;
}


CThread_pool * pool_init( int addwork_thread_num, void *(*addwork_run)(void *), int dowork_thread_num, void *(*dowork_run)(void *) )
{
	return threadpool_init( addwork_thread_num, addwork_run, NULL, dowork_thread_num, dowork_run, NULL );
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
	int i,j;
	int iRet;

	if ( NULL == pThread_Pool )
	{
		ERROR( "入参pThread_Pool为空!" );
		return -1;
	}

	if ( NULL == pThread_Pool->addwork_threadid )
	{
		ERROR( "入参pThread_Pool->addwork_threadid为空!" );
		return -1;
	}

	if ( NULL == pThread_Pool->dowork_threadid )
	{
		ERROR( "入参pThread_Pool->dowork_threadid为空!" );
		return -1;
	}

	/* 检测结束开关是否开启，若开启，则所有线程会自动退出 */
	if( 1 == pThread_Pool->shutdown )
	{
		ERROR( "pThread_Pool->shutdown为1" );
		return -1;
	}
	else if ( pThread_Pool->shutdown )
	{
		ERROR( "pThread_Pool->shutdown不为1，且也不为0" );
		return -1;
	}
	
	INFO( "start destroy threads" );

	pThread_Pool->shutdown = 1;

	DEBUG( "广播，唤醒所有工作线程，准备退出" );
	pthread_cond_broadcast( &(pThread_Pool->queue_ready) );  

	DEBUG( "线程池中的工作线程退出" );
	for( i = 0; i < pThread_Pool->dowork_threadnum; ++i )
	{
		/* 主线程等待所有线程退出，只有join第一个参数不是指针，第二个参数类型是void **，接收exit的返回值，需要强制转换 */
		iRet = pthread_join( pThread_Pool->dowork_threadid[i], NULL );
		DEBUG( "i=[%d],iRet=[%d].", i, iRet );
	}
	free( pThread_Pool->dowork_threadid );
	pThread_Pool->dowork_threadid = NULL;

	DEBUG( "发送信号给添加任务线程" );
	for( j = 0; j < pThread_Pool->addwork_threadnum; ++j )
	{
		iRet =  pthread_kill( pThread_Pool->addwork_threadid[ j ], SIGUSR1 );
		DEBUG( "iRet of phread_kill is [%d].", iRet );
	}

	DEBUG( "线程池中的添加任务线程退出" );
	for( j = 0; j < pThread_Pool->addwork_threadnum; ++j )
	{
		pthread_join( pThread_Pool->addwork_threadid[ j ], NULL );
		DEBUG( "j=[%d],iRet=[%d].", i, iRet );
	}

	free( pThread_Pool->addwork_threadid );
	pThread_Pool->addwork_threadid = NULL;
	
	CThread_worker *head = NULL;
	/* 释放未执行的工作链表剩余结点 */
	while(pThread_Pool->queue_head != NULL)
	{
		head = pThread_Pool->queue_head;
		pThread_Pool->queue_head = pThread_Pool->queue_head->next;
		free(head);
	}

	/* 销毁锁和条件变量 */
	pthread_mutex_destroy( &(pThread_Pool->queue_lock) );
	pthread_cond_destroy( &(pThread_Pool->queue_ready) );

	/* 释放线程池结构 */
	free( pThread_Pool );
	pThread_Pool = NULL;

	INFO( "end destroy threads" );

	return 0;
}


void pool_destroy( int iSignal )
{
	INFO( "start pool_destroy" );
	threadpool_destroy( pool );
	INFO( "end pool_destroy" );
}


