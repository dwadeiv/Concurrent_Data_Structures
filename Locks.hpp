#ifndef LK_H
#define LK_H
#include <atomic>
#include <iostream>
#include <string>
#include <pthread.h>
#include <assert.h>

#define SEQ_CST std::memory_order_seq_cst
#define RELAXED std::memory_order_relaxed

using std::string;

enum lock_type {

    tas,
    ttas,
    ticket,
    pthread_lock

};


class Locks
{
    private:
        std::atomic<bool> flag;
        std::atomic<int> next_num;
        std::atomic<int> now_serving;
        lock_type lock;
        pthread_mutex_t p_lock;
        bool testAndSet();
        void tas_lock();
        void tas_unlock();
        void ttas_lock();
        void ttas_unlock();
        void ticket_lock();
        void ticket_unlock();


    public:
        Locks(string lock_type);
        ~Locks();
        void acquire();
        void release();
        int try_acquire();

};

#endif