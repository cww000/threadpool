#ifndef THREADPOOL_H
#define THREADPOOL_H


#include <mutex>
#include <condition_variable>
#include "taskqueue.h"
#include <vector>
#include <thread>
#include <array>

template <typename T>
class ThreadPool
{
public:
    ThreadPool(int min, int max); //创建线程池
    ~ThreadPool();//销毁线程池
    void addTask(Task<T> task); //给线程池添加任务
    int getBusyNum(); //获取线程池中工作的线程个数
    int getLiveNum(); //获取线程池中活着的线程个数


private:
    TaskQueue<T>* taskQ;   
    std::array<std::thread, 1> m_manager;   //管理者线程
    std::vector<std::thread> m_workers;   //工作的线程
    int minNum;           //最小线程数量
    int maxNum;           //最大线程数量
    int busyNum;          //忙的线程个数
    int liveNum;         //存活的线程个数
    int exitNum;         //要销毁的线程个数
    static const int NUMBER = 2;
    std::mutex mutexPool;  //锁整个的线程池
    std::condition_variable notEmpty;     //任务队列是否为空
    bool shutdown;               //是否要销毁线程池
    static void* worker(void* arg);  //工作的线程（消费者线程）任务函数
    static void* manager(void* arg); //管理者线程任务函数
    void threadExit(); //单个线程退出
};

#endif // THREADPOOL_H
