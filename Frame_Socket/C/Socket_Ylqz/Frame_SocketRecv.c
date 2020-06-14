/*
 *    Copyright (c) 2000-2005 GIT Ltd.
 *    All rights reserved
 *
 *    This is unpublished proprietary source code of GIT Ltd.
 *    The copyright notice above does not evidence any actual
 *    or intended publication of such source code.
 *
 *    NOTICE: UNAUTHORIZED DISTRIBUTION, ADAPTATION OR USE MAY BE
 *    SUBJECT TO CIVIL AND CRIMINAL PENALTIES.
 */

/*
 *    ProgramName : Frame_SocketRecv.c
 *    Description : used by teller program
 *    History     :
 *    YYYY/MM/DD        Position        Author         Description
 *                                      
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


YLCFG     gstYlCfg;
int  iListenfd;
jmp_buf   recv_msg_timeout;
YLCOMMMSG gstYlCommMsg;
int  giSeq;
int  iCommunfd;


int make_child(char * aczYlAddr, int iYLPort, int msgsendid, int msgsendidnet);
void DaemonInit();

int RECVWORK(int iQidResp, int iQidRespNet);
void SendsigToylsend();

void ExitHandler(int iSignal);
void set_alarm_recv(int iSig);
void (*__signal(int signo, void (*func)(int)))(int);

void ProcessMsg(char * pczRecvBuf, int iBufLen, int iQidResp, int iQidRespNet);


int main(int argc, char *argv[])
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
	Get_ValuseString_ByFieldKey_InConfFile( CONFIG_FILE, "CONFIG", "RECV_LOG_PATH", sLog_Path );
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

	//创建子进程
	DEBUG_FRAME( "fork process" );
	if( fork() == 0 )
	{
		make_child (gstYlCfg.ip, gstYlCfg.port, gstYlCfg.msgsendid, gstYlCfg.msgsendidnet);
	}

	exit(0);
}


int make_child(char * aczYlAddr, int iYLPort, int msgsendid, int msgsendidnet)
{
	int  iRet;
        int  iQidResp=0, iQidRespNet=0;
	char aczConnIp[32]={0};

	//守护进程
	DaemonInit();
	DEBUG_FRAME( "daemon process sucess" );

	//消息队列
	DEBUG_FRAME( "start open msg queue" );
	iQidResp = msgget(msgsendid, 0666|IPC_CREAT);
	if (iQidResp == -1)
	{
		ERROR_FRAME("msgget [%d] response queue error", msgsendid);
		exit(errno);
	}
	DEBUG_FRAME( "end open msg queue, msgsendid=[%d] ,iQidReq=[%d]", msgsendid, iQidResp );

	DEBUG_FRAME( "start open msg queue" );
	iQidRespNet = msgget(msgsendidnet, 0666|IPC_CREAT);
	if (iQidRespNet == -1)
	{
		ERROR_FRAME("msgget internet [%d] response queue error", msgsendidnet);
		exit(errno);
	}
	DEBUG_FRAME( "end open msg queue, msgsendidnet=[%d] ,iQidRespNet=[%d]", msgsendidnet, iQidRespNet );

	DEBUG_FRAME( "Start Open port [%d]...", iYLPort);
	while((iListenfd = u_open(iYLPort)) == -1)
	{
		ERROR_FRAME("打开端口%d失败", iYLPort);
		sleep(2);
	}

	DEBUG_FRAME( "start listen ... port=[%d]", iYLPort);

	while(1)
	{
		if((iCommunfd = yl_listen(iListenfd, aczConnIp)) == -1) {
			CloseSocket(iCommunfd);
			ERROR_FRAME( "监听端口%d失败", iYLPort);
			sleep(3);
			continue;
	}

	DEBUG_FRAME( "conne ok host=[%s:%d]", aczYlAddr, iYLPort );

	iRet=strncmp(aczConnIp, aczYlAddr, strlen(aczYlAddr));
	if(iRet != 0) {
		ERROR_FRAME( "paycomm.cfg ip[%s] # connip[%s]", \
		aczYlAddr, aczConnIp);
		CloseSocket(iCommunfd);
		sleep(3);
		continue;
	}

	DEBUG_FRAME( "端口%d连接成功", iYLPort);

	iRet=RECVWORK(iQidResp, iQidRespNet);
	if(iRet != 0) {
		ERROR_FRAME( "recv error[%d]", iRet);
		CloseSocket(iCommunfd);
		sleep(1);
		continue;
	}

	CloseSocket (iCommunfd);
	sleep(1);
	SendsigToylsend();
	}
}


int RECVWORK(int iQidResp, int iQidRespNet) {
	int  iRet, iBufLen;
	char aczRecvBuf[2048];

	while(1) {
		DEBUG_FRAME( "start work" );

		signal(SIGALRM, set_alarm_recv);

		if(setjmp(recv_msg_timeout) == 0) {
			alarm(CHECK_LINK_RECV_TIMEOUT);
			memset( aczRecvBuf, 0, sizeof( aczRecvBuf ) );

			//收取银联报文，含46字节的报文头, iBufLen仅为报文体的长度
			iRet=CUPSRecvMsg(iCommunfd, (char *) aczRecvBuf, &iBufLen, 0);
			alarm(0);

			if(iRet < 0 && (iRet != -1107)) {
				ERROR_FRAME( "rcvmsg error ret=[%d], restart.", iRet);
				return -2;
			}

			if (iRet == 0) {
				//收到 '0000'
				DEBUG_FRAME( "成功接收测试空包");
				continue;
			}
			DEBUG_FRAME( "收到%d字节成功", iRet);

			if(iRet == -1107) {
				//收到异常报文
				ERROR_FRAME( "接收到长度不足46字节的异常报文");
				return -2;
			}

			//处理接收包
			ProcessMsg(aczRecvBuf, iBufLen ,iQidResp, iQidRespNet);
		} else {
			//指定时间内未收到银联报文
			DEBUG_FRAME( "[%d] timeout relisten!", giSeq);
			return -1; 
		}
	}
}


void ProcessMsg(char * pczRecvBuf, int iBufLen, int iQidResp, int iQidRespNet) {
	int iRet=0, iLen=0, iMsgLen=0; 
	int iQid=0;
	char aczTrace[16]={0};
	char aczMsgtype[8]={0};
	char aczCardNumber[64]={0};
	struct timeval stTime;
	MSGSTRUCT stCupsMq;

#if 0
	//加时间报文头
	gettimeofday(&stTime, NULL);
	memset(&stCupsMq, 0, sizeof(MSGSTRUCT));
	sprintf((char *)stCupsMq.mtext, "%-16.16ld", stTime.tv_sec);

    /* 响应交易:银行方发出消息的响应 包含消息报文格式错误的应答消息*/
    if( memcmp( pczRecvBuf, "0110" , 4 ) == 0 ||
        memcmp( pczRecvBuf, "0130" , 4 ) == 0 ||
        memcmp( pczRecvBuf, "0210" , 4 ) == 0 ||
        memcmp( pczRecvBuf, "0230" , 4 ) == 0 ||
        memcmp( pczRecvBuf, "0430" , 4 ) == 0 ||
        memcmp( pczRecvBuf, "0432" , 4 ) == 0 ||
        memcmp( pczRecvBuf, "0530" , 4 ) == 0 ||
        memcmp( pczRecvBuf, "0532" , 4 ) == 0 ||
        memcmp( pczRecvBuf, "0630" , 4 ) == 0 ||
        memcmp( pczRecvBuf, "0810" , 4 ) == 0 ||
        memcmp( pczRecvBuf, "0830" , 4 ) == 0 )
    {
		/* do nothing */
    }
    /***
    *** 银联发来的请求交易
    ***/
    else if( memcmp( pczRecvBuf, "0800" , 4 ) == 0 ||
             memcmp( pczRecvBuf, "0200" , 4 ) == 0 ||
             memcmp( pczRecvBuf, "0400" , 4 ) == 0 )
    {
        memcpy(stCupsMq.mtext+16, pczRecvBuf, iBufLen);
        iMsgLen = iBufLen+16;
        
        stCupsMq.mtype = 1;
        iRet=msgsnd(iQidResp, (struct msgbuf *)&stCupsMq, iMsgLen, IPC_NOWAIT);
        if (iRet != 0)
        {
            ERROR_FRAME(LERROR , "MMQsnd [%4.4s][%d] err[%d] iQid[%d], errno[%d]", \
            aczMsgtype, stCupsMq.mtype, iRet, iQid, errno );
            TOOL_Dump(0, "payrecv/sendqerr.log", \
            (unsigned char *)pczRecvBuf, iBufLen );
            return;
        }
    }
    else
    {
        ERROR_FRAME( LERROR, "receive unknow tran" );
        return;
    }
