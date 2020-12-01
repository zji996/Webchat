#include "threadpool.h"

namespace platinum {
void ThreadPool::run() {
    MutexType::Lock lock(m_mutex);
    if(m_started)
            return;
    m_started = true;

    for(int i = 0; i < m_threadNumber; ++i) {
        Thread* ptrTh = new Thread(std::bind(&ThreadPool::threadFunc, this), "cb_name_" + std::to_string(i));
        m_vecThreadPtr.push_back(ptrTh);
    }
}

void ThreadPool::stop() {
    if(!m_started)
        return;
    m_started = false;
    m_cond.notifyall();

    for(int i = 0; i < m_threadNumber; ++i) {
        m_vecThreadPtr[i]->join();
        delete m_vecThreadPtr[i];
    }
}

void ThreadPool::push(Task task) {
    MutexType::Lock lock(m_mutex);
    m_buffer.push(task);
    m_cond.notify();
}

ThreadPool::Task ThreadPool::pop() {
    MutexType::Lock lock(m_mutex);
    if(isbufferempty() && m_started) {
        m_cond.wait();
    }
    if(m_started) {
        Task t = m_buffer.front();
        m_buffer.pop();
        return t;
    }
    return NULL;
}

}; // end of platinum

