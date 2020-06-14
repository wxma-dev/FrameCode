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

#define UNIXDOMAIN_PATH "/tmp/unix.dg"
#define MAXLEN 1024
#define MAXLINE 1024
void dg_echo(int,struct sockaddr *,socklen_t );


int main(int argc ,char **argv)
{
    int sockfd;
    struct sockaddr_un cliaddr,servaddr;

    if((sockfd = socket(AF_LOCAL,SOCK_DGRAM,0))<0)//socket
    {
        printf("socket error\r\n");
        return -1;
    }
    unlink(UNIXDOMAIN_PATH);//delete file

    bzero(&servaddr,sizeof(servaddr));
//    memset(&servaddr,0x00,sizeof(servaddr));

    servaddr.sun_family=AF_LOCAL;
    strcpy(servaddr.sun_path,UNIXDOMAIN_PATH);

    if(bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr))<0)//bind
    {
        printf("bind error\r\n");
        return -1;
    }
    dg_echo(sockfd,(struct sockaddr *)&cliaddr,sizeof(cliaddr));

    return 0;
}
#if 1
void dg_echo(int sockfd ,struct sockaddr* pcliaddr,socklen_t clilen)
{
    char buf[MAXLEN];
    int n;
    socklen_t len ;
    while(1)
    {
        len = clilen;
        if((n=recvfrom(sockfd,buf,MAXLEN,0,pcliaddr,&len))<0)
        {
            printf("recvfrom error\r\n");
            return ;
        }
        sendto(sockfd,buf,n,0,pcliaddr,len);
    }
}
#endif