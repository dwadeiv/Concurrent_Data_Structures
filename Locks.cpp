#include "Locks.hpp"

using std::string;

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

Locks::~Locks() {

    // Destroying pthread lock if it was created 
    if(lock == pthread_lock) {

        pthread_mutex_destroy(&p_lock);
        
    }

}

// Locks //

bool Locks::testAndSet() {

    bool expected = false;
    return flag.compare_exchange_strong(expected, true, SEQ_CST);

}

void Locks::tas_lock() {

    while(testAndSet() == false){}

}

void Locks::tas_unlock() {

    flag.store(false,SEQ_CST);

}

void Locks::ttas_lock() {

    while(flag.load(SEQ_CST) == true || testAndSet() == false){}

}

void Locks::ttas_unlock() {

    flag.store(false,SEQ_CST); 

}

void Locks::ticket_lock() {

    int my_num = next_num.fetch_add(1,SEQ_CST);
    while(now_serving.load(SEQ_CST) != my_num){}

}

void Locks::ticket_unlock() {

    now_serving.fetch_add(1,SEQ_CST);

}

// lock.acquire handler function
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

// lock.release handler function
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





