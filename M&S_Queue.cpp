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
#include "M&S_Queue.hpp"

//***********************************************************************************
// functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *  This is the constructor for the M&S_Stack Class
 *
 * @details
 *  Initialize the dummy node and set head and tail equal to dummy
 *
 * @note
 *  none
 *
 ******************************************************************************/
MS_queue::MS_queue() {
    node* dummy = new node;
    head.store(dummy, ACQREL);
    tail.store(dummy, ACQREL);
}

/***************************************************************************//**
 * @brief
 *  This is the destructor for the M&S_Queue class
 *
 * @details
 *  none
 *
 * @note
 *  none
 *
 ******************************************************************************/
MS_queue::~MS_queue() {
    head.store(NULL, ACQREL);
    tail.store(NULL, ACQREL);
}

///////////////////////////////////   NEED TO IMPLEMENT OPTIMIZATION AND GARBAGE COLLECTION   ///////////////////////////////////////

/***************************************************************************//**
 * @brief
 *  This is the enqueue method for the M&S_Queue class
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
void MS_queue::enqueue(int val) {

    node *imposter_tail, *true_end, *new_node;
    new_node = new node;
    new_node->val = val;

    while(true) {

        imposter_tail = tail.load(ACQ);
        true_end = imposter_tail->next.load(ACQ);

        if(imposter_tail == tail.load(ACQ)) {

            ///// Ask about if its true_end or NULL /////
            if(true_end == NULL && imposter_tail->next.compare_exchange_strong(true_end, new_node)) {
                
                break;

            }

            else if(true_end != NULL) {

                tail.compare_exchange_strong(imposter_tail, true_end);

            }

        }

    }

    tail.compare_exchange_strong(imposter_tail, new_node);

}

/***************************************************************************//**
 * @brief
 *  This is the dequeue method for the M&S_Queue class
 *
 * @details
 *  Function dequeues and returns the value of the dequeued node
 *
 * @note
 * 	Function will return NULL if queue is empty
 *
 ******************************************************************************/
int MS_queue::dequeue() {

    node *dummy, *new_dummy, *imposter_tail;

    while(true) {

        dummy = head.load(ACQREL);
        imposter_tail = tail.load(ACQREL);
        new_dummy = head.load(ACQREL)->next.load();


        if(dummy = head.load(ACQREL)) {

            if(dummy = imposter_tail) {

                if(new_dummy == NULL) {

                    return NULL;

                }

                else {

                    tail.compare_exchange_strong(dummy, new_dummy);

                }

            }

            else {

                int ret = new_dummy->val;
                if(head.compare_exchange_strong(dummy, new_dummy));

            }

        }

    } 

}