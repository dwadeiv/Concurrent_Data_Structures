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
using std::string;
using std::cout;
using std::endl;
using std::atomic;

#define ACQREL std::memory_order_acq_rel
#define ACQ std::memory_order_acquire


//***********************************************************************************
// data structure prototypes
//***********************************************************************************
struct node {

    int val;
    node* next;

};

class T_stack{

    private:
    atomic<node*> top;

    public:
    T_stack();
    ~T_stack();
    void push(int val);
    int pop();

};

#endif
