
#include <iostream>

#include<thread>  
#include<chrono>  

using namespace std;  

void fun1(int n)  //初始化构造函数  
{  
    cout << "Thread " << n << " executing\n";  
    n += 10;  
    this_thread::sleep_for(chrono::milliseconds(10));  
}

  
void fun2(int & n) //拷贝构造函数  
{  
    cout << "Thread " << n << " executing\n";  
    n += 20;  
    this_thread::sleep_for(chrono::milliseconds(10));  
}

  
int main()  
{  
    int n = 0;  
    thread t1;               //t1不是一个thread  
    thread t2(fun1, n + 1);  //按照值传递  
    t2.join();  

    cout << "n=" << n << '\n';  

    n = 10;  
    std::thread t3(fun2, ref(n)); //引用  
    std::thread t4(move(t3));     //t4执行t3，t3不是thread  
    t4.join();  

    cout << "n=" << n << '\n';  
    return 0;  
}  


