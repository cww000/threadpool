#ifndef TaskQUEUE_H
#define TaskQUEUE_H

#include <queue>
#include <mutex>

using callback = void(*) (void * arg);
template <typename T>
class Task
{
public:
    Task<T>()
    {
        function = nullptr;
        arg = nullptr;
    }
    Task<T>(callback f, void* arg)
    {
        this->arg = (T*)arg;
        this->function = f;
    }
    callback function;
    T* arg;
};

template <typename T>
class TaskQueue
{
public:
    TaskQueue();
    ~TaskQueue();
    void addTask(Task<T> task);  //添加任务
    void addTask(callback f, void* arg);  //添加任务
    Task<T> takeTask();         //取出一个任务
    int taskNumber() {return m_taskQ.size();}  //获取当前任务个数
private:
    std::queue<Task<T>> m_taskQ;
  //  pthread_mutex_t mutexQueue;
    std::mutex mutexQueue;

};

#endif // Task<T>QUEUE_H
