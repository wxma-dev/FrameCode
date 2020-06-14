

#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>  
#define MAX 100
sem_t sem1,sem2;    
void* th_fn1(void* arg)
{    
       inti;    
       for(i = 0; i < MAX; ++i)    
       {        
              sem_wait(&sem1);        
              printf("numberin thread1 is %d\n",i);        
              sem_post(&sem2);    
       }
       pthread_exit((void*)"thread1exit\n");
}
void* th_fn2(void* arg)
{    
       inti;  
       for(i = 0; i < MAX; ++i)
   {        
              sem_wait(&sem2);
       printf("number in thread2 is %d\n",i);       
               sem_post(&sem1);
   }   
        pthread_exit((void*)"thread2exit\n"); }
int main(void)
{   
       void*tret;  
       sem_init(&sem1,0,5);
       sem_init(&sem2,0,5);
       pthread_ttid1,tid2;  
       pthread_create(&tid1,NULL,th_fn1,NULL);           pthread_create(&tid2,NULL,th_fn2,NULL); 
       pthread_join(tid1,&tret);  
       pthread_join(tid2,&tret);  
       sem_destroy(&sem1); 
       sem_destroy(&sem2);  
       return0;
}




