

/*client.c*/
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
    int mid, ret, type;
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
        fputs("输入你要传送的数据类型:", stdout);
        setbuf(stdin, NULL);
        scanf("%d", &type);
        data.type = type;
        fputs("输入要传送的字符数据:", stdout);
        //清空键盘缓冲区，在linux下getchar(),fflush(stdin),rewind(stdin)都不起作用
        setbuf(stdin, NULL);
        fgets(buf, MAX_TEXT, stdin);
        memcpy(data.msg_text, buf, MAX_TEXT);
        ret = msgsnd(mid, (void *)&data, MAX_TEXT, 0);
        if(ret == -1) {
            exit(3);
            perror("msgsnd error");
        }
        if(strncmp(data.msg_text, "exit", 4) == 0) {
            exit(0);
        }
    }

    return 0;
}



