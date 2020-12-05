mysort: Driver.o Merge_Sort.o
	g++  Driver.o Merge_Sort.o -O0 -g -fopenmp -o mysort

Driver.o: Driver.cpp
	g++ -c -g -fopenmp Driver.cpp

Merge_Sort.o: Merge_Sort.cpp
	g++ -c -g -fopenmp Merge_Sort.cpp

clean:
	rm *.o mysort