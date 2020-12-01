#include "mutex.h"
#include "log.h"

namespace platinum {
Semaphore::Semaphore(uint32_t count) {
    if(sem_init(&m_semaphore, 0 ,count)) {
        LOG_ERROR("sem_init error %d", 0);
    }
}

Semaphore::~Semaphore() {
    sem_destroy(&m_semaphore);
}

void Semaphore::wait() {
    if(sem_wait(&m_semaphore)) {
        LOG_ERROR("sem_wait error %d", 0);
    }
}

void Semaphore::notify() {
    if(sem_post(&m_semaphore)) {
        LOG_ERROR("sem_notify error %d", 0);
    }
}

Condition::Condition(Mutex& mutex):m_mutex(mutex) {
    pthread_cond_init(&m_cond, nullptr);
}

Condition::~Condition() {
    pthread_cond_destroy(&m_cond);
}
void Condition::wait() {
    pthread_cond_wait(&m_cond, m_mutex.getMutexPtr());
}
void Condition::notify() {
    pthread_cond_signal(&m_cond);
}
void Condition::notifyall() {
    pthread_cond_broadcast(&m_cond);
}

}; // end of platinum
