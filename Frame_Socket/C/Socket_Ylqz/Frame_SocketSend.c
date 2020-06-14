/*
 *    Copyright (c) 2000-2012
 *    All rights reserved
 *
 *    This is unpublished proprietary source code of GIT Ltd.
 *    The copyright notice above does not evidence any actual
 *    or intended publication of such source code.
 *
 *    NOTICE: UNAUTHORIZED DISTRIBUTION, ADAPTATION OR USE MAY BE
 *    SUBJECT TO CIVIL AND CRIMINAL PENALTIES.
 *
 *    ProgramName : paysend.c
 *    Description : used by teller program
 *    History     :
 *    YYYY/MM/DD        Position        Author         Description
 *    -------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <setjmp.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>


#include "Frame_Socket.h"

#include "Log_Frame.h"


int send_test_msg();

extern	int	errno;
jmp_buf		send_msg_timeout;

void		ExitHandler( int iSignal );
int		giSocketID;
char	gaczProg[80];

YLCOMMMSG	gstYlCommMsg;
YLCFG		gstYlCfg;   

int         giSeq;
char        gaczJgdm[10];


void DaemonInit();

void set_alarm_send( int iSig )
{
   signal( iSig, SIG_IGN );

   longjmp( send_msg_timeout, 1 );
}

main(argc,argv)
int argc;
char ** argv;
{
	char    sLog_Path[256];
	char    sBuf_Port[20+1];
	char    sBuf_Shmkey[20+1];
	char    sBuf_Msgrecvid[20+1];
	char    sBuf_Msgsendid[20+1];
	char    sBuf_Msgsendidnet[20+1];

	memset( sLog_Path, 0x00, sizeof( sLog_Path ) );
	memset( sBuf_Port, 0x00, sizeof( sBuf_Port ) );
	memset( sBuf_Shmkey, 0x00, sizeof( sBuf_Shmkey ) );
	memset( sBuf_Msgrecvid, 0x00, sizeof( sBuf_Msgrecvid ) );
	memset( sBuf_Msgsendid, 0x00, sizeof( sBuf_Msgsendid ) );
	memset( sBuf_Msgsendidnet, 0x00, sizeof( sBuf_Msgsendidnet ) );

	//取LOG_PATH
	Get_ValuseString_ByFieldKey_InConfFile( CONFIG_FILE, "CONFIG", "SEND_LOG_PATH", sLog_Path );
	printf( "sLog_Path=[%s]\n", sLog_Path );

	//日志初始化
	DEBUG_OPEN_FRAME( sLog_Path );
	DEBUG_FRAME( "初始化日志成功" );

	//取IP、端口、消息队列
	memset(&gstYlCfg, 0x00, sizeof(YLCFG));
	Get_ValuseString_ByFieldKey_InConfFile( CONFIG_FILE, "00000001-1", "IP",           gstYlCfg.ip   );
	Get_ValuseString_ByFieldKey_InConfFile( CONFIG_FILE, "00000001-1", "PORT",         sBuf_Port );
	gstYlCfg.port = atoi( sBuf_Port );

	Get_ValuseString_ByFieldKey_InConfFile( CONFIG_FILE, "00000001-1", "SHMKEY",       sBuf_Shmkey );
	gstYlCfg.shmkey = atoi( sBuf_Shmkey );

	Get_ValuseString_ByFieldKey_InConfFile( CONFIG_FILE, "00000001-1", "MSGRCVID",     sBuf_Msgrecvid );
	gstYlCfg.msgrecvid = atoi( sBuf_Msgrecvid );

	Get_ValuseString_ByFieldKey_InConfFile( CONFIG_FILE, "00000001-1", "MSGSNDID_YL",  sBuf_Msgsendid );
	gstYlCfg.msgsendid = atoi( sBuf_Msgsendid );

	Get_ValuseString_ByFieldKey_InConfFile( CONFIG_FILE, "00000001-1", "MSGSNDID_NET", sBuf_Msgsendidnet );
	gstYlCfg.msgsendidnet = atoi( sBuf_Msgsendidnet );

	DEBUG_FRAME( "gstYlCfg.ip=[%s], gstYlCfg.port=[%d], gstYlCfg.shmkey=[%d], gstYlCfg.msgrecvid=[%d], gstYlCfg.msgsendid=[%d], gstYlCfg.msgsendidnet=[%d].", 
					gstYlCfg.ip, gstYlCfg.port, gstYlCfg.shmkey, 
					gstYlCfg.msgrecvid, gstYlCfg.msgsendid, gstYlCfg.msgsendidnet );

	/* fork子进程进行连接通讯 */
	DEBUG_FRAME( "fork process" );
	if ( fork () == 0 )
	{
		make_child ( gstYlCfg.ip, gstYlCfg.port , gstYlCfg.msgrecvid );
	}

	exit(0);
}

