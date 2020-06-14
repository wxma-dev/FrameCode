#include <iostream>  
#include <cstring>  
#include <strings.h>  
#include <stdlib.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <unistd.h>

using namespace std;  
  
int main(int argc, char *argv[])  
{  
    //创建套接字  
    int sk = socket(AF_INET, SOCK_STREAM, 0);  
  
    struct sockaddr_in server;  
    bzero(&server, sizeof(server));  
    server.sin_family = AF_INET;  
    server.sin_port = htons(atoi(argv[1]));  
    server.sin_addr.s_addr = htonl(INADDR_ANY);  
    //端口绑定  
    bind(sk, (struct sockaddr*)&server, sizeof(server));  
  
    //监听  
    listen(sk, 5);  
  
    struct sockaddr_in client;  
    bzero(&client, sizeof(client));  
    socklen_t len = sizeof(client);  
    //接受连接请求  
    int talk = accept(sk, (struct sockaddr*)&client, &len);  
  
    //发送数据  
    send(talk, "Hello", strlen("Hello") + 1, 0);  
    //接收数据  
    char buff[1024] = {'\0'};  
    recv(talk, buff, sizeof(buff), 0);  
    cout << buff << endl;  
  
    //关闭套接字  
    close(talk);  
    close(sk);  
    return 0;  
}