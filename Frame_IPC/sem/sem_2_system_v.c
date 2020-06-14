#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/ipc.h>

int main(int argc,char *argv[])
{
    int     c,i,flag,semid,nops;
    struct  sembuf *ptr;
    flag = 0;
        //���������в������ò���ģʽ
    while( ( c = getopt(argc,argv,"nu")) != -1)
    {
        switch(c)
        {
            case 'n':
                flag |= IPC_NOWAIT;   //������
                break;
            case 'u':
                flag |= SEM_UNDO;   //���ɻָ�
                break;
        }
    }
    if(argc - optind < 2)
    {
        printf("usage: semops [-n] [-u] <pathname> operation...");
        exit(0);
    } 
    //��һ���Ѿ����ڵ��ź�������
    if((semid = semget(ftok(argv[optind],0),0,0)) == -1)
    {
        perror("semget() error");
        exit(-1);
    }
    optind++;  //ָ��ǰ��һ���ź�����λ��
    nops = argc - optind;   //�ź�������
    ptr = calloc(nops,sizeof(struct sembuf));
    for(i=0;i<nops;++i)
    {
        ptr[i].sem_num = i;  //�ź����任
        ptr[i].sem_op = atoi(argv[optind+i]);   //�����ź�����ֵ
        ptr[i].sem_flg = flag;   //���ò���ģʽ
    }
    //���ź���ִ�в���
    if(semop(semid,ptr,nops) == -1)  
    {
        perror("semop() error");
        exit(-1);
    }
    exit(0);
}




