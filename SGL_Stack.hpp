/*
 * SGL_Stack.hpp
 *
 *  Created on: Dec 4th, 2020
 *      Author: David Wade IV
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#ifndef SGL_S_HPP
#define SGL_S_HPP

#include <iostream>
#include <atomic>
#include <vector>
#include "Locks.hpp"

//***********************************************************************************
// defined files
//***********************************************************************************
using std::atomic;

#define ACQREL std::memory_order_acq_rel
#define ACQ std::memory_order_acquire

// #define ELIMINATION_OPTIMIZATION_ON
#define FLAT_COMBINING_OPTIMIZATION_ON
// #define NO_OPTIMIZATION

//***********************************************************************************
// data structure prototypes
//***********************************************************************************
struct ls_node {

    int val;
    ls_node* next;

};

struct sglS_operations {

    bool pop;
    bool push;
    int push_value;

};

class SGL_stack {

    private:
    Locks* lock;
    ls_node* top;
    atomic<sglS_operations*>* elimination_array;
    int ELIM_ARRAY_SIZE;
    atomic<sglS_operations*>* flat_combining_array;
    int NUM_THREADS;

    public:
    SGL_stack(int number_of_threads, int iterations);
    ~SGL_stack();
    void push(int val, Locks* lock, int tid);
    int pop(Locks* lock, int tid);

};

#endif

