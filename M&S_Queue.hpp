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
    atomic<node*> next;

};

class MS_queue{

    private:
    atomic<node*> head, tail;

    public:
    MS_queue();
    ~MS_queue();
    void enqueue(int val);
    int dequeue();

};

#endif                   