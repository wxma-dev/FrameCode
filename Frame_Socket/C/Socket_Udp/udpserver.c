#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<unistd.h>

//��ӡ������ʾѡ��
static void* usage(const char* port)
{ 
      printf("usage: %s [local_ip] [local_port]\n",port);
}

 int main(int argc,char* argv[])
 { 
      if(argc!=3)
      { 
          usage(argv[0]);
          return 1;
      }
      //�����׽���
      int sock = socket(AF_INET,SOCK_DGRAM,0);
      if(sock<0)
      {
          perror("socket");
          exit(1);
      }
    //���׽�����ip��ַ�Ͷ˿ںŽ��а�
    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = htons(atoi(argv[2]));
    local.sin_addr.s_addr = inet_addr(argv[1]);
    if(bind(sock,(struct sockaddr*)&local,sizeof(local))<0)
    {
        perror("bind");
        exit(2);
    }
    char buf[1024];

    struct sockaddr_in client;
    socklen_t len = sizeof(client);  
    char* msg = "Have a goog day";
    while(1)
    {
       //��ȡ����
       int r = recvfrom(sock,buf,sizeof(buf)-1,0,(struct sockaddr*)&client,&len);
       if(r<0)
       {
           perror("recvfrom");
           exit(3);
       }
       else
       {
            buf[r] = 0;
            printf("[%s : %d]#  %s\n",inet_ntoa(client.sin_addr), ntohs(client.sin_port),buf);  

            //��������
            if(sendto(sock,msg,strlen(msg),0,(struct sockaddr*)&client,len)<0)
            {
                perror("sendto");
                exit(4);
            }
            break;
       }
    }
    return 0;
 }