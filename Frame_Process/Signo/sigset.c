#include <unistd.h>  
#include <signal.h>  
#include <sys/types.h>  
#include <stdlib.h>  
#include <stdio.h>  
void handler(int sig)  
{  
	printf("Handle the signal %d\n", sig);  
}  
/*
首先，我们能过sigaction函数改变了SIGINT信号的默认行为，使之执行指定的函数handler，
所以输出了语句：Handle the signal 2。然后，通过sigprocmask设置进程的信号屏蔽字，把SIGINT信号屏蔽起来，
所以过了10秒之后，用sigpending函数去获取被阻塞的信号集时，检测到了被阻塞的信号SIGINT，
输出The SIGINT signal has ignored。最后，用函数sigdelset函数去除先前用sigaddset函数加在sigset上的信号SIGINT，
再调用函数sigsuspend，把进程的屏蔽字再次修改为sigset（不包含SIGINT)，并挂起进程。由于先前的SIGINT信号停留在待处理状态，
而现在进程已经不再阻塞该信号，所以进程马上对该信号进行处理，
从而在最后，你不用输入Ctrl+c也会出现后面的处理语句（可参阅前面特别提醒的内容），最后过了5秒程序就成功退出了。
*/
int main()  
{  
	sigset_t sigset;		//用于记录屏蔽字  
	sigset_t ign;			//用于记录被阻塞的信号集  
	struct sigaction act;

	//清空信号集  
	sigemptyset(&sigset);  
	sigemptyset(&ign); 

	//向信号集中添加信号SIGINT  
	sigaddset(&sigset, SIGINT);  

	//设置处理函数和信号集      
	act.sa_handler = handler;  
	sigemptyset(&act.sa_mask);  
	act.sa_flags = 0;  
	sigaction(SIGINT, &act, 0);  

	printf("Wait the signal SIGINT...\n");  
	pause();			//挂起进程，等待信号  

	//设置进程屏蔽字，在本例中为屏蔽SIGINT   
	sigprocmask(SIG_SETMASK, &sigset, 0);     
	printf("Please press Ctrl+c in 10 seconds...\n");  
	sleep(10);

	//测试SIGINT是否被屏蔽  
	sigpending(&ign);  
	if(sigismember(&ign, SIGINT))  
	printf("The SIGINT signal has ignored\n");  

	//在信号集中删除信号SIGINT  
	sigdelset(&sigset, SIGINT);  
	printf("Wait the signal SIGINT...\n");  
	//将进程的屏蔽字重新设置，即取消对SIGINT的屏蔽
	//并挂起进程  
	sigsuspend(&sigset);  

	printf("The app will exit in 5 seconds!\n");  
	sleep(5);  
	exit(0);  
}  
