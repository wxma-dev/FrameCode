#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#define SEM_R    0400   //�û�����������
#define SEM_A    0200   //�û���������д
#define SVSEM_MODE (SEM_R | SEM_A | SEM_R>>3 | SEM_R>>6)

int main(int argc,char *argv[])
{
    int   c,oflag,semid,nsems;
    oflag = SVSEM_MODE | IPC_CREAT;   //���ô���ģʽ
    //���������в���e�ж��Ƿ��ƶ���IPC_EXCLģʽ
    while((c = getopt(argc,argv,"e"))!= -1)   
    {
        switch(c)
        {
            case 'e':
                oflag |= IPC_EXCL;
                break;
        }
    }
    //�ж������в����Ƿ�Ϸ�
    if (optind != argc -2)
    {
        printf("usage: semcreate [-e] <pathname> <nsems>");
        exit(0);
    }
    //��ȡ�ź��������е��ź�������
    nsems = atoi(argv[optind+1]);
    //�����ź�����ͨ��ftok��������һ��key�������ź��� ��ʶ��
    semid = semget(ftok(argv[optind],0),nsems,oflag);
    exit(0);
}


