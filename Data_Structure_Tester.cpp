/**
 * @file Data_Structure_Tester.cpp
 * @author David Wade IV
 * @date December 4th, 2020
 * @brief Contains all the functions for testing the concurrent data structures
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "Data_Structure_Tester.hpp"

//***********************************************************************************
// static functions/variables
//***********************************************************************************
struct timespec start, finish;


//***********************************************************************************
// functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *  This is the constructor for the DS_Tester Class
 *
 * @details
 *  
 *
 * @note
 * none
 *
 ******************************************************************************/
DS_Tester::DS_Tester(int number_of_threads) {

    threads = (pthread_t*)(malloc(number_of_threads*sizeof(pthread_t)));

}

/***************************************************************************//**
 * @brief
 *  This is the destructor for the DS_Tester class
 *
 * @details
 *  none
 *
 * @note
 *  none
 *
 ******************************************************************************/
DS_Tester::~DS_Tester() {

    free(threads);

}

/***************************************************************************//**
 * @brief
 *  This is the garbage collection method for the treiber stack. 
 *
 * @details
 *  Finds the minimum epoch in which nodes can be freed, then scans the retired nodes list to free respectively.
 *
 * @note
 * 	Each thread cleans up their own garbage
 *
 * @param[in] retired_nodes
 *  List of retired nodes waiting to be freed
 *
 * @param[in] reservations
 *  List of each node's current epoch
 * 
 * @param[in] number_of_threads
 *  Number of threads
 * 
 ******************************************************************************/ 
static void extermination_treiber(multimap<int,tstack_node*> &retired_nodes, atomic<int>* reservations, int number_of_threads) {

    int min = INT_MAX; 

    for(int i = 0; i < number_of_threads; i++) {

        if(min > reservations[i].load(ACQ)) {

            min = reservations[i].load(ACQ);

        }

    }
    
    auto itr = retired_nodes.begin();
    itr++;

    for(itr; itr != retired_nodes.end(); itr++) {

        if(min > itr->first) {

            auto itcopy = itr;
            itcopy--;
            delete(itr->second);
            retired_nodes.erase(itr);
            itr = itcopy;

        }

    }

}

/***************************************************************************//**
 * @brief
 *  This is the garbage collection method for the m&s queue. 
 *
 * @details
 *  Finds the minimum epoch in which nodes can be freed, then scans the retired nodes list to free respectively.
 *
 * @note
 * 	Each thread cleans up their own garbage
 *
 * @param[in] retired_nodes
 *  List of retired nodes waiting to be freed
 *
 * @param[in] reservations
 *  List of each node's current epoch
 * 
 * @param[in] number_of_threads
 *  Number of threads
 * 
 ******************************************************************************/ 
static void extermination_ms(multimap<int,ms_node*> retired_nodes, atomic<int>* reservations, int number_of_threads) {

    int min = INT_MAX; 

    for(int i = 0; i < number_of_threads; i++) {

        if(min > reservations[i].load(ACQ)) {

            min = reservations[i].load(ACQ);

        }

    }

    for(auto itr = retired_nodes.begin(); itr != retired_nodes.end(); itr++) {

        if(min > itr->first) {

            auto itcopy = itr;
            itcopy--;
            delete(itr->second);
            retired_nodes.erase(itr);
            itr = itcopy;

        }

    }

}

/***************************************************************************//**
 * @brief
 *  This it the fork for the parallel testing of the Treiber Stack
 *
 * @details
 *  Pushes numbers from a counter for the given number of iterations. Then immediatly pops for the given
 *  number of iterations. This method uses epoch based reclamation for garbage collection. There are two counters that are used in this methhod. 
 *  One that is in charge of when garbage collection occurs and the other for when the global epoch is increased.
 *
 * @note
 * 	Each thread will execute this method individually
 *
 * @param[in] args
 *  
 *
 ******************************************************************************/ 
