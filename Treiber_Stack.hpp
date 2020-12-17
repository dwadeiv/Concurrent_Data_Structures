/*
 * Treiber_Stack.hpp
 *
 *  Created on: Dec 4th, 2020
 *      Author: David Wade IV
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#ifndef TS_HPP
#define TS_HPP
#include <iostream>
#include <atomic>


//***********************************************************************************
// defined files
//***********************************************************************************
using std::atomic;

#define ACQREL std::memory_order_acq_rel
#define ACQ std::memory_order_acquire

#define ELIMINATION_OPTIMIZATION_ON


//***********************************************************************************
// data structure prototypes
//***********************************************************************************

struct tstack_node {

    int val;
    tstack_node* next;

};

struct treiber_elimination_operations {

    bool pop;
    bool push;
    int push_value;

};

class T_stack{

    private:    
    atomic<tstack_node*> top;
    atomic<treiber_elimination_operations*>* elimination_array;
    int ELIM_ARRAY_SIZE;

    public:
    T_stack(int number_of_threads, int iterations);
    ~T_stack();
    void push(int val);
    tstack_node* pop();

};

#endif


