/*
 * SGL_Queue.hpp
 *
 *  Created on: Dec 4th, 2020
 *      Author: David Wade IV
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#ifndef SGL_Q_HPP
#define SGL_Q_HPP

#include <iostream>
#include <atomic>
#include "Locks.hpp"

//***********************************************************************************
// defined files
//***********************************************************************************
using std::atomic;
using std::cout;
using std::endl;

#define ACQREL std::memory_order_acq_rel
#define ACQ std::memory_order_acquire

#define FLAT_COMBINING_OPTIMIZATION_ON

//***********************************************************************************
// data structure prototypes
//***********************************************************************************
struct lq_node {

    int val;
    lq_node* next;

};

struct sglQ_operations {

    bool dequeue;
    bool enqueue;
    int enqueue_value;

};

class SGL_Queue {

    private:
    Locks* lock;
    lq_node* head;
    lq_node* tail;
    atomic<sglQ_operations*>* flat_combining_array;
    int NUM_THREADS;

    public:
    SGL_Queue(int number_of_threads);
    ~SGL_Queue();
    void enqueue(int val, Locks* lock, int tid);
    int dequeue(Locks* lock, int tid);



};

#endif
