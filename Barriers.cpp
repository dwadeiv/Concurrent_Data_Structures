/**
 * @file Barriers.cpp
 * @author David Wade IV
 * @date October 18th, 2020
 * @brief Contains all the functions for the Barrier algorithms 
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "Barriers.hpp"

//***********************************************************************************
// functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *  This is the constructor for the Barriers Class
 *
 * @details
 *  Using the passed argument, figure out which barrier algorithm to utilize. Do the neccesary
 *  instantiations for whichever barrier is desired. 
 *
 * @note
 * none
 *
 ******************************************************************************/
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

/***************************************************************************//**
 * @brief
 *  This is the destructor for the Barriers class
 *
 * @details
 *  Destroys the pthread barrier if the pthread barrier was used
 *
 * @note
 *  none
 *
 ******************************************************************************/
Barriers::~Barriers() {
    
    // Destroying pothread barrier if being used
    if(bar == pthread_barrier) {

        pthread_barrier_destroy(&p_bar);

    }

}

/***************************************************************************//**
 * @brief
 *  This is the sense reversal algorithm
 *
 * @details
 *  Every thread has their own local sense. All threads prior to last thread
 *  will spin waiting for the global sense to flip which is triggered by the last thread arriving.
 *
 * @note
 *  none
 *
 ******************************************************************************/
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

/***************************************************************************//**
 * @brief
 *  This is the barrier algorithm handler function
 *
 * @details
 *  none
 *
 * @note
 *  none
 *
 ******************************************************************************/
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