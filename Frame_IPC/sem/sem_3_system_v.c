

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/ipc.h>

//�����ź�������������ṹ
union semun
{
    int                val;
    struct semid_ds    *buf;
    unsigned short     *array;
};

int main(int argc,char *argv[])
{
    int semid,nsems,i;
    struct semid_ds seminfo;
    unsigned short *ptr;
    union semun arg;
    if(argc < 2)
    {
            printf("usage: semsetvalues <pathname>[values ...]");
            exit(0);
    }
    //���Ѿ����ڵ��ź�������
    semid = semget(ftok(argv[1],0),0,0);
    arg.buf = &seminfo;
        //��ȡ�ź������������Ϣ
    semctl(semid,0,IPC_STAT,arg);
    nsems = arg.buf->sem_nsems;  //�ź����ĸ���
    if(argc != nsems + 2 )
    {
        printf("%s semaphores in set,%d values specified",nsems,argc-2);
        exit(0);
    }
    //�����ź���
    ptr = calloc(nsems,sizeof(unsigned short));
    arg.array = ptr;
    //��ʼ���ź�����ֵ
    for(i=0;i<nsems;i++)
        ptr[i] = atoi(argv[i+2]);
    //ͨ��arg�����ź�������
    semctl(semid,0,SETALL,arg);
    exit(0);
}



