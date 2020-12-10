/**
 * @file SGL_Queue.cpp
 * @author David Wade IV
 * @date December 4th, 2020
 * @brief Contains all the functions for a concurrent queue data structure that uses a Single Global Lock
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "SGL_Queue.hpp"

//***********************************************************************************
// functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *  This is the constructor for the SGL_Stack Class
 *
 * @details
 *  Initialize the global lock  and dummy lq_node. Set head and tail to equal tp dummy
 *
 * @note
 *  none
 *
 ******************************************************************************/
SGL_Queue::SGL_Queue(int number_of_threads) {

    head = NULL;
    tail = NULL;

    #ifdef FLAT_COMBINING_OPTIMIZATION_ON

    NUM_THREADS = number_of_threads;
    flat_combining_array = new atomic<sglQ_operations*>[number_of_threads];

    for(int i = 0; i < number_of_threads; i++) {

        flat_combining_array[i].store(NULL);

    }

    #endif

}

/***************************************************************************//**
 * @brief
 *  This is the destructor for the SGL_Queue class
 *
 * @details
 *  none
 *
 * @note
 *  none
 *
 ******************************************************************************/
SGL_Queue::~SGL_Queue() {

    #ifdef FLAT_COMBINING_OPTIMIZATION_ON

    delete [] flat_combining_array;

    #endif

}

/***************************************************************************//**
 * @brief
 *  This is the dequeue method for the SGL_Queue class
 *
 * @details
 *  Function dequeues and returns the value of the dequeued lq_node
 *
 * @note
 * 	Function will return NULL if queue is empty
 *
 ******************************************************************************/
int SGL_Queue::dequeue(Locks* lock, int tid) {

    #ifdef FLAT_COMBINING_OPTIMIZATION_ON

    // Non lock holder
    while(lock->try_acquire() == EBUSY) {

        // Not lock holder, so place operationinto combining array
        sglQ_operations* operation = new sglQ_operations;
        operation->dequeue = true;
        operation->enqueue = false;

        while(true) {

            sglQ_operations* current_index = flat_combining_array[tid].load(ACQ);
            if(flat_combining_array[tid].compare_exchange_strong(current_index,operation,ACQREL)) {

                while(lock->try_acquire() == EBUSY) {}
                break;

            }

        }

        break;

    }

    for(int i = 0; i < NUM_THREADS; i++) {

        sglQ_operations* current_operation = flat_combining_array[i].load(ACQ);

        if(current_operation != NULL) {

            if(flat_combining_array[i].compare_exchange_strong(current_operation,NULL)) {

                if(current_operation->enqueue) {

                    lq_node* new_lq_node = new lq_node;
                    new_lq_node->val = current_operation->enqueue_value;
                    new_lq_node->next = NULL;

                    if(!head || !tail) {

                        head = new_lq_node;
                        tail = new_lq_node;

                    }

                    else {

                        tail->next = new_lq_node;
                        tail = new_lq_node;

                    }

                    delete(current_operation);

                }

                else if(current_operation->dequeue) {

                    // Checking if there are contents in the queue
                    if(!(!head || !tail)) {

                        if(head == tail) {

                            int value = head->val;
                            head = NULL;
                            tail = NULL;

                        }
                        else {

                            lq_node* temp = head;
                            head = head->next;
                            int value = temp->val;
                            delete(temp);

                        }

                    }

                    delete(current_operation);

                }

            }

        }

    }

    // Checking if there are contents in the queue
    if(!(!head || !tail)) {

        if(head == tail) {

            int value = head->val;
            delete(head);
            head = NULL;
            tail = NULL;
            lock->release();
            return value;

        }
        else {

            lq_node* temp = head;
            head = head->next;
            int value = temp->val;
            delete(temp);
            lock->release();
            return value;

        }

    }

    // Queue is empty
    else {

        lock->release();
        return -1;


    }

    lock->release();

    #else

    lock->acquire();

    // Checking if there are contents in the queue
    if(!(!head || !tail)) {

        if(head == tail) {

            int value = head->val;
            delete(head);
            head = NULL;
            tail = NULL;
            lock->release();
            return value;

        }
        else {

            lq_node* temp = head;
            head = head->next;
            int value = temp->val;
            delete(temp);
            lock->release();
            return value;

        }

    }

    // Queue is empty
    else {

        lock->release();
        return -1;


    }

    lock->release();

    #endif

}

/***************************************************************************//**
 * @brief
 *  This is the enqueue method for the SGL_Queue class
 *
 * @details
 *  Places new lq_node in the queue
 *
 * @note
 * 	none
 *
 * @param[in] val
 *  Value of the new lq_node
 *
 ******************************************************************************/
void SGL_Queue::enqueue(int val, Locks* lock, int tid) {

    lq_node *new_lq_node;
    new_lq_node = new lq_node;
    new_lq_node->val = val;
    new_lq_node->next = NULL;

    #ifdef FLAT_COMBINING_OPTIMIZATION_ON

    // Non lock holder
    while(lock->try_acquire() == EBUSY) {

        // Not lock holder, so place operationinto combining array
        sglQ_operations* operation = new sglQ_operations;
        operation->dequeue = false;
        operation->enqueue = true;
        operation->enqueue_value = val;

        while(true) {

            sglQ_operations* current_index = flat_combining_array[tid].load(ACQ);
            if(flat_combining_array[tid].compare_exchange_strong(current_index,operation,ACQREL)) {

                while(lock->try_acquire() == EBUSY) {}
                break;

            }

        }

        break;

    }

    for(int i = 0; i < NUM_THREADS; i++) {

        sglQ_operations* current_operation = flat_combining_array[i].load(ACQ);

        if(current_operation != NULL) {

            if(flat_combining_array[i].compare_exchange_strong(current_operation,NULL)) {

                if(current_operation->enqueue) {

                    if(!head || !tail) {

                        head = new_lq_node;
                        tail = new_lq_node;

                    }

                    else {

                        tail->next = new_lq_node;
                        tail = new_lq_node;

                    }

                    delete(current_operation);

                }

                else if(current_operation->dequeue) {

                    // Checking if there are contents in the queue
                    if(!(!head || !tail)) {

                        if(head == tail) {

                            int value = head->val;
                            head = NULL;
                            tail = NULL;

                        }
                        else {

                            lq_node* temp = head;
                            head = head->next;
                            int value = temp->val;
                            delete(temp);

                        }

                    }
                    delete(current_operation);

                }

            }

        }

    }


    if(!head || !tail) {

        head = new_lq_node;
        tail = new_lq_node;

    }

    else {

        tail->next = new_lq_node;
        tail = new_lq_node;

    }

    lock->release();

    return;

    #else

    lock->acquire();

    if(!head || !tail) {

        head = new_lq_node;
        tail = new_lq_node;

    }

    else {

        tail->next = new_lq_node;
        tail = new_lq_node;

    }

    lock->release();

    #endif

}
