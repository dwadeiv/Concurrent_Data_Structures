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
 *  Initialize the global lock  and dummy node. Set head and tail to equal tp dummy
 *
 * @note
 *  none
 *
 ******************************************************************************/
SGL_Queue::SGL_Queue() {

    Locks global_lock(LOCK);
    lock = &global_lock;

    node* dummy = new node;
    head = dummy;
    tail = dummy;

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

}

///////////////////////////////////   NEED TO IMPLEMENT OPTIMIZATION AND GARBAGE COLLECTION   ///////////////////////////////////////

/***************************************************************************//**
 * @brief
 *  This is the dequeue method for the SGL_Queue class
 *
 * @details
 *  Function dequeues and returns the value of the dequeued node
 *
 * @note
 * 	Function will return NULL if queue is empty
 *
 ******************************************************************************/
int SGL_Queue::dequeue() {



}

/***************************************************************************//**
 * @brief
 *  This is the enqueue method for the SGL_Queue class
 *
 * @details
 *  Places new node in the queue
 *
 * @note
 * 	none
 *
 * @param[in] val
 *  Value of the new node
 *
 ******************************************************************************/
void SGL_Queue::enqueue(int val) {

    node *imposter_tail, *true_end, *new_node;
    new_node = new node;
    new_node->val = val;

    while(true) {

        imposter_tail = tail;
        true_end = imposter_tail->next;

        if(imposter_tail == tail) {

            if(true_end == NULL) {

                if(true_end == imposter_tail->next) {

                    lock->acquire();

                    imposter_tail->next = new_node;

                    lock->release();

                    break;

                }

            }

            else if(true != NULL) {

                if(imposter_tail == tail) {

                    lock->acquire();

                    tail = true_end;

                    lock->release();

                }

            }

        }

    }

    if(imposter_tail == tail) {

        lock->acquire();

        tail = new_node;

        lock->release();

    }

}