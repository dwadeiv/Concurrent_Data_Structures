#include "Barriers.hpp"

using std::string;

Barriers::Barriers(string barrierType, int number_of_threads) {

    NUM_THREADS = number_of_threads;

    // Determing wich barrier algorithm to use
    if(barrierType.compare("sense") == 0) {
        bar = sense_barrier;
        cnt.store(0,RELAXED);
        sense.store(0,RELAXED);

    }
    else if(barrierType.compare("pthread") == 0) {
        bar = pthread_barrier;
        pthread_barrier_init(&p_bar, NULL, number_of_threads);

    }
    else {
        assert(false);
    }

}

Barriers::~Barriers() {
    
    // Destroying pothread barrier if being used
    if(bar == pthread_barrier) {

        pthread_barrier_destroy(&p_bar);

    }

}

// Sense reversal algorithm 
void Barriers::sense_wait(){

    thread_local bool my_sense = 0;
    if(my_sense == 0) {
        // Flip local sense
        my_sense = 1; 
    }
    else {
        my_sense = 0;
    }

    int cnt_cpy = cnt.fetch_add(1,SEQ_CST);

    // Last thread to arrive
    if(cnt_cpy == NUM_THREADS-1) { 
        cnt.store(0, RELAXED);
        sense.store(my_sense,SEQ_CST);
    }

    // Not last thread to arrive
    else { 
        while(sense.load(SEQ_CST) != my_sense) {}
    }

}

// Barrier algorithm handler function 
void Barriers::wait() {

    switch(bar) {

        case(sense_barrier):{
            sense_wait();
            break;
        }

        case(pthread_barrier):{         
            pthread_barrier_wait(&p_bar);
            break;
        }

        default:{
            break;
        }
    }

}