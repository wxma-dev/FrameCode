/************************************************************
* Copyright (c)                                             *
* Source File:    Application.c                             *
* Description:    Ӧ�ÿ���Դ�ļ�                            *
*           Ӧ�ò�Ĵ��룬���ڴ˴�����ʵ��                  *
*           (1)�����ڴ棬�����ڴ�ĵ�ַ�������             *
*           (2)�õ��ڴ�ĵ�ַ�����н�����ִ������           *
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
	�̳߳صĿ�ܵ�Ӧ�ò��֣��ڸ��ļ��б�д�书Ч��
	��ɫ��
		�����ߣ�__REPLACE__AddWork ����������������,malloc(new) �ķ�ʽ�����ݷ������У�
			�������̵߳���������Ϊ0��ֱ����������߳� ִ������
		�����ߣ�__REPLACE__DoWork  ���������ִ������,ֱ���õ���Ӧ���ڴ棬ִ��������free(delete)�ڴ�
		��Ҫһ�� ����2�����������Ͽɵ� ����ṹ�壬�����ж���
*/

#include  <sys/types.h>  
#include <sys/epoll.h> 	 
#include <fcntl.h>       
#include <sys/resource.h>


/************************start Ӧ�ñ���******����Ϊʾ����˵��********************************/
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
	
	/* �������ļ��У���ȡִ��������߳���Ŀ */
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
��ȡ������ĵ�ַ��ִ�о����ҵ�����
ֻ��Ҫ�ڴ˺����У����ж�����ص�ҵ��������ɣ����ļ��е������ط�����Ҫ�Ķ�
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
	
	/*1.����socket*/
	fd=socket(AF_INET,SOCK_STREAM,6);
	/*2.��������:write/send/sendto*/
	addr.sin_family=AF_INET;
	addr.sin_port=htons(6888);
	inet_aton("10.242.134.75",&addr.sin_addr);

	r = connect(fd,(struct sockaddr*)&addr,sizeof(addr)); printf ("iCount_Do=%d\n", iCount_Do++ );
	char buf[256];
	r = read(fd,buf,sizeof(buf)-1);
	if( r > 0 ){
		buf[r]=0;
		printf("���Է���������Ϣ:%s\n",buf);
	}
	if(r==0) printf("�����Ѿ��ر�!\n");
	
	if(r==-1) printf("�������!\n");
	
	/*3.�ر�socket*/
	close(fd);
	
}

#endif
/******************************end Ӧ�ñ���******************************/





int AddWork( void *arg )
{
	return __REPLACE__AddWork( arg );
}


int DoWork( void * const pPoint_Task_Addr )
{
	return __REPLACE__DoWork( pPoint_Task_Addr );
}