static void* fork_Treiber(void* args){

	// Extracting arguments from the passed struct
	DS_Tester_args* inArgs = (DS_Tester_args*)args;
	Locks* lock = inArgs->lock;
    Barriers* bar = inArgs->barrier;
    T_stack* t_stack = inArgs->t_stack;
    SGL_stack* s_stack = inArgs->s_stack;
    MS_queue* m_queue = inArgs->m_queue;
    SGL_Queue* s_queue = inArgs->s_queue;
    atomic<int>* epoch = inArgs->epoch;
    atomic<int>* reservations = inArgs->reservations;
	int tid = inArgs->tid;
	int number_of_threads = inArgs->number_of_threads;
    int iterations = inArgs->iterations;

    thread_local multimap<int,tstack_node*> retired_nodes; 
    int value = tid;
    int garbage_genocide_countdown = 500;
    int time_shift_countdown = 100;

	bar->wait();
    
    // START CLOCK
    if(tid == 0) {
        clock_gettime(CLOCK_MONOTONIC,&start);
    }
    
    for(int i = 0; i < iterations; i ++) {
    
        t_stack->push(value);
        value += 1;

    }

    while(true){

        int time = epoch[0].load(ACQ);
        reservations[tid].store(time,ACQREL);

        tstack_node* to_retire = t_stack->pop();

        // Making sure the value isn't the end saginal 
        if(to_retire->val != -1) {

            bool flag = false;
            for(auto itr = retired_nodes.begin(); itr != retired_nodes.end(); itr++) {
                
                if((tstack_node*)itr->second == to_retire) {
                    flag = true;
                    break;
                    
                }
                
            }
            
            if(flag == false) {
             
                retired_nodes.insert({time, to_retire});
                
            }
            
            
            
        }

        garbage_genocide_countdown += -1;
        time_shift_countdown += -1;
        
        // performiong garbage collection when either garbage collection countdown hits zero or if at the end of all pops
        if(garbage_genocide_countdown == 0 || to_retire->val == -1) {

            if(retired_nodes.size() > 0) {
                
                extermination_treiber(retired_nodes, reservations, number_of_threads);

            }
            garbage_genocide_countdown = 500;
            
            if(to_retire->val == -1) {
                delete(to_retire);
                break;
            }

        }
        
        // When time shift countdown is zzero, tid 0 will increment the global epoch
        if(tid == 0 && time_shift_countdown == 0) {
            
            epoch[0].store(time + 1, ACQREL);
            time_shift_countdown = 100;
            
        }


    }
    
    
    // Making sure all retired nodees are freed before exiting
    while(retired_nodes.begin() != retired_nodes.end()) {
        auto itr = retired_nodes.begin();
        delete(itr->second);
        retired_nodes.erase(itr);
        
    }
    
	bar->wait();

    // END CLOCK
    if(tid == 0) {
        clock_gettime(CLOCK_MONOTONIC,&finish);
    }

}

/***************************************************************************//**
 * @brief
 *  This it the fork for the parallel testing of the M&S Queue
 *
 * @details
 *  Enqueues numbers from a counter for the given number of iterations. Then immediatly dequeues for the given
 *  number of iterations. This method uses epoch based reclamation for garbage collection. There are two counters that are used in this methhod. 
 *  One that is in charge of when garbage collection occurs and the other for when the global epoch is increased.
 *
 * @note
 * 	Each thread will execute this method individually. An end signal of -1 is used to know when all pops have occured. 
 *
 * @param[in] args
 *  
 *
 ******************************************************************************/ 
static void* fork_MS(void* args){

	// Extracting arguments from the passed struct
	DS_Tester_args* inArgs = (DS_Tester_args*)args;
	Locks* lock = inArgs->lock;
    Barriers* bar = inArgs->barrier;
    T_stack* t_stack = inArgs->t_stack;
    SGL_stack* s_stack = inArgs->s_stack;
    MS_queue* m_queue = inArgs->m_queue;
    SGL_Queue* s_queue = inArgs->s_queue;
    atomic<int>* epoch = inArgs->epoch;
    atomic<int>* reservations = inArgs->reservations;
	int tid = inArgs->tid;
	int number_of_threads = inArgs->number_of_threads;
    int iterations = inArgs->iterations;

    thread_local multimap<int,ms_node*> retired_nodes; 
    int value = tid;
    int garbage_genocide_countdown = 500;
    int time_shift_countdown = 100;

	bar->wait();
    
    // START CLOCK
    if(tid == 0) {
        clock_gettime(CLOCK_MONOTONIC,&start);
    }
    
    for(int i = 0; i < iterations; i ++) {
    
        m_queue->enqueue(value);
        value += 1;

    }

    while(true){

        int time = epoch[0].load(ACQ);
        reservations[tid].store(time,ACQREL);

        ms_node* to_retire = m_queue->dequeue();

        // Making sure value isn't the end signal
        if(to_retire->val != -1) {

            bool flag = false;
            for(auto itr = retired_nodes.begin(); itr != retired_nodes.end(); itr++) {
                
                if((ms_node*)itr->second == to_retire) {
                    flag = true;
                    break;
                    
                }
                
            }
            
            if(flag == false) {
             
                retired_nodes.insert({time, to_retire});
                
            }
            
            
            
        }

        garbage_genocide_countdown += -1;
        time_shift_countdown += -1;
        
        // performiong garbage collection when either garbage collection countdown hits zero or if at the end of all pops
        if(garbage_genocide_countdown == 0 || to_retire->val == -1) {

            if(retired_nodes.size() > 0) {
                
                extermination_ms(retired_nodes, reservations, number_of_threads);

            }
            garbage_genocide_countdown = 500;
            
            if(to_retire->val == -1) {
                break;
            }

        }
        
        // When time shift countdown is zzero, tid 0 will increment the global epoch
        if(tid == 0 && time_shift_countdown == 0) {
            
            epoch[0].store(time + 1, ACQREL);
            time_shift_countdown = 100;
            
        }


    }
    

    // Making sure all retired nodees are freed before exiting
    while(retired_nodes.begin() != retired_nodes.end()) {
        auto itr = retired_nodes.begin();
        delete(itr->second);
        retired_nodes.erase(itr);
        
    }
    

	bar->wait();

    // END CLOCK
    if(tid == 0) {
        clock_gettime(CLOCK_MONOTONIC,&finish);
    }
	

}

