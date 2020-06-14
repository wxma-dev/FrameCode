#ifndef OBSERVER_H
#define OBSERVER_H

#include <iostream>
#include <list>

typedef int STATE;

class Observer;

// Subject抽象基类,只需要知道Observer基类的声明就可以了
class Subject
{
public:
    Subject() : m_nSubjectState(-1){}
    virtual ~Subject();

    void Notify();                            // 通知对象改变状态，模式的核心
    void Attach(Observer *pObserver);         // 新增对象
    void Detach(Observer *pObserver);         // 删除对象

    // 虚函数,提供默认的实现,派生类可以自己实现来覆盖基类的实现
    virtual void   SetState(STATE nState);    // 设置状态
    virtual STATE  GetState();                // 得到状态

protected:
    STATE m_nSubjectState;                    // 模拟保存Subject状态的变量
    std::list<Observer*>    m_ListObserver;   // 保存Observer指针的链表
};

// Observer抽象基类
class Observer
{
public:
    Observer() : m_nObserverState(-1){}
    virtual ~Observer(){}

    // 纯虚函数,各个派生类可能有不同的实现
    // 通知Observer状态发生了变化
    virtual void Update(Subject* pSubject) = 0;

protected:
    STATE m_nObserverState;                    // 模拟保存Observer状态的变量
};

// ConcreateSubject类,派生在Subject类
class ConcreateSubject
    : public Subject
{
public:
    ConcreateSubject() : Subject(){}
    virtual ~ConcreateSubject(){}

    // 派生类自己实现来覆盖基类的实现
    virtual void    SetState(STATE nState);    // 设置状态
    virtual STATE   GetState();                // 得到状态

};

// ConcreateObserver类派生自Observer
class ConcreateObserver
    : public Observer
{
public:
    ConcreateObserver() : Observer(){}
    virtual ~ConcreateObserver(){}

    // 虚函数,实现基类提供的接口
    virtual void Update(Subject* pSubject);
};

#endif




#include <iostream>
#include <algorithm>

/* --------------------------------------------------------------------
|    Subject类成员函数的实现
|
 ----------------------------------------------------------------------*/

void Subject::Attach(Observer *pObserver)
{
    std::cout << "Attach an Observer\n";

    m_ListObserver.push_back(pObserver);
}

void Subject::Detach(Observer *pObserver)
{
    std::list<Observer*>::iterator iter;
    iter = std::find(m_ListObserver.begin(), m_ListObserver.end(), pObserver);

    if (m_ListObserver.end() != iter)
    {
        m_ListObserver.erase(iter);
    }

    std::cout << "Detach an Observer\n";
}

void Subject::Notify()
{
    std::cout << "Notify Observers's State\n";

    std::list<Observer*>::iterator iter1, iter2;

    for (iter1 = m_ListObserver.begin(), iter2 = m_ListObserver.end();
         iter1 != iter2;
         ++iter1)
    {
        (*iter1)->Update(this);
    }
}

void Subject::SetState(STATE nState)
{
    std::cout << "SetState By Subject\n";
    m_nSubjectState = nState;
}

STATE Subject::GetState()
{
    std::cout << "GetState By Subject\n";
    return m_nSubjectState;
}

Subject::~Subject()
{
    std::list<Observer*>::iterator iter1, iter2, temp;

    for (iter1 = m_ListObserver.begin(), iter2 = m_ListObserver.end();
        iter1 != iter2;
        )
    {
        temp = iter1;
        ++iter1;
        delete (*temp);
    }

    m_ListObserver.clear();
}

/* --------------------------------------------------------------------
|    ConcreateSubject类成员函数的实现
|
----------------------------------------------------------------------*/
void ConcreateSubject::SetState(STATE nState)
{
    std::cout << "SetState By ConcreateSubject\n";
    m_nSubjectState = nState;
}

STATE ConcreateSubject::GetState()
{
    std::cout << "GetState By ConcreateSubject\n";
    return m_nSubjectState;
}

/* --------------------------------------------------------------------
|    ConcreateObserver类成员函数的实现
|
----------------------------------------------------------------------*/
void ConcreateObserver::Update(Subject* pSubject)
{
    if (NULL == pSubject)
        return;

    m_nObserverState = pSubject->GetState();

    std::cout << "The ObeserverState is " << m_nObserverState << std::endl;
}




int main()
{
    Observer *p1 = new ConcreateObserver;
    Observer *p2 = new ConcreateObserver;

    Subject* p = new ConcreateSubject;
    p->Attach(p1);
    p->Attach(p2);
    p->SetState(4);          // 展现威力的时候：)
    p->Notify();

    p->Detach(p1);
    p->SetState(10);
    p->Notify();

    delete p;

    system("pause");

    return 0;
}


