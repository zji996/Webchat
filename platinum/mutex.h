#ifndef _PLATINUM_MUTEX_H_
#define _PLATINUM_MUTEX_H_

#include <list>
#include <memory>
#include <thread>
#include <functional>
#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>

#include "nocopyable.h"

namespace platinum {

class Mutex;

class Semaphore : Nocopyable {
public:
    Semaphore(uint32_t count = 0);
    ~Semaphore();

    void wait();
    void notify();

private:
    sem_t m_semaphore;
};

class Condition {
public:
    Condition(Mutex& mutex);
    ~Condition();
    void wait();
    void notify();
    void notifyall();
private:
    pthread_cond_t m_cond;
    Mutex& m_mutex;
};

template <typename T>
struct ScopedLockImpl {
public:
    ScopedLockImpl(T& mutex)
        :m_mutex(mutex) {
        m_mutex.lock();
        m_locked = true;
    };
    ~ScopedLockImpl() {
        unlock();
    }

    void lock() {
        if(!m_locked) {
            m_mutex.lock();
            m_locked = true;
        }
    }

    void unlock() {
        if(m_locked) {
            m_mutex.unlock();
            m_locked = false;
        }
    }
private:
    T& m_mutex;
    bool m_locked;
};

class Mutex : Nocopyable {
public:
    typedef ScopedLockImpl<Mutex> Lock;

    Mutex() {
        pthread_mutex_init(&m_mutex, nullptr);
    }
    ~Mutex() {
        pthread_mutex_destroy(&m_mutex);
    }

    void lock() {
        pthread_mutex_lock(&m_mutex);
    }

    void unlock() {
        pthread_mutex_unlock(&m_mutex);
    }
    pthread_mutex_t* getMutexPtr() { return &m_mutex;}
private:
    pthread_mutex_t m_mutex;
};

class Spinlock : Nocopyable {
public:
    typedef ScopedLockImpl<Spinlock> Lock;
    Spinlock() {
        pthread_spin_init(&m_mutex, 0);
    }
    ~Spinlock() {
        pthread_spin_destroy(&m_mutex);
    }

    void lock() {
        pthread_spin_lock(&m_mutex);
    }

    void unlock() {
        pthread_spin_unlock(&m_mutex);
    }
private:
    pthread_spinlock_t m_mutex;
};


}; // end of platinum


#endif
