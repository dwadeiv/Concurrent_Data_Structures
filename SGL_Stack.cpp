/**
 * @file SGL_Stack.cpp
 * @author David Wade IV
 * @date December 4th, 2020
 * @brief Contains all the functions for a concurrent stack data structure that uses a Single Global Lock
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "SGL_Stack.hpp"

//***********************************************************************************
// functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *  This is the constructor for the SGL_Stack Class
 *
 * @details
 *  Initialize the global lock and set top to a new instance of the ls_node struct
 *
 * @note
 * none
 *
 ******************************************************************************/
SGL_stack::SGL_stack(int number_of_threads, int iterations) {

    ls_node* initialized_ls_node = NULL;
    top = initialized_ls_node;

    #ifdef ELIMINATION_OPTIMIZATION_ON

    ELIM_ARRAY_SIZE = (number_of_threads * iterations)/2;
    elimination_array = new atomic<sglS_operations*>[ELIM_ARRAY_SIZE];

    for(int i = 0; i < ELIM_ARRAY_SIZE; i++) {

        elimination_array[i].store(NULL);

    }

    #endif

    #ifdef FLAT_COMBINING_OPTIMIZATION_ON

    NUM_THREADS = number_of_threads;
    flat_combining_array = new atomic<sglS_operations*>[number_of_threads];

    for(int i = 0; i < number_of_threads; i++) {

        flat_combining_array[i].store(NULL);

    }

    #endif

}

/***************************************************************************//**
 * @brief
 *  This is the destructor for the SGL_Stack class
 *
 * @details
 *  none
 *
 * @note
 *  none
 *
 ******************************************************************************/
SGL_stack::~SGL_stack() {

    top = NULL;

    #ifdef ELIMINATION_OPTIMIZATION_ON

    delete [] elimination_array;

    #endif

    #ifdef FLAT_COMBINING_OPTIMIZATION_ON

    delete [] flat_combining_array;

    #endif

}

/***************************************************************************//**
 * @brief
 *  This is the push method for the Treiber_Stack class
 *
 * @details
 *  Pushes a new tstack_node to the top of the stack
 *
 * @note
 * 	none
 *
 * @param[in] val
 *  Value of the new top tstack_node
 *
 ******************************************************************************/
static void time_delay(int milliseconds) {

    clock_t start_time = clock();

    while(clock() < start_time + milliseconds) {};

}

/***************************************************************************//**
 * @brief
 *  This is the pop method for the SGL_Stack class
 *
 * @details
 *  Function pops the top of the stack and returns the value of the popped ls_node
 *
 * @note
 * 	Function will return NULL if stack is empty
 *
 ******************************************************************************/
int SGL_stack::pop(Locks* lock, int tid) {

    int value;

    while(true) {

        // Elimination optimization turned on
        #ifdef ELIMINATION_OPTIMIZATION_ON

        while(lock->try_acquire() == EBUSY) {
            
            int index = rand() % ELIM_ARRAY_SIZE;
            sglS_operations* elimination_index = elimination_array[index].load(ACQ);

            // Empty slot in elimination array
            if(elimination_index == NULL) {

                sglS_operations* operation = new sglS_operations;
                operation->pop = true;
                operation->push = false;

                // Place operation into array
                if(elimination_array[index].compare_exchange_strong(elimination_index, operation, ACQREL)) {

                    // Figure out how long to delay
                    time_delay(200);
                    sglS_operations* temp = elimination_array[index].load(ACQ);

                    // No one combined while waiting
                    if(temp != NULL) {

                        // Take operation out and try again on stack
                        elimination_array[index].compare_exchange_strong(temp, NULL);

                    }

                }

            }

            // Slot has a push operation
            else if(elimination_index->push) {

                // Perform matching operations and remove operation from elimination array
                if(elimination_array[index].compare_exchange_strong(elimination_index, NULL, ACQREL)) {

                    int return_value = elimination_index->push_value;
        
                    delete(elimination_index);
                    return return_value;

                }

            }

        }

        #endif


        #ifdef FLAT_COMBINING_OPTIMIZATION_ON

        // Non lock holder
        while(lock->try_acquire() == EBUSY) {

            // Not lock holder, so place operationinto combining array
            sglS_operations* operation = new sglS_operations;
            operation->pop = true;
            operation->push = false;

            while(true) {

                sglS_operations* current_index = flat_combining_array[tid].load(ACQ);
                if(flat_combining_array[tid].compare_exchange_strong(current_index,operation,ACQREL)) {

                    while(lock->try_acquire() == EBUSY) {}
                    break;

                }

            }

            break;

        }

        // lock holder
        
        for(int i = 0; i < NUM_THREADS; i++) {

            sglS_operations* current_operation = flat_combining_array[i].load(ACQ);

            if(current_operation != NULL) {

                if(flat_combining_array[i].compare_exchange_strong(current_operation,NULL)) {
                    
                    
                    if(current_operation->push) {

                        ls_node* updated_top = new ls_node;
                        updated_top->val = current_operation->push_value;
                        updated_top->next = top;
                        top = updated_top;

                        delete(current_operation);

                    }

                    else if(current_operation->pop) {

                        if(top != NULL) {

                            ls_node* temp = top;

                            int value = temp->val;

                            top = temp->next;

                            delete(temp);

                        }

                        delete(current_operation);

                    }   

                    
                }

            }

        }

        if(top != NULL) {

            ls_node* temp = top;
            top = temp->next;
            int value = temp->val;
            delete(temp);

            lock->release();

            return value;
        }

        else {

            lock->release();
            return -1;

        }

        #endif


        // Elimination optimization turned off
        #ifdef NO_OPTIMIZATION

        lock->acquire();

        if(top != NULL) {

            ls_node* temp = top;
            top = temp->next;
            int value = temp->val;
            delete(temp);

            lock->release();

            return value;
        }

        else {

            lock->release();
            return -1;

        }

        #endif

    }   


}

