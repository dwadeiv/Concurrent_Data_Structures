#ifndef BAR_H
#define BAR_H
#include <atomic>
#include <iostream>
#include <string>
#include <pthread.h>
#include <assert.h>

#define SEQ_CST std::memory_order_seq_cst
#define RELAXED std::memory_order_relaxed

using std::string;

enum barrier_type {

    sense_barrier,
    pthread_barrier

};

class Barriers
{
    private:
        std::atomic<int> cnt;
        std::atomic<int> sense;
        barrier_type bar;
        pthread_barrier_t p_bar;
        void sense_wait();
        int NUM_THREADS;


    public:
        Barriers(string barrierType, int number_of_threads);
        ~Barriers();
        void wait();
        
        
};

#endif