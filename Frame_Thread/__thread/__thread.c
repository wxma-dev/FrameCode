#include<iostream>  
#include<pthread.h>  
#include<unistd.h>  
using namespace std;  
const int i=5;  
__thread int var=i;//���ַ�ʽЧ��һ��  
//__thread int var=5;//  
void* worker1(void* arg);  
void* worker2(void* arg);  
int main(){
    pthread_t pid1,pid2;  
    //__thread int temp=5;  
    static __thread  int temp=10;//���κ����ڵ�static����  
    pthread_create(&pid1,NULL,worker1,NULL);  
    pthread_create(&pid2,NULL,worker2,NULL);  
    pthread_join(pid1,NULL);  
    pthread_join(pid2,NULL);  
    cout<<temp<<endl;//���10  
    return 0;  
}  
void* worker1(void* arg){
	sleep(2);
    cout<<++var<<endl;//��� 6  
}  
void* worker2(void* arg){  
    sleep(10);//�ȴ��߳�1�ı�varֵ����֤�Ƿ�Ӱ���߳�2  
    cout<<++var<<endl;//���6  
}