/*************************************************************
*    FileName : MAIN.c                                       *
*    FileFunc : 创建各子进程并监控子进程状态，实时重启各子进程*
*    Version: V0.1                                           *
*    Author :                                                *
*    Date : 2016-12-25                                       *
*    Descp :                                                 *
*************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <errno.h>
#include <error.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "log.h"

#include "Lib/lib_string.h"
#include "Lib/lib_file.h"
#include "Lib/lib_conf_file.h"
#include "Lib/lib_time.h"

#include "Monitor_ChildProcess.h"

#define CONF_FULLPATHNAME "./info.conf"

ST_Frame *pSt_Frame;

/************************************************************
* Function Name: func_waitpid                               *
* Description:   获取发送SIGCHLD信号的子进程的pid           *
* In Params:                                                *
* Out Params:                                               *
*                                                           *
* Return:        void                                       *
* explain:                                                  *
************************************************************/
void func_waitpid( int signo )
{
	pid_t pid = 0;
	int stat = 0;

	while ( 0 < (pid = waitpid(-1, &stat, WNOHANG)) ) /* 获取发送SIGCHLD信号的子进程的详细信息 */
	{
		TEST_DEBUG( "监控到子进程pid=[%d]退出.", pid );
		/* 某一个子进程退出，需要在链表中将状态职位2 */
		struct list_head list;
		struct list_head *pPos;
		ST_ChildProcess *pPoint_St_ChildProcess;

		memset( &list, 0x00, sizeof( list ) );
		pPos = NULL;
		pPoint_St_ChildProcess = NULL;

		/* 遍历 */
		list_for_each( pPos, &((pSt_Frame->plist_head_F)->list) )
		{
			pPoint_St_ChildProcess = list_entry( pPos, ST_ChildProcess, list );
			if ( pid == pPoint_St_ChildProcess->iPid )
			{
				pPoint_St_ChildProcess->iPocessStatus = 2;
				TEST_DEBUG( "监控:标记子进程pid=[%d]的状态为2.", pPoint_St_ChildProcess->iPid );
			}
		}
	}
}

/************************************************************
* Function Name: sig_func                                   *
* Description:   信号注册                                   *
* In Params:                                                *
* Out Params:                                               *
*                                                           *
* Return:        void                                       *
* explain:                                                  *
************************************************************/
void sig_func( int signo )
{
	if ( signo == SIGCHLD )
	{
		/* SIGCHLD信号，子进程退出的时候都会向父进程发送该信号 */
		func_waitpid( signo );
	}
}


/*
	本框架的初始化
	(1)ST_Frame结构体的初始化
	(2)信号函数注册
*/
int Init_Frame( ST_Frame * pPoint_StFrame, char * sConfFullPathName )
{
	int    iRet;

	iRet = 0;

	/* ST_Frame结构体的初始化 */
	memset( pPoint_StFrame->sFullPathName_F, 0x00, sizeof( pPoint_StFrame->sFullPathName_F ) );
	strcpy( pPoint_StFrame->sFullPathName_F, sConfFullPathName );
	pPoint_StFrame->plist_head_F = (ST_ChildProcess *)malloc( sizeof( ST_ChildProcess ) );
	pPoint_StFrame->cur_list_size_F = 0;

	/* 初始化链表 */
	INIT_LIST_HEAD( &((pPoint_StFrame->plist_head_F)->list) );

	/* 信号函数注册 */
	signal( SIGCHLD, &sig_func );

	return 0;
}


/*
	将某个节点添加到链表中尾部
*/
void InsertNode_To_List( ST_ChildProcess * pPoint_StChildProcess )
{
	/* 插入链尾部 */
	list_add_tail( &(pPoint_StChildProcess->list), &((pSt_Frame->plist_head_F)->list) );
}



