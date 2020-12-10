/*
 * Data_Structure_Tester.hpp
 *
 *  Created on: Dec 4th, 2020
 *      Author: David Wade IV
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#ifndef DS_TEST_HPP
#define DS_TEST_HPP

#include <iostream>
#include <pthread.h>
#include <assert.h>
#include <string>
#include <vector>
#include <cmath>
#include <map>
#include <atomic>
#include <limits.h>
#include "Locks.hpp"
#include "Barriers.hpp"
#include "MS_Queue.hpp"
#include "Treiber_Stack.hpp"
#include "SGL_Queue.hpp"
#include "SGL_Stack.hpp"

//***********************************************************************************
// defined files
//***********************************************************************************
using std::string;
using std::atomic;
using std::cout;
using std::endl;
using std::multimap;

#define BARRIER "sense"
#define LOCK "pthread"
#define ACQREL std::memory_order_acq_rel
#define ACQ std::memory_order_acquire

//***********************************************************************************
// data structure prototypes
//***********************************************************************************
struct DS_Tester_args {
	
	Locks *lock;
    Barriers *barrier;
    T_stack* t_stack;
    SGL_stack* s_stack;
    MS_queue* m_queue;
    SGL_Queue* s_queue;
    atomic<int>* epoch;
    atomic<int>* reservations;
	int tid;
	int number_of_threads;
    int iterations;

};

class DS_Tester {

    private:
    pthread_t *threads;

    public:
    DS_Tester(int number_of_threads);
    ~DS_Tester();
    void test(int iterations, int number_of_threads, string data_structure);

};

#endif