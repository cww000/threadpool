#include <iostream>
#include <unistd.h>
#include "threadpool.h"
#include "threadpool.cpp"
using namespace std;
void taskFunc(void* arg)
{
    int num = *(int*)arg;
    cout << "thread " << this_thread::get_id() << " is working, number = " << num << endl;
    sleep(1);
}
int main()
{
    ThreadPool<int> pool(3, 10);
    for (int i = 0; i < 100; ++i)
    {
       int *num = new int(i + 100);
        pool.addTask(Task<int>(taskFunc, num));
    }

    sleep(20);

    return 0;
}
