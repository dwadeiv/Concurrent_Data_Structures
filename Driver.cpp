#include "Driver.hpp"

int main(int argc, char* argv[]){
	
	// parse args
	if(argc != 2 && argc !=6){
		
        cout << argc << endl;
		printf("Error\nUsage: concurrent_structures [--name] [-t NUM_THREADS] [-i NUM_ITERATIONS] [--structure=<treiber,msQ,sglS,sglQ>]\n\n");
		return 1;
	}

	// Variable declarations
	int c;
	size_t NUM_THREADS;
	string data_structure;
	int NUM_ITERATIONS;
	
    while(true) {

        // Creating the long options
        static struct option long_options[] = {
            {"name",   no_argument,         0,  'n' },
			{"structure",   required_argument,   0,  's' },
            {0, 0, 0, 0}
        };

        // Parsing through the command line options
        int option_index = 0;
        c = getopt_long(argc, argv, "nt:i:s:", long_options, &option_index);

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

            // Chosen data structure
            case 's': {

                data_structure = optarg;

                // If invalid data structure input is entered
                if(data_structure.compare("treiber") != 0 && data_structure.compare("msQ") != 0 && data_structure.compare("sglS") != 0 && data_structure.compare("sglQ") != 0) {
                    printf("\nInvalid data structure\nUsage: concurrent_structures [--name] [-t NUM_THREADS] [-i NUM_ITERATIONS] [--structure=<treiber,msQ,sglS,sglQ>]\n\n");
                    return 1;
                }
				break;

            }

            // Number of iterations
            case 'i': {

                NUM_ITERATIONS = atoi(optarg);
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

	DS_Tester dsTester(NUM_THREADS);

	dsTester.test(NUM_ITERATIONS, NUM_THREADS, data_structure);

	return 0;

}













