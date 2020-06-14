#ifndef _ADAPTER_H_
#define _ADAPTER_H_


#include <iostream>


//目标接口类，客户需要的接口
class Target
{
public:
    Target();
    virtual ~Target();
    virtual void Request();//定义标准接口
};

//需要适配的类
class Adaptee
{
public:
    Adaptee();
    ~Adaptee();
    void SpecificRequest();
};

//类模式，适配器类，通过public继承获得接口继承的效果，通过private继承获得实现继承的效果
class Adapter:public Target,private Adaptee
{
public:
    Adapter();
    ~Adapter();
    virtual void Request();//实现Target定义的Request接口
};

//对象模式，适配器类，继承Target类，采用组合的方式实现Adaptee的复用
class Adapter1:public Target
{
public:
    Adapter1(Adaptee* adaptee);
    Adapter1();
    ~Adapter1();
    virtual void Request();//实现Target定义的Request接口
private:
    Adaptee* _adaptee;
};
#endif



using namespace std;

Target::Target()
{}

Target::~Target()
{}

void Target::Request()
{
    cout << "Target::Request()" << endl;
}

Adaptee::Adaptee()
{
}

Adaptee::~Adaptee()
{
}

void Adaptee::SpecificRequest()
{
    cout << "Adaptee::SpecificRequest()" << endl;
}

//类模式的Adapter
Adapter::Adapter()
{
}

Adapter::~Adapter()
{
}

void Adapter::Request()
{
    cout << "Adapter::Request()" << endl;
    this->SpecificRequest();
    cout << "----------------------------" <<endl;
}

//对象模式的Adapter
Adapter1::Adapter1():_adaptee(new Adaptee)
{
}

Adapter1::Adapter1(Adaptee* _adaptee)
{
    this->_adaptee = _adaptee;
}

Adapter1::~Adapter1()
{
}

void Adapter1::Request()
{
    cout << "Adapter1::Request()" << endl;
    this->_adaptee->SpecificRequest();
    cout << "----------------------------" <<endl;
}


int main()
{
    //类模式Adapter
    Target* pTarget = new Adapter();
    pTarget->Request();

    //对象模式Adapter1
    Adaptee* ade = new Adaptee();
    Target* pTarget1= new Adapter1(ade);
    pTarget1->Request();

    //对象模式Adapter2
    Target* pTarget2 = new Adapter1();
    pTarget2->Request();

    return 0;
}
