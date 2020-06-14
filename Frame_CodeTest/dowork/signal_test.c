
/*
	测试pause的触发条件：信号并且有信号处理函数，或者退出信号
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>


void addwork_thread_singnal_fun( int signo )
{
	printf( "1" );
	return;
}

int main()
{
	signal( 28, addwork_thread_singnal_fun );
	pause();
	printf( "exit" );
}
