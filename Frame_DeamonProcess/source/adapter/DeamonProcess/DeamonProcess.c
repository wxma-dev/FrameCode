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
 *    ProgramName :
 *    Description : 本程序为Daemon进程初始化、运行的技术框架
 *    History     :
 *    YYYY/MM/DD        Position        Author         Description
 *                                      wxma
 *    -------------------------------------------------------------
 */

#include "DeamonProcessWork.h"

void DaemonInit();
void ExitHandler(int iSignal);
void (*__signal(int signo, void (*func)(int)))(int);

int main()
{

    int iRet = -1;

    iRet = fork();
    if ( 0 == iRet )  /* child process */
    {
        make_child();
    }
    else  /* father process */
    {
        exit(0);
    }
}

int make_child()
{
    int  iRet = -1;

    //守护进程
    DaemonInit();
    
    iRet = ProcessWorkInit();
    if ( -1 == iRet )
    {
        exit(-1);
    }
    
    iRet = ProcessWork();
    if ( -1 == iRet )
    {
        exit(-1);
    }
}


void DaemonInit() {
    int i;
    
    setbuf(stdout,NULL);                /* 标准输出缓存关闭 */
    setgid(0);                          /* 进程组设置为root组 */
    
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

void ExitHandler(int iSignal)
{
    char aczCmd[200];

    signal(SIGTERM, SIG_IGN);
    signal(SIGUSR1, SIG_IGN);
    signal(SIGUSR2, SIG_IGN);
    signal(SIGINT,  SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGKILL, SIG_IGN);
    signal(SIGSEGV, SIG_IGN);
    signal(SIGCONT, SIG_IGN);

    ExitHandler_Work();

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

/*
** end of file
*/
