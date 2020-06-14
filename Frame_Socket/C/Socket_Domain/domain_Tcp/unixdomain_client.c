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

#define UNIXDOMAIN_PATH "/tmp/unix.str"
#define MAXLEN 1024


void str_cli(FILE*,int);

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_un servaddr;
    if((sockfd = socket(AF_LOCAL,SOCK_STREAM,0))<0)//socket
    {
        printf("socket error\r\n");
        return -1;
    }

    bzero(&servaddr,sizeof(servaddr));
//    memset(&servaddr,0x00,sizeof(servaddr));
    servaddr.sun_family=AF_LOCAL;//or AF_UNIX
    strcpy(servaddr.sun_path,UNIXDOMAIN_PATH);
    if(connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0)//connect
    {
        printf("conncet error\r\n");
        return -1;
    }
    str_cli(stdin,sockfd);
    return 0;
}

void str_cli(FILE*fp,int sockfd)
{
    int nread;
    int nwrite;
    char readbuff[MAXLEN];

    while( fgets(readbuff,sizeof(readbuff),fp)!=NULL)
    {
        if( (nwrite= write(sockfd,readbuff,strlen(readbuff)))<0)
        {
            printf("write error \r\n");
            return ;
        }
        memset(readbuff,0x00,sizeof(readbuff));
        if(( nread= read(sockfd,readbuff,sizeof(readbuff)))<0)
        {
            printf("read error \r\n");
            return ;
        }
        fputs(readbuff,stdout);
    }
}