/***************************************************************************//**
 * @brief
 *  This it the fork for the parallel testing of the Single Global Lock Stack
 *
 * @details
 *  Pushes numbeers from a counter for the given number of iterations. Then immediatly pops for the given
 *  number of iterations.
 * @note
 * 	Each thread will execute this method individually. An end signal of -1 is used to know when all pops have occured. 
 *
 * @param[in] args
 *  
 *
 ******************************************************************************/ 
static void* fork_SGL_Stack(void* args){

	// Extracting arguments from the passed struct
	DS_Tester_args* inArgs = (DS_Tester_args*)args;
	Locks* lock = inArgs->lock;
    Barriers* bar = inArgs->barrier;
    T_stack* t_stack = inArgs->t_stack;
    SGL_stack* s_stack = inArgs->s_stack;
    MS_queue* m_queue = inArgs->m_queue;
    SGL_Queue* s_queue = inArgs->s_queue;
    atomic<int>* epoch = inArgs->epoch;
    atomic<int>* reservations = inArgs->reservations;
	int tid = inArgs->tid;
	int number_of_threads = inArgs->number_of_threads;
    int iterations = inArgs->iterations;

    thread_local multimap<int,ls_node*> retired_nodes; 
    int value = tid;
    int garbage_genocide_countdown = 500;
    int time_shift_countdown = 100;

	bar->wait();
    
    // START CLOCK
    if(tid == 0) {
        clock_gettime(CLOCK_MONOTONIC,&start);
    }
    
    for(int i = 0; i < iterations; i ++) {
    
        s_stack->push(value, lock, tid);
        value += 1;

    }

    while(s_stack->pop(lock, tid) != -1) {}
    

	bar->wait();

    // END CLOCK
    if(tid == 0) {
        clock_gettime(CLOCK_MONOTONIC,&finish);
    }


}

/***************************************************************************//**
 * @brief
 *  This it the fork for the parallel testing of the Single Global Lock Queue
 *
 * @details
 *  Enqueues numbers from a counter for the given number of iterations. Then immediatly dequeues for the given
 *  number of iterations.
 * @note
 * 	Each thread will execute this method individually. An end signal of -1 is used to know when all pops have occured. 
 *
 * @param[in] args
 *  
 *
 ******************************************************************************/ 
static void* fork_SGL_Queue(void* args){
	
	// Extracting arguments from the passed struct
	DS_Tester_args* inArgs = (DS_Tester_args*)args;
	Locks* lock = inArgs->lock;
    Barriers* bar = inArgs->barrier;
    T_stack* t_stack = inArgs->t_stack;
    SGL_stack* s_stack = inArgs->s_stack;
    MS_queue* m_queue = inArgs->m_queue;
    SGL_Queue* s_queue = inArgs->s_queue;
    atomic<int>* epoch = inArgs->epoch;
    atomic<int>* reservations = inArgs->reservations;
	int tid = inArgs->tid;
	int number_of_threads = inArgs->number_of_threads;
    int iterations = inArgs->iterations;

    thread_local multimap<int,ls_node*> retired_nodes; 
    int value = tid;
    int garbage_genocide_countdown = 500;
    int time_shift_countdown = 100;

	bar->wait();
    
    // START CLOCK
    if(tid == 0) {
        clock_gettime(CLOCK_MONOTONIC,&start);
    }
    
    for(int i = 0; i < iterations; i ++) {
    
        s_queue->enqueue(value, lock, tid);
        value += 1;

    }

    while(s_queue->dequeue(lock, tid) != -1) {}
    

	bar->wait();

    // END CLOCK
    if(tid == 0) {
        clock_gettime(CLOCK_MONOTONIC,&finish);
    }

}

