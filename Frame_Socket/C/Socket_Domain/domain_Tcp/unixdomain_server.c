#include <sys/un.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#define UNIXDOMAIN_PATH "/tmp/unix.str"
#define MAXLEN 1024
void sig_child(int);
void str_echo(int );
typedef void(*sig_handle)(int);//handle

int main(int argc ,char **argv)
{
    int listenfd;
    int connfd;
    socklen_t clilen;
    struct sockaddr_un cliaddr,servaddr;
    pid_t childpid;

    if((listenfd = socket(AF_LOCAL,SOCK_STREAM,0))<0)//socket
    {
        printf("socket error\r\n");
        return -1;
    }
    unlink(UNIXDOMAIN_PATH);//delete file

    bzero(&servaddr,sizeof(servaddr));
//    memset(&servaddr,0x00,sizeof(servaddr));

    servaddr.sun_family=AF_LOCAL;
    strcpy(servaddr.sun_path,UNIXDOMAIN_PATH);

    if(bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr))<0)//bind
    {
        printf("bind error\r\n");
        return -1;
    }

    if(listen(listenfd,5)<0)//listen
    {
        printf("listen error\r\n");
        return -1;
    }

    sig_handle handle1 = sig_child;
    signal(SIGCHLD,handle1);

    while(1)
    {
        clilen = sizeof(cliaddr);
        if((connfd=accept(listenfd,(struct sockaddr *)&cliaddr,&clilen))<0)
        {
            printf("accept error\r\n");
        }
        if((childpid= fork())==0)//child process
        {
            close(listenfd);
            str_echo(connfd);
            return 0;
        }
        close(connfd);//parent  close connfd
    }
    return 0;
}

void str_echo(int connfd)
{
    ssize_t nread;
    char readbuff[MAXLEN];

    memset(readbuff,0x00,sizeof(readbuff));
    while((nread=read(connfd,readbuff,MAXLEN))>0)
    {
        write(connfd,readbuff,strlen(readbuff));

        memset(readbuff,0x00,sizeof(readbuff));
    }

}



void sig_child(int signo)
{
    pid_t pid;
    int stat;
#if 1 
    while((pid=waitpid(-1,&stat,WNOHANG))>0)
    printf("waitpid:child terminated,pid=%d\r\n",pid);
#endif
    return ;
}
