// Copyright 2013,2019
// Posting or sharing this file is prohibited, including any changes/additions.
//
///////////////////////////////////////////////////////////////////////////////
// Main File:        intdate.c
// This File:        division.c
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

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

int count = 0;

void signal_handler(int signal) {
    if (signal == SIGINT) {   // Interrupt case
	printf("\nTotal number of operations successfully completed: %d\n", count);
        printf("The program will be terminated.\n");
    }

    else {  // Divide by 0 case
    	printf("Error: a division by 0 operation was attempted.\n");
   	printf("Total number of operations completed successfully: %d\n", count);
   	printf("The program will be terminated.\n");
    }
    exit(0);
}

int main() {
    int first;
    int second;
    int quotient;
    int remainder;
    char buffer[100];

    // Setting up the signal handler
    struct sigaction sa;

    if (memset(&sa, 0, sizeof(sa))== 0) {
        printf("Error with the memset.");
        exit(0);
    }

    // Handle the divide by 0
    sa.sa_handler = signal_handler;
    if (sigaction(SIGFPE, &sa, NULL) == -1 ) {
        printf("Error with the SIGFPE signal.\n");
        exit(0);
    }

    // Handle the interrupt
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        printf("Error with the SIGINT signal.\n");
        exit(0);
    }

    while (1) {
        // Get the first number  
        printf("Enter first integer:  ");
        fgets(buffer, 100, stdin);
        first = atoi(buffer);

	// Get the second number
        printf("Enter second integer:  ");
        fgets(buffer, 100, stdin);
        second = atoi(buffer);

	// Perform the division operation
	quotient = first / second;
        remainder = first % second;
        count++;
        printf("%d / %d is %d with a remainder of %d\n", 
            first, second, quotient, remainder);
    }


    return 0;
}
