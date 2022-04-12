#include "threadpool.h"
#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include "taskqueue.cpp"

using namespace std;

template <typename T>
ThreadPool<T>::ThreadPool(int min, int max)
{
    do {
        taskQ = new TaskQueue<T>;

        minNum = min;
        maxNum = max;
        busyNum = 0;
        liveNum = min;
        exitNum = 0;
        shutdown = false;

        //创建线程
        thread w(manager, this);
        m_manager.push_back(move(w));

        for (int i = 0; i < min; ++i)
        {
            thread t(worker, this);
            m_workers.push_back(move(t));
        }
        return;
    } while (0);

    //释放资源
    if (taskQ) delete taskQ;
}

template <typename T>
ThreadPool<T>::~ThreadPool()
{
    //关闭线程池
    shutdown = true;

    for (int i = 0; i < liveNum; ++i)
    {
        notEmpty.notify_all();
    }

    //释放内存
    if (taskQ)
    {
        delete taskQ;
    }

    for(thread &worker: m_workers)
    {
        worker.join();
    }

    m_manager[0].join();
}

template <typename T>
void ThreadPool<T>::addTask(Task<T> task)
{
    unique_lock<mutex> lock(mutexPool);
    if (shutdown) {
        lock.unlock();
    }

    taskQ->addTask(task);
    notEmpty.notify_all();

}

template <typename T>
int ThreadPool<T>::getBusyNum()
{
    unique_lock<mutex> lock(mutexPool);
    int busyNum = this->busyNum;
    return busyNum;
}

template <typename T>
int ThreadPool<T>::getLiveNum()
{
    unique_lock<mutex> lock(mutexPool);
    int liveNum = this->liveNum;
    return liveNum;
}

template <typename T>
void *ThreadPool<T>::worker(void *arg)
{
    ThreadPool* pool = static_cast<ThreadPool*>(arg);
    while (true)
    {
        unique_lock<mutex> lock(pool->mutexPool);
        //当前任务队列是否为空
        while (pool->taskQ->taskNumber() == 0 && !pool->shutdown)
        {
            //阻塞工作线程
            pool->notEmpty.wait(lock);

            //判断是不是要销毁线程
            if (pool->exitNum > 0)
            {
                pool->exitNum--;
                if (pool->liveNum > pool->minNum)
                {
                    pool->liveNum--;
                    lock.unlock();
                    pool->threadExit();
                }
            }
        }

        //判断线程是否关闭
        if (pool->shutdown)
        {
            lock.unlock();
            pool->threadExit();
        }

        //从任务队列中取出一个任务
        Task task = pool->taskQ->takeTask();

        //解锁
        pool->busyNum++;
        lock.unlock();

       cout << "thread "<<  this_thread::get_id()<< " start working...." << endl;

       task.function(task.arg);
       delete task.arg;
       task.arg = nullptr;

       cout << "thread "<< this_thread::get_id()<< " end working...." << endl;

       lock.lock();
       pool->busyNum--;

    }
    return NULL;
}

template <typename T>
void *ThreadPool<T>::manager(void *arg)
{
    ThreadPool* pool = static_cast<ThreadPool*>(arg);
    while (!pool->shutdown)
    {
        sleep(3);

        //取出线程池中任务的数量和当前线程的数量
        unique_lock<mutex> lock(pool->mutexPool);
        int queueSize = pool->taskQ->taskNumber();
        int liveNum = pool->liveNum;
        int busyNum = pool->busyNum;
        lock.unlock();

        //添加线程
        //任务的个数>存活的线程个数 && 存活的线程数 < 最大线程数
        if (queueSize > liveNum && liveNum < pool->maxNum)
        {
            lock.lock();
            int counter = 0;
            for (int i = 0; i < pool->maxNum && counter < NUMBER && pool->liveNum < pool->maxNum; ++i)
            {
                thread t(worker, pool);
                pool->m_workers.push_back(move(t));  //td::thread是可移动的(A也是可移动的，直到添加析构函数为止)，但不可复制。
                counter++;
                pool->liveNum++;
            }
            lock.unlock();

        }

        //销毁线程
        //忙的线程*2 < 存活的线程数 && 存活的线程数 > 最小线程数
        if (busyNum*2 < liveNum && liveNum > pool->minNum)
        {
            lock.lock();
            pool->exitNum = NUMBER;
            lock.unlock();
            //让工作的线程自杀
            for (int i = 0; i < NUMBER; ++i)
            {
                pool->notEmpty.notify_all();
            }
        }
    }
    return NULL;
}

template <typename T>
void ThreadPool<T>::threadExit()
{
    unique_lock<mutex> lock(mutexWorkers);
    thread::id this_id = this_thread::get_id();
    cout << "threadExit() called " << this_id << " exiting....." << endl;
    pthread_exit(NULL);
}