/***************************************************************************//**
 * @brief
 *  This is the push method for the SGL_Stack class
 *
 * @details
 *  Pushes a new ls_node to the top of the stack
 *
 * @note
 * 	none
 *
 * @param[in] val
 *  Value of the new top ls_node
 *
 ******************************************************************************/
void SGL_stack::push(int val, Locks* lock, int tid) {

    // Elimination optimization turned on
    #ifdef ELIMINATION_OPTIMIZATION_ON

    while(lock->try_acquire() == EBUSY) {
        
        int index = rand() % ELIM_ARRAY_SIZE;
        sglS_operations* elimination_index = elimination_array[index].load(ACQ);

        // Empty slot in elimination array
        if(elimination_index == NULL) {

            sglS_operations* operation = new sglS_operations;
            operation->pop = true;
            operation->push = false;
            operation->push_value = val;

            // Place operation into array
            if(elimination_array[index].compare_exchange_strong(elimination_index, operation, ACQREL)) {

                // Figure out how long to delay
                time_delay(200);
                sglS_operations* temp = elimination_array[index].load(ACQ);

                // No one combined while waiting
                if(temp != NULL) {

                    // Take operation out and try again on stack
                    elimination_array[index].compare_exchange_strong(temp, NULL);

                }

            }

        }

        // Slot has a push operation
        else if(elimination_index->pop) {

            // Perform matching operations and remove operation from elimination array
            if(elimination_array[index].compare_exchange_strong(elimination_index, NULL, ACQREL)) {

                int return_value = elimination_index->push_value;

                delete(elimination_index);
                delete(new_top);
                return;

            }

        }

    }

    new_top->next = top;

    top = new_top;

    lock->release();

    return;

    #endif


    #ifdef FLAT_COMBINING_OPTIMIZATION_ON

    // Non lock holder
    while(lock->try_acquire() == EBUSY) {

        // Not lock holder, so place operationinto combining array
        sglS_operations* operation = new sglS_operations;
        operation->pop = false;
        operation->push = true;
        operation->push_value = val;

        while(true) {

            sglS_operations* current_index = flat_combining_array[tid].load(ACQ);
            if(flat_combining_array[tid].compare_exchange_strong(current_index,operation,ACQREL)) {

                while(lock->try_acquire() == EBUSY) {}
                break;

            }

        }

        break;

    }

    for(int i = 0; i < NUM_THREADS; i++) {

        sglS_operations* current_operation = flat_combining_array[i].load(ACQ);

        if(current_operation != NULL) {

            if(flat_combining_array[i].compare_exchange_strong(current_operation,NULL)) {


                if(current_operation->push) {

                    ls_node* updated_top = new ls_node;
                    updated_top->val = current_operation->push_value;
                    updated_top->next = top;
                    top = updated_top;

                    delete(current_operation);

                }

                else if(current_operation->pop) {

                    if(top != NULL) {

                        ls_node* temp = top;

                        int value = temp->val;

                        top = temp->next;

                        delete(temp);

                    }

                    delete(current_operation);

                }   


            }

        }

    }

    ls_node* new_top = new ls_node;
    new_top->val = val;


    new_top->next = top;

    top = new_top;

    lock->release();

    return;

    #endif

    // Elimination and Flat Combining optimizations turned off
    #ifdef NO_OPTIMIZATION

    lock->acquire();

    new_top->next = top;

    top = new_top;

    lock->release();

    #endif

}