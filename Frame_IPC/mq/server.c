

/*server.c*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define TEST 'a'
#define MAX_TEXT 100

/*这个结构体是自己定义的，但是有一点通信双方都要一样*/
struct mybuf {
    int type;
    char msg_text[MAX_TEXT];
};

int main()
{
    int mid, ret;
    key_t key;
    char buf[MAX_TEXT];
    struct mybuf data;

    key = ftok(".", TEST);
    if(key == -1) {
        perror("ftok error");
        exit(1);
    }
    mid = msgget(key, IPC_CREAT|0777);
    if(mid == -1) {
        perror("msgget error");
        exit(2);
    }
    
    while(1) {
        ret = msgrcv(mid, (void *)&data, MAX_TEXT, 0, 0);
        if(strncmp(data.msg_text, "exit", 4) == 0) {
            break;
        }
        if(ret == -1) {
            exit(3);
            perror("msgrcv error");
        }
        printf("%d    %s", data.type, data.msg_text);
    }
    msgctl(mid, IPC_RMID, (struct msqid_ds *)0);
    return 0;
}


