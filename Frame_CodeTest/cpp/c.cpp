
#include <iostream>
using namespace std;

template <typename T>
T add(T a,T b)  //ע���βκͷ�������
{   
 return a+b;
}

 
int main()
{
    int num1, num2, sum; 
    cin>>num1>>num2;
    sum=add(num1,num2); //��intƥ��ģ�����T����sum,num1,num2���Ͳ�һ�����޷�ƥ�䡣
    cout<<sum;
}