/* 根据配置文件，将子进程的信息插入到链表的尾部 */
int Insert_ChildProcessInfo_List( ST_Frame * pSt_Frame )
{
	ST_ReadLine * p_St_ReadLine;
	ST_ReadLine * p_St_ReadLine_Tmp;
	char    sStr_Between_Zkh[4096 + 1];
	int     iFlag_Zkh;                         /* 是否遇到中括号的标识，1-遇到，0-没有遇到 */
	ST_ChildProcess * pPoint_StChildProcess;
	ST_ChildProcess * pPoint_StChildProcess_Tmp;
	char    sStr_Key[ 4096 + 1 ];
	char    sStr_Value[ 4096 + 1 ];
	int     iProcessnum;
	int     iFor;

	p_St_ReadLine = NULL;
	p_St_ReadLine_Tmp = NULL;
	memset( sStr_Between_Zkh, 0x00, sizeof( sStr_Between_Zkh ) );
	iFlag_Zkh = 0;
	pPoint_StChildProcess = NULL;
	pPoint_StChildProcess_Tmp = NULL;
	memset( sStr_Key, 0x00, sizeof( sStr_Key ) );
	memset( sStr_Value, 0x00, sizeof( sStr_Value ) );
	iProcessnum = 0;
	iFor = 0;

	p_St_ReadLine = ( ST_ReadLine *) malloc( sizeof( ST_ReadLine ) );
	if( NULL == p_St_ReadLine )
	{
		return -1;
	}
	memset( p_St_ReadLine, 0x00, sizeof( ST_ReadLine ) );

	strcpy( p_St_ReadLine->sStr_FilePathName, pSt_Frame->sFullPathName_F );

	while( 1 )
	{
		if ( NULL != Wile_ReadLine_FromConfFile( &p_St_ReadLine ) )
		{
			memset( sStr_Between_Zkh, 0x00, sizeof( sStr_Between_Zkh ) );
			if ( NULL == Get_String_Between_Zkh( p_St_ReadLine->sStr_Line, sStr_Between_Zkh ) )
			{
				/*
					没有中括号，则获取到key-value的内容，往指定的内存区域添值;
					若没有获取到key-value的内容，则continue
				*/

				/* 获取节点的值 */
				memset( sStr_Key, 0x00, sizeof( sStr_Key ) );
				memset( sStr_Value, 0x00, sizeof( sStr_Value ) );
				if ( Get_KeyValue_Dh_QdKg( p_St_ReadLine->sStr_Line, sStr_Key, sStr_Value ) )
				{
					/* 复制 */
					if ( 0 == strncmp( sStr_Key, "PROCESSNUM", strlen( sStr_Key ) ) )
					{
						iProcessnum = atoi( sStr_Value );
					}
					else if ( 0 == strncmp( sStr_Key, "BINPATHNAME", strlen( sStr_Key ) ) )
					{
						strcpy( pPoint_StChildProcess->sBin_PathName, sStr_Value );
					}
				}
				else
				{
					/* 没有=号则到此分支 */
					continue;
				}
			}
			else
			{
				/*
					遇到中括号，则插入链表，负责内存的分配初始化
				*/

				/* 再次遇见中括号，则认为需要将上一次的子进程的信息插入list */
				for( iFor = 0; iFor < iProcessnum; iFor++ )
				{
					if ( 0 == iFor )
					{
						/* 第一次插入，因为已经有分配好的子进程结构体内存了 */
						InsertNode_To_List( pPoint_StChildProcess );
					}
					else
					{
						/* 第二次以上插入，只能再次复制一个同样的内存，然后再插入 */
						pPoint_StChildProcess_Tmp = NULL;
						pPoint_StChildProcess_Tmp = ( ST_ChildProcess *) malloc( sizeof( ST_ChildProcess ) );
						memcpy( pPoint_StChildProcess_Tmp, pPoint_StChildProcess, sizeof( ST_ChildProcess ) );
						InsertNode_To_List( pPoint_StChildProcess_Tmp );
					}
				}
				iProcessnum = 0;

				{
					/* 分配内存 */
					pPoint_StChildProcess = ( ST_ChildProcess * )malloc( sizeof( ST_ChildProcess ) );

					/* 内存初始化 */
					memset( pPoint_StChildProcess, 0x00, sizeof( ST_ChildProcess ) );
					pPoint_StChildProcess->iPocessStatus = 0;
					strcpy( pPoint_StChildProcess->sProcess_Label, sStr_Between_Zkh );
				}
			}
		}
		else
		{
			/* 最后一次插入链表中 */
			if ( NULL != pPoint_StChildProcess )
			{
				/* 再次遇见中括号，则认为需要将上一次的子进程的信息插入list */
				for( iFor = 0; iFor < iProcessnum; iFor++ )
				{
					if ( 0 == iFor )
					{
						/* 第一次插入，因为已经有分配好的子进程结构体内存了 */
						InsertNode_To_List( pPoint_StChildProcess );
					}
					else
					{
						/* 第二次以上插入，只能再次复制一个同样的内存，然后再插入 */
						pPoint_StChildProcess_Tmp = NULL;
						pPoint_StChildProcess_Tmp = ( ST_ChildProcess *) malloc( sizeof( ST_ChildProcess ) );
						memcpy( pPoint_StChildProcess_Tmp, pPoint_StChildProcess, sizeof( ST_ChildProcess ) );
						InsertNode_To_List( pPoint_StChildProcess_Tmp );
					}
				}
			}

			break;
		}
	}

	return 0;
}


