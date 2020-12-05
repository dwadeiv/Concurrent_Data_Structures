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
 *  Initialize the global lock and set top to a new instance of the node structß
 *
 * @note
 * none
 *
 ******************************************************************************/
T_stack::T_stack() {

    node* n = new node;
    top.store(n, ACQREL);
}  

/***************************************************************************//**
 * @brief
 *  This is the destructor for the Treiber_Stack class
 *
 * @details
 *  none
 *
 * @note
 *  none
 *
 ******************************************************************************/
T_stack::~T_stack() {

    top.store(NULL, ACQREL);

}

///////////////////////////////////   NEED TO IMPLEMENT OPTIMIZATION AND GARBAGE COLLECTION   ///////////////////////////////////////

/***************************************************************************//**
 * @brief
 *  This is the pop method for the Treiber_Stack class
 *
 * @details
 *  Function pops the top of the stack and returns the value of the popped node
 *
 * @note
 * 	Function will return NULL if stack is empty
 *
 ******************************************************************************/
int T_stack::pop() {

    int value;

    while(true){

        node* to_pop = top.load(ACQ);
        to_pop = top.load(ACQ);

        if(to_pop == NULL) {

            return NULL;
             
        }
        
        node* new_top = to_pop->next;
        value = to_pop->val;

        if(top.compare_exchange_strong(to_pop, new_top, ACQREL)) {

            break;

        }

    }

    return value;
}

/***************************************************************************//**
 * @brief
 *  This is the push method for the Treiber_Stack class
 *
 * @details
 *  Pushes a new node to the top of the stack
 *
 * @note
 * 	none
 *
 * @param[in] val
 *  Value of the new top node
 *
 ******************************************************************************/
void T_stack::push(int val) {

    node* new_top = new node;
    new_top->val = val;

    while(true) {

        node* old_top = top.load(ACQ);
        new_top->next = old_top;

        if(top.compare_exchange_strong(old_top, new_top, ACQREL)) {

            break;

        }

    }

}