make_child( char   *aczYLAddr, int iYLPort ,int msgrecvid )
{
	int         iRet,iRet1;
	int         iBufLen;
	int         iQidReq;
	int         iLen;
	char        aczLen[5];
	char        aczSendBuf[CUPS_MAX_DATA_LEN+1];
	char        aczFile[80];
	char        aczRecvBuf[CUPS_MAX_DATA_LEN+1];
	char        aczRecvHeadBuf[CUPS_HEAD_LEN+1];
	char        aczTmp[128];
	MSGSTRUCT   stMsgBuf;

	char aczpczBuf[CUPS_MAX_DATA_LEN+1];
	char *pczBuf=NULL;
	char aczMsgType[5];
	int iBitMapLen;
	unsigned char aczBitMap[24];
	int iLoop;
	char aczFieldValue[1024];
	char aczAllValue[1024];
	int iTmpLen;
	char aczTmpLen[5];

	memset( aczpczBuf, 0x00, sizeof( aczpczBuf ) );
	memset( aczMsgType, 0x00, sizeof( aczMsgType ) );
	memset( aczBitMap, 0x00, sizeof( aczBitMap ) );
	memset( aczFieldValue, 0x00, sizeof( aczFieldValue ) );
	memset( aczAllValue, 0x00, sizeof( aczAllValue ) );
	memset( aczTmpLen, 0x00, sizeof( aczTmpLen ) );

	/* 进程设置为后台守护进程 */
	DaemonInit();
	DEBUG_FRAME( "daemon process sucess" );
	
	/* 创建消息队列,处理请求信息 */
	DEBUG_FRAME( "start open msg queue" );
	iQidReq = msgget( msgrecvid , 0666|IPC_CREAT );
	if ( iQidReq == -1 )
	{
		ERROR_FRAME( "msgget[%d] response queue error" , msgrecvid );
		exit( errno );
	}
	DEBUG_FRAME( "end open msg queue, msgrecvid=[%d] ,iQidReq=[%d]", msgrecvid, iQidReq );

	DEBUG_FRAME( "Tcp Connect to [%s:%d]", aczYLAddr, iYLPort);

	while(1)
	{
		/* 参数3: 连接次数    参数4: 每次连接间隔时间 超时=N3*N4+5 */
		giSocketID = TranClientByShortTcpIp_ext( iYLPort, aczYLAddr, 1, 1 );
		if( giSocketID < 0 )
		{
			/* 间隔3秒循环打开 */
			/*
			close( giSocketID );
			*/

			ERROR_FRAME( "Connect to failed[%d] [%s:%d] ", giSocketID, aczYLAddr, iYLPort );

			sleep( 3 );
			continue;
		}
		DEBUG_FRAME( "Connect to [%s:%d] ok", aczYLAddr, iYLPort );

		CONNWOR( iQidReq );
		close(giSocketID);
	}
}


