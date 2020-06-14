

#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#define MAX 100
#define BUFSIZE 10
//#define DEBUG 0
int buf[BUFSIZE]={0};
int in=0;
int out=0;
sem_t full,empty;
#ifdef DEBUG
void print(void)
{
   int i;
   printf("buf is:");
    for(i=out;i<in;i++)
       printf("%d ",buf[i]);
   printf("\n");
}
#endif  
void* producer(void* arg)
{
   int i;
   for (i = 1; i <= MAX; ++i)
    {
       /*produce*/
       sem_wait(&full);
       buf[in++]=i;
       in%=BUFSIZE;
#ifdef DEBUG
       print();
#endif
       sem_post(&empty);
    }
   pthread_exit((void*)"thread1 exit\n");
}
void* comsumer(void* arg)
{
   int i;
   for (i = 1; i <= MAX; ++i)
    {
       /*comsumer*/
       sem_wait(&empty);
       out++;
       out%=BUFSIZE;
       sem_post(&full);
    }
   pthread_exit((void*)"thread2 exit\n");
}
int main(void)
{
   void *tret;
   sem_init(&full,0,10);
   sem_init(&empty,0,0);
   pthread_t tid_producer,tid_comsumer;
   pthread_create(&tid_producer,NULL,producer,NULL);
   pthread_create(&tid_comsumer,NULL,comsumer,NULL);
   pthread_join(tid_producer,&tret);
       printf("%s\n",(char*)tret);
   pthread_join(tid_comsumer,&tret);
       printf("%s\n",(char*)tret);
   sem_destroy(&full);
   sem_destroy(&empty);
   return 0;
}


