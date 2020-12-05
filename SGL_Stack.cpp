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
 *  Initialize the global lock and set top to a new instance of the node structß
 *
 * @note
 * none
 *
 ******************************************************************************/
SGL_Stack::SGL_Stack() {

    Locks global_lock(LOCK);
    lock = &global_lock;

    node* initialized_node = new node;
    top = initialized_node;

}

/***************************************************************************//**
 * @brief
 *  This is the destructor for the SGL-Stack class
 *
 * @details
 *  none
 *
 * @note
 *  none
 *
 ******************************************************************************/
SGL_Stack::~SGL_Stack() {

}

///////////////////////////////////   NEED TO IMPLEMENT OPTIMIZATION AND GARBAGE COLLECTION   ///////////////////////////////////////

/***************************************************************************//**
 * @brief
 *  This is the pop method for the SGL_Stack class
 *
 * @details
 *  Function pops the top of the stack and returns the value of the popped node
 *
 * @note
 * 	Function will return NULL if stack is empty
 *
 ******************************************************************************/
int SGL_Stack::pop() {

    while(true) {

        node* to_pop = top;

        if(to_pop == NULL) {

            return NULL;

        }

        node* new_top = to_pop->next;
        int value = to_pop->val;

        if(to_pop == top) {

            lock->acquire();

            top = new_top;

            lock->release();

            break;

        }

    }   


}

/***************************************************************************//**
 * @brief
 *  This is the push method for the SGL_Stack class
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
void SGL_Stack::push(int val) {

    node* new_top = new node;
    new_top->val = val;

    while(true) {

        node* old_top = top;
        new_top->next = old_top;

        if(old_top == top) {

            lock->acquire();

            top = new_top;

            lock->release();

            break;

        }

    }

}