CONNWOR( int iQidReq )
{
	int iRet;
	MSGSTRUCT   stMsgBuf;
	int iSendLen;

	struct timeval stTime;
	long   lStime;
	char   aczStime[17];

	iRet=send_test_msg();
	if ( iRet < 0 )
	{
		ERROR_FRAME( "send_test_msg err!" );
		return -1;
	}
	DEBUG_FRAME( "send_test_msg succ!" );

	/* 长连接，接受前置请求报文和银联请求的响应报文,分别进行处理 */
	while(1)
	{
		DEBUG_FRAME( "start work" );

		signal( SIGALRM, set_alarm_send );

		if( setjmp( send_msg_timeout ) == 0 )
		{
			alarm( CHECK_LINK_SEND_TIMEOUT );

			memset( stMsgBuf.mtext, 0, sizeof( stMsgBuf.mtext ) );
			iRet = msgrcv ( iQidReq, &stMsgBuf, MSG_SIZE, 0, MSG_NOERROR );
			if ( iRet <= 0 )
			{
				ERROR_FRAME( "msgrcv ret=%d error =%d !", iRet,  errno );
				continue;
			}
			alarm( 0 );

			memset( aczStime, 0x00, sizeof(aczStime) );
			memcpy( aczStime, stMsgBuf.mtext, 16 );

			lStime = atol(aczStime);

			gettimeofday( &stTime, NULL );
			if( (stTime.tv_sec - lStime) >30 )
			{
				ERROR_FRAME( "[%4.4s]trace[%d]timeout", stMsgBuf.mtext, stMsgBuf.mtype );
				continue;
			}

			iSendLen = iRet - 16 - 46;

			iRet = SendMsgToSocket( giSocketID, (char *)stMsgBuf.mtext + 16 + 46, iSendLen , 20, ASCII_FOUR_NONE_HEAD );
			if(iRet<0)
			{
				ERROR_FRAME( "send to pay error! mtype=[%d] ret=[%d] errno=[%d]", stMsgBuf.mtype, iRet, errno);
				sleep( 2 );
				return -1;
			}
			ERROR_FRAME( "send to pay ok! mtype=[%d] len=[%d]", stMsgBuf.mtype, iRet );

		}
		else
		{
			/*启动定时发送时钟程序*/
			iRet = send_test_msg();
			if ( iRet < 0 )
			{
				ERROR_FRAME( "send_test_msg err!" );
				return -1;
			}
		}
	}
}


void ExitHandler( int iSignal )
{
	char aczCmd[200];
	
	signal( SIGTERM, SIG_IGN );
	signal( SIGUSR1, SIG_IGN );
	signal( SIGUSR2, SIG_IGN );
	signal( SIGINT,  SIG_IGN );
	signal( SIGQUIT, SIG_IGN );
	signal( SIGKILL, SIG_IGN );
	signal( SIGSEGV, SIG_IGN );
	signal( SIGCONT, SIG_IGN );
	
	close ( giSocketID );
	
	gstYlCommMsg.status= 0;

	/* 中断 11   SIGUSR1- 16 */
	//if( iSignal == SIGSEGV || iSignal == SIGUSR1 )
	if( iSignal == SIGUSR1 )
	{
		sprintf( aczCmd, "paysend %s %d", gaczJgdm, giSeq );
		DEBUG_FRAME( "RESTART SIG[%d] [%s]", iSignal, aczCmd );
		system( aczCmd );
	}
	
	ERROR_FRAME( "close! SIG[%d]", iSignal );

	exit( 0 );
}

void DaemonInit()
{
	int i;

	setbuf(stdout,NULL);
	setgid(0);

	signal( SIGTERM, ExitHandler );
	signal( SIGUSR1, ExitHandler );
	signal( SIGUSR2, ExitHandler );
	signal( SIGINT,  ExitHandler );
	signal( SIGQUIT, ExitHandler );
	signal( SIGKILL, ExitHandler );
	signal( SIGSEGV, ExitHandler );
	signal( SIGCONT, ExitHandler );
	
	signal( SIGHUP,  SIG_IGN );
	signal( SIGTSTP, SIG_IGN );
	signal( SIGCLD,  SIG_IGN );
}

int send_test_msg()
{
	int iRet = 0;
	iRet = CUPSSendZeroToSocket(giSocketID,4);
	if ( iRet )
	{
		close(giSocketID);
		ERROR_FRAME( "send zero test buf to pay err! " );
		return -1;
	}
	return 0;
}


/*
** end of file
*/