#endif
    return ;
}

/*
**  查找程序pczProgram在运行的进程 ID
*/
int	FindPidId( char *pczProgram )
{
	char		aczCommand[200];
	char		aczLogName[50];
	char		aczBuf[1024];
	int		iPidNum = 0;
	FILE		*fp;

	memset( aczCommand, 0x00, sizeof( aczCommand) );
	memset( aczLogName, 0x00, sizeof( aczLogName) );
	memset( aczBuf, 0x00, sizeof( aczBuf) );

	strcpy( aczLogName ,getenv( "LOGNAME" ) );

	sprintf( aczCommand, "ps -x -u %s |grep \"%s\"|grep -v grep|awk '{print $1}' ",
				aczLogName, pczProgram);

	if( (fp = popen( aczCommand,"r" ) ) != NULL )
	if( fgets( aczBuf, 1024, fp) != NULL )
	{
		//TrimStr( aczBuf, ' ' );
		iPidNum = atoi( aczBuf );
	}

	pclose( fp );

	return iPidNum;
}


void SendsigToylsend() {
	int iRet=0 ;
	char aczProg[200];

	sprintf( aczProg, "paysend %d", giSeq+2 );

	iRet=FindPidId(aczProg);
	if(iRet > 1) {
		ERROR_FRAME( "restart [%s] pid=[%d]", aczProg, iRet);
		kill(iRet, SIGUSR1);
	}

	return;
}


