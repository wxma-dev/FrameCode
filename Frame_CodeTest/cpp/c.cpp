
#include <iostream>
using namespace std;

template <typename T>
T add(T a,T b)  //注意形参和返回类型
{   
 return a+b;
}

 
int main()
{
    int num1, num2, sum; 
    cin>>num1>>num2;
    sum=add(num1,num2); //用int匹配模版参数T，若sum,num1,num2类型不一致则无法匹配。
    cout<<sum;
}




