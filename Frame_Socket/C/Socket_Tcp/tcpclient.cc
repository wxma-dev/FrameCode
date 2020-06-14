#include <iostream>  
#include <cstring>  
#include <strings.h>  
#include <stdlib.h>  
#include <sys/socket.h>  
#include <arpa/inet.h>  
#include <netinet/in.h>  
#include <unistd.h>

using namespace std;  
  
int main(int argc, char *argv[])  
{  
    //创建套接字  
    int sk = socket(AF_INET, SOCK_STREAM, 0);  
  
    struct sockaddr_in server;  
    server.sin_family = AF_INET;  
    server.sin_port = htons(atoi(argv[2]));  
    server.sin_addr.s_addr = inet_addr(argv[1]);  
    //连接服务器  
    connect(sk, (struct sockaddr*)&server, sizeof(server));  

    char buff[1024] = {'\0'};  
    //接收数据  
    recv(sk, buff, sizeof(buff), 0);  
    cout << buff << endl;  
    //发送数据  
    send(sk, "I am hahaya", strlen("I am hahaya") + 1, 0);  
  
    //关闭套接字  
    close(sk);  
    return 0;  
}  