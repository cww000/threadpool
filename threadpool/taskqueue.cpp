#include "taskqueue.h"

template <typename T>
TaskQueue<T>::TaskQueue()
{

}

template <typename T>
TaskQueue<T>::~TaskQueue()
{

}

template <typename T>
void TaskQueue<T>::addTask(Task<T> task)
{
    std::lock_guard<std::mutex> lock(mutexQueue);
    m_taskQ.push(task);
}

template <typename T>
void TaskQueue<T>::addTask(callback f, void *arg)
{
    std::lock_guard<std::mutex> lock(mutexQueue);
    m_taskQ.push(Task<T>(f, arg));
}

template <typename T>
Task<T> TaskQueue<T>::takeTask()
{
    Task<T> t;
    std::lock_guard<std::mutex> lock(mutexQueue);
    if (!m_taskQ.empty()) {
        t = m_taskQ.front();
        m_taskQ.pop();
    }
    return t;
}
