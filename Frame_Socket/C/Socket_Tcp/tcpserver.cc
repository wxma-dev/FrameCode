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
    //�����׽���  
    int sk = socket(AF_INET, SOCK_STREAM, 0);  
  
    struct sockaddr_in server;  
    bzero(&server, sizeof(server));  
    server.sin_family = AF_INET;  
    server.sin_port = htons(atoi(argv[1]));  
    server.sin_addr.s_addr = htonl(INADDR_ANY);  
    //�˿ڰ�  
    bind(sk, (struct sockaddr*)&server, sizeof(server));  
  
    //����  
    listen(sk, 5);  
  
    struct sockaddr_in client;  
    bzero(&client, sizeof(client));  
    socklen_t len = sizeof(client);  
    //������������  
    int talk = accept(sk, (struct sockaddr*)&client, &len);  
  
    //��������  
    send(talk, "Hello", strlen("Hello") + 1, 0);  
    //��������  
    char buff[1024] = {'\0'};  
    recv(talk, buff, sizeof(buff), 0);  
    cout << buff << endl;  
  
    //�ر��׽���  
    close(talk);  
    close(sk);  
    return 0;  
}