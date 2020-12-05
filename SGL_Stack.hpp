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


class SGL_Stack {

    private:
    Locks* lock;
    node* top;

    public:
    SGL_Stack();
    ~SGL_Stack();
    void push(int val);
    int pop();



};

#endif