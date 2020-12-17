/**
 * @file M&S_Queue.cpp
 * @author David Wade IV
 * @date December 4th, 2020
 * @brief Contains all the functions for the Michael & Scott concurrent queue data structure
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "MS_Queue.hpp"

//***********************************************************************************
// functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *  This is the constructor for the M&S_Stack Class
 *
 * @details
 *  Initialize the dummy ms_node and set head and tail equal to dummy
 *
 * @note
 *  none
 *
 ******************************************************************************/
MS_queue::MS_queue() {
    ms_node* dummy = new ms_node;
    dummy->val = 21;
    dummy->next = NULL;
    head.store(dummy, RELAXED);
    tail.store(dummy, RELAXED);
}

/***************************************************************************//**
 * @brief
 *  This is the destructor for the M&S_Queue class
 *
 * @details
 *  delete head node
 *
 * @note
 *  none
 *
 ******************************************************************************/
MS_queue::~MS_queue() {

    ms_node* garbage = head.load();
    delete(garbage);

}

/***************************************************************************//**
 * @brief
 *  This is the enqueue method for the M&S_Queue class
 *
 * @details
 *  Places new ms_node in the queue. Uses atomic operators (CAS) to create linearization points for the threads. 
 *
 * @note
 * 	Funtion attempts to update imposter tail, but update is not guaranteed
 *
 * @param[in] val
 *  Value of the new ms_node
 *
 ******************************************************************************/
void MS_queue::enqueue(int val) {

    ms_node *imposter_tail, *true_end, *new_ms_node;
    new_ms_node = new ms_node;
    new_ms_node->val = val;

    while(true) {

        imposter_tail = tail.load(ACQ);

        true_end = imposter_tail->next.load(ACQ);


        if(imposter_tail == tail.load(ACQ)) {

            if(true_end == NULL && imposter_tail->next.compare_exchange_strong(true_end, new_ms_node)) {
                
                break;

            }
            

            else if(true_end != NULL) {

                tail.compare_exchange_strong(imposter_tail, true_end);

            }

        }

    }

    tail.compare_exchange_strong(imposter_tail, new_ms_node);

}

/***************************************************************************//**
 * @brief
 *  This is the dequeue method for the M&S_Queue class
 *
 * @details
 *  Function dequeues and returns the value of the dequeued ms_node. Uses atomic operators (CAS) to create linearization points for the threads.
 *
 * @note
 * 	Function will return a node with a value of -1 when the queue is empty
 *
 ******************************************************************************/
ms_node* MS_queue::dequeue() {

    ms_node *dummy, *new_dummy, *imposter_tail;

    while(true) {

        dummy = head.load(ACQ);
        imposter_tail = tail.load(ACQ);
        new_dummy = dummy->next.load(ACQ);


        if(dummy == head.load(ACQ)) {

            if(dummy == imposter_tail) {

                if(new_dummy == NULL) {

                    dummy->val = -1;
                    return dummy;

                }

                else {

                    tail.compare_exchange_strong(imposter_tail, new_dummy, ACQREL);

                }

            }

            else {

                int ret = new_dummy->val;
                if(head.compare_exchange_strong(dummy, new_dummy, ACQREL)) {
                    return dummy;
                }

            }

        }

    } 

}