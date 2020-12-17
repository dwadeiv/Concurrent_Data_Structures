/**
 * @file Treiber_Stack.cpp
 * @author David Wade IV
 * @date December 4th, 2020
 * @brief Contains all the functions for the Treiber Stack concurrent ata structure
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "Treiber_Stack.hpp"

//***********************************************************************************
// functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *  This is the constructor for the Treiber_Stack Class
 *
 * @details
 *  Initialize the global lock and set top to a new instance of the tstack_node struct. If elimination optimization is enabled,
 *  also intializes the elimination array and fills it with NULL values. 
 *
 * @note
 * none
 *
 ******************************************************************************/
T_stack::T_stack(int number_of_threads, int iterations) {

    tstack_node* n = NULL;
    top.store(n, ACQREL);

    #ifdef ELIMINATION_OPTIMIZATION_ON

    ELIM_ARRAY_SIZE = (number_of_threads * iterations)/2;
    elimination_array = new atomic<treiber_elimination_operations*>[ELIM_ARRAY_SIZE];

    for(int i = 0; i < ELIM_ARRAY_SIZE; i++) {

        elimination_array[i].store(NULL);

    }

    #endif

}  

/***************************************************************************//**
 * @brief
 *  This is the destructor for the Treiber_Stack class
 *
 * @details
 *  Deletes the elimination array if the optimization is turned on
 *
 * @note
 *  none
 *
 ******************************************************************************/
T_stack::~T_stack() {

    top.store(NULL, ACQREL);

    #ifdef ELIMINATION_OPTIMIZATION_ON

    delete [] elimination_array;

    #endif

}

/***************************************************************************//**
 * @brief
 *  This is a time delay function. 
 *
 * @details
 *  none
 *
 * @note
 * 	none
 *
 * @param[in] milliseconds
 *  Length of delay in milliseconds
 *
 ******************************************************************************/
static void time_delay(int milliseconds) {

    clock_t start_time = clock();

    while(clock() < start_time + milliseconds) {};

}

/***************************************************************************//**
 * @brief
 *  This is the pop method for the Treiber_Stack class
 *
 * @details
 *  Function pops the top of the stack and returns the value of the popped tstack_node. Elimination optimization relieves 
 *  contention on the global lock. The lock holder performs their operation and returns, non lock holders pick a random index in the elimination array 
 *  and see what's in the index. If the complimentary operation is in the index, then the thread will perform both operations and return. If the index is NULL, 
 *  then the operation will place its operation in the index and wait for some delay. It then checks if its operation was commpleted by another thread. If not,
 *  it then contends on lock again. The process is repeated if the thread doesn't acquire the lock. 
 *
 * @note
 * 	Function will return NULL if stack is empty
 *
 ******************************************************************************/
tstack_node* T_stack::pop() {

    while(true){

        tstack_node* to_pop = top.load(ACQ);

        if(to_pop == NULL) {

            tstack_node* end_signal = new tstack_node;
            end_signal->val = -1;

            return end_signal;
             
        }
        
        tstack_node* new_top = to_pop->next;

        bool success = top.compare_exchange_strong(to_pop, new_top, ACQREL);
        if(success) {

            return to_pop;

        }

        #ifdef ELIMINATION_OPTIMIZATION_ON
        // Elimination optimization
        else {

            while(true) {

                int index = rand() % ELIM_ARRAY_SIZE;
                treiber_elimination_operations* elimination_index = elimination_array[index].load(ACQ);

                // Empty slot in elimination array
                if(elimination_index == NULL) {

                    treiber_elimination_operations* operation = new treiber_elimination_operations;
                    operation->pop = true;
                    operation->push = false;

                    // Place operation into array
                    if(elimination_array[index].compare_exchange_strong(elimination_index, operation, ACQREL)) {

                        // Figure out how long to delay
                        time_delay(200);
                        treiber_elimination_operations* temp = elimination_array[index].load(ACQ);

                        // No one combined while waiting
                        if(temp != NULL) {

                            // Take operation out and try again on stack
                            elimination_array[index].compare_exchange_strong(temp, NULL);
                            tstack_node* return_node = pop();
                            return return_node;

                        }

                    }

                }

                // Slot has a push operation
                else if(elimination_index->push) {

                    // Perform matching operations and remove operation from elimination array
                    if(elimination_array[index].compare_exchange_strong(elimination_index, NULL, ACQREL)) {

                        int value = elimination_index->push_value;
                        tstack_node* return_node = new tstack_node;
                        return_node->val = value;
            
                        delete(elimination_index);
                        return return_node;

                    }

                }

            }

        }

        #endif

    }

}

/***************************************************************************//**
 * @brief
 *  This is the push method for the Treiber_Stack class
 *
 * @details
 *  Pushes a new tstack_node to the top of the stack. Elimination optimization relieves contention on the global lock. 
 *  The lock holder performs their operation and returns, non lock holders pick a random index in the elimination array 
 *  and see what's in the index. If the complimentary operation is in the index, then the thread will perform both operations and return. If the index is NULL, 
 *  then the operation will place its operation in the index and wait for some delay. It then checks if its operation was commpleted by another thread. If not,
 *  it then contends on lock again. The process is repeated if the thread doesn't acquire the lock. 
 *
 * @note
 * 	none
 *
 * @param[in] val
 *  Value of the new top tstack_node
 *
 ******************************************************************************/
void T_stack::push(int val) {

    tstack_node* new_top = new tstack_node;
    new_top->val = val;

    while(true) {

        tstack_node* old_top = top.load(ACQ);
        new_top->next = old_top;

        bool success = top.compare_exchange_strong(old_top, new_top, ACQREL); 
        
        if(success){

            break;

        }

        #ifdef ELIMINATION_OPTIMIZATION_ON
        else {

            while(true) {

                int index = rand() % ELIM_ARRAY_SIZE;
                treiber_elimination_operations* elimination_index = elimination_array[index].load(ACQ);

                // Empty slot in elimination array
                if(elimination_index == NULL) {

                    treiber_elimination_operations* operation = new treiber_elimination_operations;
                    operation->pop = false;
                    operation->push = true;
                    operation->push_value = val;

                    // Place operation into array
                    if(elimination_array[index].compare_exchange_strong(elimination_index, operation, ACQREL)) {

                        // Figure out how long to delay
                        time_delay(200);
                        treiber_elimination_operations* temp = elimination_array[index].load(ACQ);

                        // No one combined while waiting
                        if(temp != NULL) {

                            // Take operation out and try again on stack
                            elimination_array[index].compare_exchange_strong(temp, NULL);
                            push(val);
                            return;

                        }

                    }

                }

                // Slot has a push operation
                else if(elimination_index->pop) {

                    // Perform matching operations and remove operation from elimination array
                    if(elimination_array[index].compare_exchange_strong(elimination_index, NULL, ACQREL)) {
            
                        delete(new_top);
                        delete(elimination_index);
                        return;

                    }

                }

            }

        }

        #endif

    }

}