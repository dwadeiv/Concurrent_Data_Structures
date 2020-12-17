concurrent_structures: Driver.o Locks.o Barriers.o Data_Structure_Tester.o MS_Queue.o SGL_Queue.o SGL_Stack.o Treiber_Stack.o
	g++  Driver.o Locks.o Barriers.o Data_Structure_Tester.o MS_Queue.o SGL_Queue.o SGL_Stack.o Treiber_Stack.o -pthread -O0 -g -oconcurrent_structures

Driver.o: Driver.cpp
	g++ -c -g Driver.cpp

Locks.o: Locks.cpp
	g++ -c -g Locks.cpp

Barriers.o: Barriers.cpp
	g++ -c -g Barriers.cpp

Data_Structure_Tester.o: Data_Structure_Tester.cpp
	g++ -c -g Data_Structure_Tester.cpp

MS_Queue.o: MS_Queue.cpp
	g++ -c -g MS_Queue.cpp

SGL_Queue.o: SGL_Queue.cpp
	g++ -c -g SGL_Queue.cpp

SGL_Stack.o: SGL_Stack.cpp
	g++ -c -g SGL_Stack.cpp

Treiber_Stack.o: Treiber_Stack.cpp
	g++ -c -g Treiber_Stack.cpp

clean:
	rm *.o concurrent_structures