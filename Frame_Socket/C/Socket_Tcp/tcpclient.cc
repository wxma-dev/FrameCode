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
    //�����׽���  
    int sk = socket(AF_INET, SOCK_STREAM, 0);  
  
    struct sockaddr_in server;  
    server.sin_family = AF_INET;  
    server.sin_port = htons(atoi(argv[2]));  
    server.sin_addr.s_addr = inet_addr(argv[1]);  
    //���ӷ�����  
    connect(sk, (struct sockaddr*)&server, sizeof(server));  

    char buff[1024] = {'\0'};  
    //��������  
    recv(sk, buff, sizeof(buff), 0);  
    cout << buff << endl;  
    //��������  
    send(sk, "I am hahaya", strlen("I am hahaya") + 1, 0);  
  
    //�ر��׽���  
    close(sk);  
    return 0;  
}  