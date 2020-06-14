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
#include <stdlib.h>

#define UNIXDOMAIN_PATH "/tmp/unix.dg"
#define MAXLEN 1024


void dg_cli(FILE*,int,const struct sockaddr *,socklen_t);

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_un servaddr;
    struct sockaddr_un cliaddr;

    if((sockfd = socket(AF_LOCAL,SOCK_DGRAM,0))<0)//socket
    {
        printf("socket error\r\n");
        return -1;
    }

    bzero(&cliaddr,sizeof(cliaddr));
//    memset(&servaddr,0x00,sizeof(servaddr));
    cliaddr.sun_family=AF_LOCAL;
    char temp[]="template_XXXXXX";
    int fd = mkstemp(temp);
    strcpy(cliaddr.sun_path,temp);
    unlink(temp);
    close(fd);
    if(bind(sockfd,(struct sockaddr*)&cliaddr,sizeof(cliaddr))<0)//bind
    {
        printf("conncet error\r\n");
        return -1;
    }
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sun_family=AF_LOCAL;
    strcpy(servaddr.sun_path,UNIXDOMAIN_PATH);
    dg_cli(stdin,sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
    return 0;
}



void dg_cli(FILE*fp,int sockfd,const struct sockaddr*pservaddr,socklen_t servlen)
{
    int n;
    char sendbuff[MAXLEN];
    char recvbuff[MAXLEN+1];
    while(fgets(sendbuff,MAXLEN,fp)!=NULL)
    {
        //指定服务器套接字结构直接sendto
        sendto(sockfd,sendbuff,strlen(sendbuff),0,pservaddr,servlen);
        if((n=recvfrom(sockfd,recvbuff,MAXLEN,0,NULL,NULL))<=0)
        {
            printf("recvfrom error\r\n");
            return ;
        }
        recvbuff[n]='\0';//防止越界
        fputs(recvbuff,stdout);//输出回射数据
    }
}