////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2013,2019
// Posting or sharing this file is prohibited, including any changes/additions.
//
///////////////////////////////////////////////////////////////////////////////
// Main File:        intdate.c
// This File:        intdate.c
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

#include <unistd.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>


int countUSR = 0;
int timer = 3;

// Alarm handler
void handle_alarm() {
	time_t currTime;
	time(&currTime);
	printf("PID: %d | Current Time: %s", getpid(), ctime(&currTime));
	alarm(timer);
}

// SIGUSR1 handler
void handle_user1() {
	countUSR++;
	printf("SIGUSR1 caught!\n");
}

// SIGINT handler
void handle_sigint() {
		printf("\nSIGINT received.\n");
		printf("SIGUSR1 was received %d times. Exiting now.\n", countUSR);
		exit(0);
}

// Function to decide which sig handler to send sig to
void handle_signal(int signal) {

	if (signal == SIGALRM) { // SIGALRM condition
		handle_alarm();
	}

	else if (signal == SIGUSR1) { // SIGUSR1 condition
		handle_user1();
	}

	else {  // SIGINT condition
		handle_sigint(); 
	}
}

int main() {
	printf("Date will be printed every 3 seconds.\n");
	printf("Enter ^C to end the program.\n");

	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = &handle_signal;
	
	// SIGALRM handler error check
	if (sigaction(SIGALRM, &sa, NULL) == -1) {
		printf("Error: Could not register SIGALRM handler. \n");
		exit(0);
	}
	
	// SIGINT handler error check
	if(sigaction(SIGINT, &sa, NULL) == -1) {
		printf("Error: Could not register SIGINT handler. \n");
		exit(0);
	}
	
	// SIGUSR1 handler error check
	if(sigaction(SIGUSR1, &sa, NULL) == -1) {
		printf("Error: Could not register SIGUSR1 handler. \n");
		exit(0);
	}

	// Start loop - once every 3 seconds
	alarm(timer);


	// Infinite while
	while (1) {
	}

	return 0;
}
