#ifndef _ADAPTER_H_
#define _ADAPTER_H_


#include <iostream>


//Ŀ��ӿ��࣬�ͻ���Ҫ�Ľӿ�
class Target
{
public:
    Target();
    virtual ~Target();
    virtual void Request();//�����׼�ӿ�
};

//��Ҫ�������
class Adaptee
{
public:
    Adaptee();
    ~Adaptee();
    void SpecificRequest();
};

//��ģʽ���������࣬ͨ��public�̳л�ýӿڼ̳е�Ч����ͨ��private�̳л��ʵ�ּ̳е�Ч��
class Adapter:public Target,private Adaptee
{
public:
    Adapter();
    ~Adapter();
    virtual void Request();//ʵ��Target�����Request�ӿ�
};

//����ģʽ���������࣬�̳�Target�࣬������ϵķ�ʽʵ��Adaptee�ĸ���
class Adapter1:public Target
{
public:
    Adapter1(Adaptee* adaptee);
    Adapter1();
    ~Adapter1();
    virtual void Request();//ʵ��Target�����Request�ӿ�
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

//��ģʽ��Adapter
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

//����ģʽ��Adapter
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
    //��ģʽAdapter
    Target* pTarget = new Adapter();
    pTarget->Request();

    //����ģʽAdapter1
    Adaptee* ade = new Adaptee();
    Target* pTarget1= new Adapter1(ade);
    pTarget1->Request();

    //����ģʽAdapter2
    Target* pTarget2 = new Adapter1();
    pTarget2->Request();

    return 0;
}
