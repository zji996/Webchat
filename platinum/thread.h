#ifndef __PLATINUM_THREAD_H_
#define __PLATINUM_THREAD_H_

#include "mutex.h"

namespace platinum {

class Thread {
public:
	Thread(std::function<void()> cb, const std::string& name);
    ~Thread();

    pid_t getId() const { return m_id;}

    const std::string& getName() const { return m_name;}
    void join();

    static Thread* GetThis();

    static const std::string& GetName();

    static void SetName(const std::string& name);
private:
    static void* run(void* arg);

private:
    pid_t m_id = -1;
    pthread_t m_thread = 0;
    std::function<void()> m_cb;
    std::string m_name;
    Semaphore m_semaphore;

};

};

#endif
