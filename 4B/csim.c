////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2013,2019
// Posting or sharing this file is prohibited, including any changes/additions.
//
///////////////////////////////////////////////////////////////////////////////
// Main File:        csim.c
// This File:        csim.c
// Other Files:      N/A
// Semester:         CS 354 Fall 2019
//
// Author:           Cristian Sanchez
// Email:            csanchez24@wisc.edu
// CS Login:         sanchez-herrera
//
/////////////////////////// OTHER SOURCES OF HELP /////////////////////////////
//                   fully acknowledge and credit all sources of help,
//                   other than Instructors and TAs.
//
// Persons:          N/A
//
// Online sources:   N/A
//           
//                
///////////////////////////////////////////////////////////////////////////////


/*
 * csim.c:
 * A cache simulator that can replay traces (from Valgrind) and output
 * statistics for the number of hits, misses, and evictions.
 * The replacement policy is LRU.
 *
 * Implementation and assumptions:
 *  1. Each load/store can cause at most 1 cache miss plus a possible eviction.
 *  2. Instruction loads (I) are ignored.
 *  3. Data modify (M) is treated as a load followed by a store to the same
 *  address. Hence, an M operation can result in two cache hits, or a miss and a
 *  hit plus a possible eviction.
 */

#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

/******************************************************************************/
/* DO NOT MODIFY THESE VARIABLES **********************************************/

//Globals set by command line args.
int b = 0; //number of block (b) bits
int s = 0; //number of set (s) bits
int E = 0; //number of lines per set

//Globals derived from command line args.
int B; //block size in bytes: B = 2^b
int S; //number of sets: S = 2^s

//Global counters to track cache statistics in access_data().
int hit_cnt = 0;
int miss_cnt = 0;
int evict_cnt = 0;

//Global to control trace output
int verbosity = 0; //print trace if set

//Global to keep track of the LRU Counter
int lruCount = 0;
/******************************************************************************/


//Type mem_addr_t: Use when dealing with addresses or address masks.
typedef unsigned long long int mem_addr_t;

//Type cache_line_t: Use when dealing with cache lines.
typedef struct cache_line {                    
    char valid;
    mem_addr_t tag;
	int lruCount;
    //Add a data member as needed by your implementation for LRU tracking.
} cache_line_t;

//Type cache_set_t: Use when dealing with cache sets
//Note: Each set is a pointer to a heap array of one or more cache lines.
typedef cache_line_t* cache_set_t;
//Type cache_t: Use when dealing with the cache.
//Note: A cache is a pointer to a heap array of one or more sets.
typedef cache_set_t* cache_t;

// Create the cache (i.e., pointer var) we're simulating.
cache_t cache;  

/* init_cache:
 * Allocates the data structure for a cache with S sets and E lines per set.
 * Initializes all valid bits and tags with 0s.
 */                    
void init_cache() { 
	
	// 2^s gives you the value of S
	S = pow(2, s);

	// Variable for our initialized cache line
	cache_line_t init;

	// Allocates memory for a cache with S sets
	cache = (cache_set_t*) malloc(sizeof(cache_set_t) * S); 
	
	// Error condition for failed allocation
	if (cache == NULL) {
		printf("Cannot allocate memory for cache\n");
		exit(1);
   }

	for (int set = 0; set < S; set++) {
		// For each set, allocates memory for E lines
		cache[set] = (cache_line_t*) malloc(sizeof(cache_line_t) * E);
			if (cache[set] == NULL) {
				printf("Cannot allocate memory for cache set\n");
				exit(1);
			}
		//Set the tag, valid bit, and lruCount variable to 0 for each line
		for (int line = 0; line < E; line++) {
			init.tag = 0;
			init.valid = 0;
			init.lruCount = 0;
			cache[set][line] = init;
		}
	}   
}  
  

/* free_cache:
 * Frees all heap allocated memory used by the cache.
 */                    
void free_cache() {   
	// Individually frees each set of the cache
	for(int set = 0; set < S; set++) {
		// Can receive an error freeing a pointer set to NULL
		if (cache[set] == NULL) {    
			printf("Cannot free a pointer set to NULL");
			exit(1);
		}
		free(cache[set]);
	}
	// Checks if cache is NULL
	if (cache == NULL) {
		printf("Cannot free a pointer set to NULL");
		exit(1);
	}
	free(cache);
}  


/* access_data:
 * Simulates data access at given "addr" memory address in the cache.
 *
 * If already in cache, increment hit_cnt
 * If not in cache, cache it (set tag), increment miss_cnt
 * If a line is evicted, increment evict_cnt
 */                    
