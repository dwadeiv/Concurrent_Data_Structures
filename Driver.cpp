#include "Driver.hpp"
using std::string;
using std::ifstream;
using std::ofstream;

struct timespec start, finish;

int main(int argc, char* argv[]){
	
	// parse args
	if(argc != 2 && argc != 7){
		
		printf("Error\nUsage: mysort [--name] [sourcefile.txt] [-o outfile.txt] [-t NUM_THREADS] [--bar=<sense,pthread>] [--lock=<tas,ttas,ticket,pthread>]\n\n");
		return 1;
	}

	// Variable declarations
	int c;
	size_t NUM_THREADS;
	string barrierType;
	string lockType;
	string inputFile;
	string outputFile;

	ifstream inFile;
	ofstream outFile;

	// First option is the name of the input file.
	inputFile = argv[1];
	
    while(1) {

        // Creating the long options
        static struct option long_options[] = {
            {"name",   no_argument,         0,  'n' },
			{"bar",   required_argument,   0,  'b' },
			{"lock",   required_argument,   0,  'l' },
            {0, 0, 0, 0}
        };

        // Parsing through the command line options
        int option_index = 0;
        c = getopt_long(argc, argv, "nb:l:o:t:", long_options, &option_index);

        // When all the options have been read
        if (c == -1) {
            break;
        }

        switch(c) {

            // Name option. Program will terminate after printing name.
            case 'n': {

                printf("\nDavid Milton Wade IV\n");
                return 0;

            }

            // Chosen barrier type
            case 'b': {

                barrierType = optarg;

                // If invalid barrier type input is entered
                if(barrierType.compare("sense") != 0 && barrierType.compare("pthread") != 0) {
                    printf("\nInvalid lock type\nUsage: mysort [--name] [sourcefile.txt] [-o outfile.txt] [-t NUM_THREADS] [--bar=<sense,pthread>] [--lock=<tas,ttas,ticket,pthread>]\n\n");
                    return 1;
                }
				break;

            }

            // Chosen lock type
            case 'l': {

                lockType = optarg;

                // If invalid lock type input is entered
                if(lockType.compare("tas") != 0 && lockType.compare("ttas") != 0 && lockType.compare("ticket") != 0 && lockType.compare("pthread") != 0) {
                    printf("\nInvalid lock type\nUsage: mysort [--name] [sourcefile.txt] [-o outfile.txt] [-t NUM_THREADS] [--bar=<sense,pthread>] [--lock=<tas,ttas,ticket,pthread>]\n\n");
                    return 1;
                }
				break;

            }

            // Name of outputfile
            case 'o': {

                outputFile = optarg;
				break;

            }

			// Number of threads
            case 't': {

                // Chosen name for the output file
                NUM_THREADS = atoi(optarg);
				if(NUM_THREADS > 150){
					printf("ERROR; too many threads\n");
					exit(-1);
				}
				
				break;

            }
        }

    }
	
	inFile.open(inputFile);
	int capacity = 1000;
	int* interest_data;	
	interest_data = new int[capacity];
	int index = 0;
    string line;

	Array array;

    // Parsing through input file and inserting data into a vector

    if(inFile.is_open()) {
        while(getline(inFile, line)){

            int number = stoi(line);
			// Array needs to be resized
			if(index == capacity) {

				interest_data = array.resize(interest_data, capacity);

			}

            interest_data[index] = number;
			index++;

        }

        inFile.close();
    }

//////////// Bucket Sort //////////// 

	BucketSort bucketSort(NUM_THREADS);

	// START CLOCK
	clock_gettime(CLOCK_MONOTONIC,&start);

	bucketSort.bucketSort(interest_data, index, NUM_THREADS, lockType, barrierType);

	// END CLOCK
	clock_gettime(CLOCK_MONOTONIC,&finish);

	// Adding sorted data into output file
	outFile.open(outputFile);
	for(int i = 0; i < index; i++){

		outFile << interest_data[i] << std::endl;

	}

	outFile.close();
	delete interest_data;
	
	unsigned long long elapsed_ns;
	elapsed_ns = (finish.tv_sec-start.tv_sec)*1000000000 + (finish.tv_nsec-start.tv_nsec);
	printf("Elapsed (ns): %llu\n",elapsed_ns);


	return 0;

}










