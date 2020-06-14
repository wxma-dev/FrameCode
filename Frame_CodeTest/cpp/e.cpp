#ifndef OBSERVER_H
#define OBSERVER_H

#include <iostream>
#include <list>

typedef int STATE;

class Observer;

// Subject�������,ֻ��Ҫ֪��Observer����������Ϳ�����
class Subject
{
public:
    Subject() : m_nSubjectState(-1){}
    virtual ~Subject();

    void Notify();                            // ֪ͨ����ı�״̬��ģʽ�ĺ���
    void Attach(Observer *pObserver);         // ��������
    void Detach(Observer *pObserver);         // ɾ������

    // �麯��,�ṩĬ�ϵ�ʵ��,����������Լ�ʵ�������ǻ����ʵ��
    virtual void   SetState(STATE nState);    // ����״̬
    virtual STATE  GetState();                // �õ�״̬

protected:
    STATE m_nSubjectState;                    // ģ�Ᵽ��Subject״̬�ı���
    std::list<Observer*>    m_ListObserver;   // ����Observerָ�������
};

// Observer�������
class Observer
{
public:
    Observer() : m_nObserverState(-1){}
    virtual ~Observer(){}

    // ���麯��,��������������в�ͬ��ʵ��
    // ֪ͨObserver״̬�����˱仯
    virtual void Update(Subject* pSubject) = 0;

protected:
    STATE m_nObserverState;                    // ģ�Ᵽ��Observer״̬�ı���
};

// ConcreateSubject��,������Subject��
class ConcreateSubject
    : public Subject
{
public:
    ConcreateSubject() : Subject(){}
    virtual ~ConcreateSubject(){}

    // �������Լ�ʵ�������ǻ����ʵ��
    virtual void    SetState(STATE nState);    // ����״̬
    virtual STATE   GetState();                // �õ�״̬

};

// ConcreateObserver��������Observer
class ConcreateObserver
    : public Observer
{
public:
    ConcreateObserver() : Observer(){}
    virtual ~ConcreateObserver(){}

    // �麯��,ʵ�ֻ����ṩ�Ľӿ�
    virtual void Update(Subject* pSubject);
};

#endif




#include <iostream>
#include <algorithm>

/* --------------------------------------------------------------------
|    Subject���Ա������ʵ��
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
|    ConcreateSubject���Ա������ʵ��
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
|    ConcreateObserver���Ա������ʵ��
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
    p->SetState(4);          // չ��������ʱ��)
    p->Notify();

    p->Detach(p1);
    p->SetState(10);
    p->Notify();

    delete p;

    system("pause");

    return 0;
}