/***************************************************************************//**
 * @brief
 *  This is the test method for the DS_Tester class
 *
 * @details
 *  tests and analyzes the passed data structure
 *
 * @note
 * 	none
 *
 * @param[in] data
 *  The data structure that will be tested
 * 
 * @param[in] size
 *  The data structure that will be tested
 * 
 * @param[in] number_of_threads
 *  The data structure that will be tested
 * 
 * @param[in] data_structure
 *  The data structure that will be tested
 *
 ******************************************************************************/
void DS_Tester::test(int iterations, int number_of_threads, string data_structure) {

    Locks lock(LOCK);
	Barriers barrier(BARRIER, number_of_threads);    
    T_stack t_stack(number_of_threads, iterations);
    SGL_stack s_stack(number_of_threads, iterations);
    MS_queue m_queue;
    SGL_Queue s_queue(number_of_threads);
    atomic<int> epoch;
    atomic<int>* reservations;

    epoch.store(0, ACQ);
    reservations = new atomic<int>[number_of_threads];
    
    

	// launch threads
	int ret; size_t i;
	for(i=1; i < number_of_threads; i++){

		DS_Tester_args* dstArgs = new DS_Tester_args;
		dstArgs->barrier = &barrier;
		dstArgs->lock = &lock;
        dstArgs->t_stack = &t_stack;
        dstArgs->s_stack = &s_stack;
        dstArgs->m_queue = &m_queue;
        dstArgs->s_queue = &s_queue;
        dstArgs->epoch = &epoch;
        dstArgs->reservations = reservations;
		dstArgs->number_of_threads = number_of_threads;
        dstArgs->iterations = iterations;
		dstArgs->tid = i;
		
        //////////// Treiber Stack ////////////
        if(data_structure.compare("treiber") == 0) {

            ret = pthread_create(&threads[i], NULL, &fork_Treiber, (void*)dstArgs);

        }

        //////////// M&S Queue ////////////
        else if(data_structure.compare("msQ") == 0){

            ret = pthread_create(&threads[i], NULL, &fork_MS, (void*)dstArgs);

        }
        //////////// SGL Stack ////////////
        else if(data_structure.compare("sglS") == 0){

            ret = pthread_create(&threads[i], NULL, &fork_SGL_Stack, (void*)dstArgs);

        }
        //////////// SGL Queue ////////////
        else if(data_structure.compare("sglQ") == 0){

            ret = pthread_create(&threads[i], NULL, &fork_SGL_Queue, (void*)dstArgs);

        }

        else {

            assert(false);

        }

		if(ret){

			printf("ERROR; pthread_create: %d\n", ret);
			exit(-1);

		}
        delete dstArgs;
	}

	DS_Tester_args* masterArgs = new DS_Tester_args;
	masterArgs->barrier = &barrier;
	masterArgs->lock = &lock;
    masterArgs->t_stack = &t_stack;
    masterArgs->s_stack = &s_stack;
    masterArgs->m_queue = &m_queue;
    masterArgs->s_queue = &s_queue;
    masterArgs->epoch = &epoch;
    masterArgs->reservations = reservations;
	masterArgs->number_of_threads = number_of_threads;
    masterArgs->iterations = iterations;
	masterArgs->tid = 0;

	// master also calls thread_main

    //////////// Treiber Stack ////////////
    if(data_structure.compare("treiber") == 0) {

        fork_Treiber((void*)masterArgs);

    }

    //////////// M&S Queue ////////////
    else if(data_structure.compare("msQ") == 0){

        fork_MS((void*)masterArgs);

    }
    //////////// SGL Stack ////////////
    else if(data_structure.compare("sglS") == 0){

        fork_SGL_Stack((void*)masterArgs);

    }
    //////////// SGL Queue ////////////
    else if(data_structure.compare("sglQ") == 0){

        fork_SGL_Queue((void*)masterArgs);

    }

    else {

        assert(false);

    }

	// join threads
	for(size_t i=1; i<number_of_threads; i++){

		ret = pthread_join(threads[i],NULL);

		if(ret){

			printf("ERROR; pthread_join: %d\n", ret);
			exit(-1);

		}
	}

    delete [] reservations;
    delete masterArgs;

    unsigned long long elapsed_ns;
	elapsed_ns = (finish.tv_sec-start.tv_sec)*1000000000 + (finish.tv_nsec-start.tv_nsec);
	printf("Elapsed (ns): %llu\n",elapsed_ns);

}	