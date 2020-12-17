/*
 * M&S_Queue.hpp
 *
 *  Created on: Dec 4th, 2020
 *      Author: David Wade IV
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#ifndef MS_HPP
#define MS_HPP

#include <iostream>
#include <atomic>

//***********************************************************************************
// defined files
//***********************************************************************************
using std::string;
using std::atomic;
using std::cout;
using std::endl;
#define ACQREL std::memory_order_acq_rel
#define ACQ std::memory_order_acquire
#define RELAXED std::memory_order_relaxed

//***********************************************************************************
// data structure prototypes
//***********************************************************************************
struct ms_node {

    int val;
    atomic<ms_node*> next;

};

class MS_queue{

    private:
    atomic<ms_node*> head, tail;

    public:
    MS_queue();
    ~MS_queue();
    void enqueue(int val);
    ms_node* dequeue();

};

#endif                   