void access_data(mem_addr_t addr) {
	// A mask for the set index
	mem_addr_t tempSet = pow(2, s) - 1;

	// Extract s bits by shifting b bits out and comparing to mask
	int set = (addr >> b) & tempSet; 

	// Extracts the associated tag
	int tag = addr >> (s + b);
		
	// Hit if both the valid bit is 1, and the tag matches
	for (int line = 0; line < E; line++) {
		if(cache[set][line].valid && cache[set][line].tag == tag) {
			hit_cnt++;
			// Update the LRU Counter
			cache[set][line].lruCount = lruCount++;
			return;
		}
	}

	// Miss if not in cache
	for (int line = 0; line < E; line++) {
		// If a line is invalid, go through the miss procedure
		if(!cache[set][line].valid) {
			// 'Cache it' - sets the tag
			cache[set][line].tag = tag;
			// Update to valid
			cache[set][line].valid = 1; 
			// Update the LRU Counter
			cache[set][line].lruCount = lruCount;
			miss_cnt++; 
			return;
		}
	}

	// Eviction
	evict_cnt++;
	miss_cnt++;
	
	int lru = 0; // Temporary variable for the LRU index
	for (int line = 0; line < E; line++) {
		// Compares LRU counters of each line to one using the LRU index
		if (cache[set][line].lruCount < cache[set][lru].lruCount) {
			// If less, update the LRU index
			lru = line;
		}
	}
	// Updates the tag
	cache[set][lru].tag = tag;
	// Updates the LRU counter
	cache[set][lru].lruCount = lruCount;
}      



/* replay_trace:
 * Replays the given trace file against the cache.
 *
 * Reads the input trace file line by line.
 * Extracts the type of each memory access : L/S/M
 * TRANSLATE each "L" as a load i.e. 1 memory access
 * TRANSLATE each "S" as a store i.e. 1 memory access
 * TRANSLATE each "M" as a load followed by a store i.e. 2 memory accesses 
 */                    
void replay_trace(char* trace_fn) {           
    char buf[1000];  
    mem_addr_t addr = 0;
    unsigned int len = 0;
    FILE* trace_fp = fopen(trace_fn, "r"); 

    if (!trace_fp) { 
        fprintf(stderr, "%s: %s\n", trace_fn, strerror(errno));
        exit(1);   
    }

    while (fgets(buf, 1000, trace_fp) != NULL) {
        if (buf[1] == 'S' || buf[1] == 'L' || buf[1] == 'M') {
            sscanf(buf+3, "%llx,%u", &addr, &len);
      
            if (verbosity)
                printf("%c %llx,%u ", buf[1], addr, len);

            // GIVEN: 1. addr has the address to be accessed
            //        2. buf[1] has type of acccess(S/L/M)
            // call access_data function here depending on type of access

			// Store - 1 access
			if (buf[1] == 'S') {
				access_data(addr);
			}

			// Load - 1 access
			else if (buf[1] == 'L') {
				access_data(addr);
			}

			// 2 accesses (Load and Store) if 'M'
			if (buf[1] == 'M') {
				access_data(addr);
				access_data(addr);
			}

            if (verbosity)
                printf("\n");
        }
    }

    fclose(trace_fp);
}     


/*
 * print_usage:
 * Print information on how to use csim to standard output.
 */                    
void print_usage(char* argv[]) {                 
    printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", argv[0]);
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <num>   Number of s bits for set index.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of b bits for block offsets.\n");
    printf("  -t <file>  Trace file.\n");
    printf("\nExamples:\n");
    printf("  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", argv[0]);
    printf("  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", argv[0]);
    exit(0);
}


/*
 * print_summary:
 * Prints a summary of the cache simulation statistics to a file.
 */                    
void print_summary(int hits, int misses, int evictions) {                
    printf("hits:%d misses:%d evictions:%d\n", hits, misses, evictions);
    FILE* output_fp = fopen(".csim_results", "w");
    assert(output_fp);
    fprintf(output_fp, "%d %d %d\n", hits, misses, evictions);
    fclose(output_fp);
}


/*
 * main:
 * Main parses command line args, makes the cache, replays the memory accesses
 * free the cache and print the summary statistics.  
 */                    
int main(int argc, char* argv[]) {                      
    char* trace_file = NULL;
    char c;
    
    // Parse the command line arguments: -h, -v, -s, -E, -b, -t 
    while ((c = getopt(argc, argv, "s:E:b:t:vh")) != -1) {
        switch (c) {
            case 'b':
                b = atoi(optarg);
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'h':
                print_usage(argv);
                exit(0);
            case 's':
                s = atoi(optarg);
                break;
            case 't':
                trace_file = optarg;
                break;
            case 'v':
                verbosity = 1;
                break;
            default:
                print_usage(argv);
                exit(1);
        }
    }

    //Make sure that all required command line args were specified.
    if (s == 0 || E == 0 || b == 0 || trace_file == NULL) {
        printf("%s: Missing required command line argument\n", argv[0]);
        print_usage(argv);
        exit(1);
    }

    //Initialize cache.
    init_cache();

    //Replay the memory access trace.
    replay_trace(trace_file);

    //Free memory allocated for cache.
    free_cache();

    //Print the statistics to a file.
    //DO NOT REMOVE: This function must be called for test_csim to work.
    print_summary(hit_cnt, miss_cnt, evict_cnt);
    return 0;   
}           