void ExitHandler(int iSignal) {
	char aczCmd[200];

	signal(SIGTERM, SIG_IGN);
	signal(SIGUSR1, SIG_IGN);
	signal(SIGUSR2, SIG_IGN);
	signal(SIGINT,  SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGKILL, SIG_IGN);
	signal(SIGSEGV, SIG_IGN);
	signal(SIGCONT, SIG_IGN);

	CloseSocket(iCommunfd);
	CloseSocket(iListenfd);

	gstYlCommMsg.status=0;

	/* 中断 11  SIGUSR1-16*/
	//if( iSignal == SIGSEGV || iSignal == SIGUSR1 )
	if(iSignal == SIGUSR1) {
		SendsigToylsend();
		sprintf(aczCmd, "payrecv %d",  giSeq);
		system(aczCmd);
	}

	DEBUG_FRAME( "close SIG[%d]!", iSignal );

	exit( 0 );
}


void (*__signal(int signo, void (*func)(int)))(int) {
	struct sigaction sa, sav;

	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, signo);
	sa.sa_handler = func;
	sa.sa_flags = 0;

	if (signo == SIGALRM) {
#ifdef SA_INTERRUPT
		sa.sa_flags |= SA_INTERRUPT;
#endif
	}
	else {
#ifdef SA_RESTART
		sa.sa_flags |= SA_RESTART;
#endif
	}

	if (sigaction(signo, &sa, &sav) < 0 ) {
		return SIG_ERR;
	}
	return sav.sa_handler;
}


void DaemonInit() {
	int i;
	char aczLogFile[80];

	setbuf(stdout,NULL);
	setgid(0);

	__signal( SIGTERM, ExitHandler );
	__signal( SIGUSR1, ExitHandler );
	__signal( SIGUSR2, ExitHandler );
	__signal( SIGINT,  ExitHandler );
	__signal( SIGQUIT, ExitHandler );
	__signal( SIGKILL, ExitHandler );
	__signal( SIGSEGV, ExitHandler );
	__signal( SIGCONT, ExitHandler );

	signal( SIGHUP,  SIG_IGN );
	signal( SIGTSTP, SIG_IGN );
	signal( SIGCLD,  SIG_IGN );
}


void set_alarm_recv(int iSig) {
	signal( iSig, SIG_IGN );

	longjmp( recv_msg_timeout, 1 );
}


/*
** end of file
*/
