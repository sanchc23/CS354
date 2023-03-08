// Copyright 2013,2019
// Posting or sharing this file is prohibited, including any changes/additions.
//
///////////////////////////////////////////////////////////////////////////////
// Main File:        intdate.c
// This File:        sendsig.c
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

#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    if (strcmp(argv[1], "-i") == 0) {  // Sigint condition
        kill(atoi(argv[2]), 2); 
    } else if (strcmp(argv[1], "-u") == 0) {  // SigUSR1 condition
        kill(atoi(argv[2]), 10);
    } else {  // Other cases
        printf("Wrong parameter.\n");
        kill(atoi(argv[2]), 9);
    }

    return 0;
}
