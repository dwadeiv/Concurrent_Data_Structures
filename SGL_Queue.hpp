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
using std::string;

#define LOCK "ticket"

//***********************************************************************************
// data structure prototypes
//***********************************************************************************

struct node {

    int val;
    node* next;

};


class SGL_Queue {

    private:
    Locks* lock;
    node* head;
    node* tail;

    public:
    SGL_Queue();
    ~SGL_Queue();
    void enqueue(int val);
    int dequeue();



};

#endif
