#include <iostream> 
using namespace std;
class A {
public:
     int mInt;
     A()
     {
          cout<< "in A Default Constructor:mInt = " << mInt<< endl;
          mInt = 0;
     }
     void setIntVal(int mSet)
     {
          cout<< "in setIntVal£ºmInt = " << mInt<<"mSet="<< mSet<< endl;
          if (mInt< mSet)
          {
               mInt= mSet;
          }
     }
};

class B{
private:
     int b;
     A aObj;     //AÀµĶÔóblic:
public:
     B()
     {
          cout<< "in B Default Constructor"<< endl;
          b = 10;
          aObj.setIntVal(20);
     }
};

int main() 
{
     B * pB = new B();
     delete pB;
     return 0;
}

