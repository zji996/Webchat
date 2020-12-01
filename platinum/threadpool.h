#ifndef _PLATINUM_THREADPOOL_H_
#define _PLATINUM_THREADPOOL_H_

#include "mutex.h"
#include "thread.h"
#include "log.h"

#include <functional>
#include <vector>
#include <queue>

namespace platinum {

class ThreadPool : Nocopyable {
public:
    typedef Mutex MutexType;
    typedef std::function<void()> Task;
    ThreadPool() = delete;
    ThreadPool(int threadNumber) :m_threadNumber(threadNumber),
                m_mutex(),
                m_cond(m_mutex){
    }

    ~ThreadPool() {
        if(m_started)
            stop();
    }

    void run();
    void stop();
    void addTask(Task task) {
        push(task);
    }


private:
    void push(Task task);
    Task pop();
    bool isbufferempty() { return (0 == m_buffer.size());}
    void threadFunc() {
        while(m_started) {
            Task task = pop();
            if(task) {
                task();
            }
        }
    }

private:
    bool m_started = false;
    int m_threadNumber = 0;

    std::vector<Thread*> m_vecThreadPtr;
    std::queue<Task> m_buffer;
    MutexType m_mutex;
    Condition m_cond;
};

};

#endif

