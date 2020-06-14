/************************************************************
* Copyright (c)                                             *
* Source File:    Application.c                             *
* Description:    应用开发源文件                            *
*           应用层的代码，均在此代码中实现                  *
*           (1)分配内存，将该内存的地址插入队列             *
*           (2)得到内存的地址，进行解析，执行任务           *
* Author:         mawx                                      *
* Create Date:    2017.06.30                                *
*-----------------------------------------------------------*
* Modify Date:    YYYY-MM-DD                                *
* Modifier:                                                 *
* Modify Note:    XXXXXXXXXXXXXX                            *
* **********************************************************/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#include "../Common.h"

#include "Threads_Socket_Client.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>



/*
	线程池的框架的应用部分，在该文件中编写其功效。
	角色：
		生产者：__REPLACE__AddWork 这个函数中添加任务,malloc(new) 的方式将内容放入其中；
			生产者线程的数量可以为0，直接启动多个线程 执行任务。
		消费者：__REPLACE__DoWork  这个函数中执行任务,直接拿到对应的内存，执行任务，再free(delete)内存
		需要一个 上述2个函数，都认可的 任务结构体，需自行定义
*/

#include  <sys/types.h>  
#include <sys/epoll.h> 	 
#include <fcntl.h>       
#include <sys/resource.h>


/************************start 应用编码******以下为示例、说明********************************/
#if 1

int __REPLACE__AddWork( void *arg )
{
struct rlimit rt;
rt.rlim_max = rt.rlim_cur = 10000;
if (setrlimit(RLIMIT_NOFILE, &rt) == -1)
{
    perror("setrlimit error");
    return -1;
}

	
	int iCount = 0;
	int i = 0;
	int iTask_Num = 0;
	char sTask_Num[ 1024 ];
	
	/* 从配置文件中，获取执行任务的线程数目 */
	memset( sTask_Num, 0x00, sizeof( sTask_Num) );
	if ( 0 != Get_ValuseString_ByFieldKey_InConfFile( "conf/frame_threadpool.conf", "THREAD_POOL_SET", "SET_DO_WORK_THREAD_NUM_OF_POOL", sTask_Num ) )
	{
		return -1;
	}
	iTask_Num = atoi( sTask_Num );


	for( i = 0; i < iTask_Num; i++ )
	{
		iCount++;
		void *pPoint_Task_Addr;
		void *pPoint_Buf;

		pPoint_Task_Addr = NULL;
		pPoint_Buf = NULL;

		pPoint_Task_Addr = malloc( 1 );
		pPoint_Buf = (char *)malloc( 1 );

		memset( pPoint_Task_Addr, 0x00, 1 );
		memset( pPoint_Buf, 0x00, 1 );

		memcpy( pPoint_Task_Addr, &pPoint_Buf, 1 );

		Into_Queue( pPoint_Task_Addr );
		
		//sleep( 1 );
	}
}



int iCount_Do = 0;

/*
获取的任务的地址，执行具体的业务操作
只需要在此函数中，自行定义相关的业务操作即可，本文件中的其他地方不需要改动
*/
int __REPLACE__DoWork( void * const pPoint_Task_Addr )
{
/*
	char * p2;
	p2 = (char* )pPoint_Task_Addr; 
	printf( "%s\n", p2 );
	free( pPoint_Task_Addr );
*/
	int fd;
	struct sockaddr_in addr;
	int r;
	
	/*1.建立socket*/
	fd=socket(AF_INET,SOCK_STREAM,6);
	/*2.发送数据:write/send/sendto*/
	addr.sin_family=AF_INET;
	addr.sin_port=htons(6888);
	inet_aton("10.242.134.75",&addr.sin_addr);

	r = connect(fd,(struct sockaddr*)&addr,sizeof(addr)); printf ("iCount_Do=%d\n", iCount_Do++ );
	char buf[256];
	r = read(fd,buf,sizeof(buf)-1);
	if( r > 0 ){
		buf[r]=0;
		printf("来自服务器的信息:%s\n",buf);
	}
	if(r==0) printf("连接已经关闭!\n");
	
	if(r==-1) printf("网络故障!\n");
	
	/*3.关闭socket*/
	close(fd);
	
}

#endif
/******************************end 应用编码******************************/





int AddWork( void *arg )
{
	return __REPLACE__AddWork( arg );
}


int DoWork( void * const pPoint_Task_Addr )
{
	return __REPLACE__DoWork( pPoint_Task_Addr );
}
