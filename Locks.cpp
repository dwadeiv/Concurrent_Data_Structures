/**
 * @file Locks.cpp
 * @author David Wade IV
 * @date October 18th, 2020
 * @brief Contains all the functions for the Lock algorithms
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "Locks.hpp"

//***********************************************************************************
// functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *  This is the constructor for the Locks Class
 *
 * @details
 *  Using the passed argument, figure out which lock algorithm to utilize. Do the neccesary
 *  instantiations for whichever lock is desired. 
 *
 * @note
 * none
 *
 ******************************************************************************/
Locks::Locks(string lockType) {

    // Determing which locking algorithm to use
    if(lockType.compare("tas") == 0) {
        lock = tas;
        flag.store(false,RELAXED);
    }
    else if(lockType.compare("ttas") == 0) {
        lock = ttas;
        flag.store(false,RELAXED);
    }
    else if(lockType.compare("ticket") == 0) {
        lock = ticket;
        next_num.store(0,RELAXED);
        now_serving.store(0,RELAXED);
    }
    else if(lockType.compare("pthread") == 0) {
        lock = pthread_lock;
        pthread_mutex_init(&p_lock, NULL);
    }
    else {
        assert(false);
    }

}

/***************************************************************************//**
 * @brief
 *  This is the destructor for the Locks class
 *
 * @details
 *  Destroys the pthread lock if the pthread lock was used
 *
 * @note
 *  none
 *
 ******************************************************************************/
Locks::~Locks() {

    if(lock == pthread_lock) {

        pthread_mutex_destroy(&p_lock);
        
    }

}

/***************************************************************************//**
 * @brief
 *  This is the test and set method used by the tas and ttas locks
 *
 * @details
 *  Checks true if flag is false and sets flag to true. Returns flase if flag is held. 
 *
 * @note
 *  none
 *
 ******************************************************************************/
bool Locks::testAndSet() {

    bool expected = false;
    return flag.compare_exchange_strong(expected, true, SEQ_CST);

}

/***************************************************************************//**
 * @brief
 *  This is the test and set lock.
 *
 * @details
 *  Uses the test and set method to contend on flag
 *
 * @note
 *  none
 *
 ******************************************************************************/
void Locks::tas_lock() {

    while(testAndSet() == false){}

}

/***************************************************************************//**
 * @brief
 *  This is the test and set unclok.
 *
 * @details
 *  Makes flag equal to false
 *
 * @note
 *  none
 *
 ******************************************************************************/
void Locks::tas_unlock() {

    flag.store(false,SEQ_CST);

}

/***************************************************************************//**
 * @brief
 *  This is the test and test and set lock.
 *
 * @details
 *  Continually checks value of flag to see if it is released 
 *  and uses the test and set method to contend on flag. Thevalue checking relieves contention 
 *  on the cache line of flag. 
 *
 * @note
 *  none
 *
 ******************************************************************************/
void Locks::ttas_lock() {

    while(flag.load(SEQ_CST) == true || testAndSet() == false){}

}

/***************************************************************************//**
 * @brief
 *  This is the test and test and set unclok.
 *
 * @details
 *  Makes flag equal to false
 *
 * @note
 *  none
 *
 ******************************************************************************/
void Locks::ttas_unlock() {

    flag.store(false,SEQ_CST); 

}

/***************************************************************************//**
 * @brief
 *  This is the ticket lock method
 *
 * @details
 *  Each thread will have their own number. Threads will spin on themselves until
 *  their number is being serviced. This is a fair FIFO lock
 *
 * @note
 *  none
 *
 ******************************************************************************/
void Locks::ticket_lock() {

    int my_num = next_num.fetch_add(1,SEQ_CST);
    while(now_serving.load(SEQ_CST) != my_num){}

}

/***************************************************************************//**
 * @brief
 *  This is the ticket unlock method
 *
 * @details
 *  Changes the value of now serving to service next thread in line
 *
 * @note
 *  none
 *
 ******************************************************************************/
void Locks::ticket_unlock() {

    now_serving.fetch_add(1,SEQ_CST);

}

/***************************************************************************//**
 * @brief
 *  This is the try_acquire method rfor the pthread lock.
 *
 * @details
 *  Attempts to acquire lock and returns int that indicates succesws or different types of failure
 *
 * @note
 *  none
 *
 ******************************************************************************/
int Locks::try_acquire() {

    pthread_mutex_trylock(&p_lock);

}

/***************************************************************************//**
 * @brief
 *  This is the lock.acquire handler function
 *
 * @details
 *  none
 *
 * @note
 *  none
 *
 ******************************************************************************/
void Locks::acquire() {

    switch(lock) {

        case(tas):{
            tas_lock();
            break;
        }

        case(ttas):{         
            ttas_lock();
            break;
        }

        case(ticket):{           
            ticket_lock();
            break;
        }

        case(pthread_lock):{           
            pthread_mutex_lock(&p_lock);
            break;
        }

        default:{
            assert(false);
            break;
        }
    }
}

/***************************************************************************//**
 * @brief
 *  This is the lock.release handler function
 *
 * @details
 *  none
 *
 * @note
 *  none
 *
 ******************************************************************************/
void Locks::release() {

    switch(lock) {

        case(tas):{
            tas_unlock();
            break;
        }

        case(ttas):{
            ttas_unlock();
            break;
        }

        case(ticket):{           
            ticket_unlock();
            break;
        }

        case(pthread_lock):{
            pthread_mutex_unlock(&p_lock);
            break;
        }

        default:{
            assert(false);
            break;
        }
    }
}





