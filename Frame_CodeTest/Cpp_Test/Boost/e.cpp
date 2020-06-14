#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/tss.hpp>
#include <iostream>

boost::mutex io_mutex;
boost::thread_specific_ptr<int> ptr;

struct count
{
        count(int id) : id(id) { }
        
        void operator()()
        {
                if (ptr.get() == 0)
                ptr.reset(new int(0));
                
                for (int i = 0; i < 10; ++i)
                {
                        (*ptr)++;
                        boost::mutex::scoped_lock lock(io_mutex);
                        std::cout << id << ": " << *ptr << std::endl;
                }
        }
        
        int id;
};

int main(int argc, char* argv[])
{
        boost::thread thrd1(count(1));
        boost::thread thrd2(count(2));
        thrd1.join();
        thrd2.join();
        return 0;
}