/* 循环遍历链表，检查自进程状态，进行维护 */
/*
-1 失败
0 成功
*/
int Fix_AllChildProcess_ByList( ST_Frame * pSt_Frame )
{
	struct list_head list;
	struct list_head *pPos;
	ST_ChildProcess *pPoint_St_ChildProcess;
	int  iRet;

	memset( &list, 0x00, sizeof( list ) );
	pPos = NULL;
	pPoint_St_ChildProcess = NULL;
	iRet = 0;

	/* 遍历 */
	list_for_each( pPos, &((pSt_Frame->plist_head_F)->list) )
	{
		pPoint_St_ChildProcess = list_entry( pPos, ST_ChildProcess, list );

		if ( 1 == pPoint_St_ChildProcess->iPocessStatus )
		{
			/* do nothing */
		}
		else if ( 2 == pPoint_St_ChildProcess->iPocessStatus || 0 == pPoint_St_ChildProcess->iPocessStatus )
		{
			if ( 2 == pPoint_St_ChildProcess->iPocessStatus )
			{
				TEST_DEBUG( "某子进程pid=[%d]的状态为iPocessStatus=[%d]即将启动", 
					pPoint_St_ChildProcess->iPid, pPoint_St_ChildProcess->iPocessStatus );
			}
			else if ( 0 == pPoint_St_ChildProcess->iPocessStatus )
			{
				TEST_DEBUG( "子进程iPocessStatus=[%d]未启动，即将启动", 
					pPoint_St_ChildProcess->iPocessStatus );
			}

			/* 进行启动 */
			iRet = fork();
			if ( 0 > iRet )
			{
				return -1;
			}
			/* 子进程 */
			else if ( 0 == iRet )
			{
				//execl( pPoint_St_ChildProcess->sBin_PathName, pPoint_St_ChildProcess->sBin_PathName, NULL );
				execl( "/usr/bin/cat", "cat", NULL );

				/* 子进程正常执行则不会执行下面语句 */
				TEST_DEBUG( "子进程pid=[%d]执行失败，将要退出.", getpid() );
				exit( -1 );
			}
			/* 父进程 */
			else
			{
				pPoint_St_ChildProcess->iPid = iRet;               /* 子进程id */
				pPoint_St_ChildProcess->iPocessStatus = 1;         /* 子进程的状态 */

				TEST_DEBUG( "父进程id=[%d]启动了子进程id=[%d]成功,标记子进程iPocessStatus=[%d].",
					getpid(),  pPoint_St_ChildProcess->iPid, pPoint_St_ChildProcess->iPocessStatus );
			}
		}
	}

	return 0;
}


/*
	日志的初始化、开启
*/
int Init_Open_Log()
{
	char sBasePath[ 512 +1 ];
	char sStrTime[ 4 + 1 ];
	char sStrDate[ 6 + 1 ];
	char sFileLogName[ 512 + 1 ];
	char sFile[ 512 + 1 ];

	memset( sBasePath, 0x00, sizeof(sBasePath) );
	memset( sStrTime, 0x00, sizeof(sStrTime) );
	memset( sStrDate, 0x00, sizeof(sStrDate) );
	memset( sFileLogName, 0x00, sizeof(sFileLogName) );
	memset( sFile, 0x00, sizeof(sFile) );

	strcpy( sBasePath, "." );

	/* 获取字符格式的时间 */
	if ( 0 != getstrtime( sStrTime ) )
	{
		return -1;
	}

	/* 获取字符串个数的日期 */
	if ( 0 != getstrdate( sStrDate ) )
	{
		return -1;
	}

	/* pltlog日志文件名, main.日期-时间-进程号 */
	sprintf( sFileLogName, "%s%s%s%s%s%06d", "main.", sStrDate, "-", sStrTime, "-", getpid() );

	/* pltlog日志路径名 */
	strcpy( sFile, "./");
	strcpy( sFile, sFileLogName );

	TEST_LOG_INIT();
	TEST_DEBUG_OPEN( sFile );
}


/* 日志关闭 */
void Close_Log()
{
	TEST_DEBUG_CLOSE();
}


/************************************************************
* Function Name: main                                       *
* Description:   监控主进程：                               *
*                1.启动Manger程序，创建各业务子进程；       *
*                2.实时监控子进程的状态，实时重启业务子进程 *
* In Params:                                                *
* Out Params:                                               *
*                                                           *
* Return:        0: 成功                                    *
*               -1: 失败                                    *
* explain:                                                  *
************************************************************/
int main( int argc, char * argv[] ) 
{
	int    iRet;

	iRet = 0;

	Init_Open_Log();

	/* 分配STFrame内存 */
	pSt_Frame = ( ST_Frame * )malloc( sizeof( ST_Frame ) );
	memset( pSt_Frame, 0x00, sizeof(  ST_Frame ) );

	/* 分配STFrame内存，并初始化 */
	iRet = Init_Frame( pSt_Frame, CONF_FULLPATHNAME );
	if ( 0 != iRet )
	{
		TEST_DEBUG( "Init_Frame error!" );
		return -1;
	}
	TEST_DEBUG( "Init_Frame ok." );

	/* 根据配置文件，将子进程的信息插入到链表中 */
	iRet = Insert_ChildProcessInfo_List( pSt_Frame );
	if ( 0 != iRet )
	{
		TEST_DEBUG( "Insert_ChildProcessInfo_List error!" );
	}

	while( 1 )
	{
		/* 循环遍历链表，检查子进程状态，进行维护 */
		iRet = Fix_AllChildProcess_ByList( pSt_Frame );
		if ( 0 != iRet )
		{
			TEST_DEBUG( "Fix_AllChildProcess_ByList error!" );
		}

		/* 间隔监控时间 */
		sleep( 1 );
	}

	Close_Log();

	return 0;
}


/*


*/