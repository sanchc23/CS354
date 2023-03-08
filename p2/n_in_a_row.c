///////////////////////////////////////////////////////////////////////////////
//
// Copyright 2019 Jim Skrentny
// Posting or sharing this file is prohibited, including any changes/additions.
//
////////////////////////////////////////////////////////////////////////////////
// Main File:        n_in_a_row.c
// This File:        n_in_a_row.c
// Other Files:      N/A
// Semester:         CS 354 Fall 2019
//
// Author:           Cristian Sanchez
// Email:            csanchez24@wisc.edu
// CS Login:         sanchez-herrera
//
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
//                   fully acknowledge and credit all sources of help,
//                   other than Instructors and TAs.
//
// Persons:          N/A
//
//
// Online sources:   N/A
//                  
//                   
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>  
#include <stdlib.h>
#include <string.h>     

char *COMMA = ",";  

/* COMPLETED:       
 * Retrieves from the first line of the input file,
 * the size of the board (number of rows and columns).
 * 
 * fp: file pointer for input file
 * size: pointer to size
 */
void get_dimensions(FILE *fp, int *size) {      
    char *line = NULL;
    size_t len = 0;
    if (getline(&line, &len, fp) == -1) {
        printf("Error in reading the file\n");
        exit(1);
    }

    char *token = NULL;
    token = strtok(line, COMMA);
    *size = atoi(token);
}   


/*
 * Returns 1 if and only if the board is in a valid state.
 * Otherwise returns 0.
 * 
 * board: heap allocated 2D board
 * size: number of rows and columns
 */
int n_in_a_row(int **board, int size) {
    	
    int x_count = 0;  // Keeps track of number of X's
    int o_count = 0;  // Keeps track of  number of O's

    for(int i = 0; i < size; i++){     // checks the number of X's and O's on the board
   	for(int j = 0; j < size; j++){
	   if((*(*(board + i) + j)) == 1)
	      x_count += 1;
	   else if((*(*(board + i) + j)) == 2)
	      o_count += 1;
	}       	
    }	    

    int difference  = x_count - o_count; // Obtain the difference between # of X's and O's
    if((difference  > 1) || (difference < 0)){  // Ensure that difference is no more than 1, and that there are not more O's than X's
    	return 0; // Invalid game state if that is the case
    }

    int x_win = 0; // Variable to determine if X player has won
    int o_win = 0; // Variable to determine if O player has won
    int in_a_row = 0; // Variable to record consecutive X's and O's on the board

   
    for(int row = 0; row < size; row++){   // Checks for a row win on each column
        in_a_row  = 0; // Resets the count value to properly check each row

        for(int col = 0; col < size; col++){ // Check that consecutive items in a row are the same
           if(((*(*(board + row) + col)) == *(*(board + row)))){ 
              in_a_row  += 1;
           }
        }
        if((in_a_row == size) && *(*(board + row)) == 1){ // If there are 'size' X's in a row, it's X's win
            x_win += 1;
        }
        else if((in_a_row == size) && *(*(board + row)) == 2){ // If there are 'size' O's in a row, it's O's win
            o_win += 1;
        }
    }

    for(int col = 0; col < size; col++){    // Checks for a column win on each row
        in_a_row = 0;  // Resets the count value to properly check each column

        for(int row = 0; row < size; row++){
           if(((*(*(board + row) + col)) == *(*(board)+ col))){
              in_a_row += 1;
           }
        }
        if((in_a_row == size) && *(*(board)+ col) == 1){  // If there are 'size' X's in a column, it's X's win
            x_win += 1;
        }
        else if((in_a_row == size) && *(*(board) + col) == 2){  // If there are 'size' O's in a column, it's O's win
            o_win += 1;
        }
    }

    in_a_row  = 0; // Resets count variable outside of loop, as we are using one loop per diagonal 
   
    for(int i = 0; i < size; i++){  // Checks for a win on the left-to-right diagonal
    	if(((*(*(board + i) + i)) == *(*(board)))){ 
	   in_a_row += 1;
	}
    }

    if((in_a_row == size) && **board == 1){  // If there are 'size' X's on the left-to-right diagonal, it's X's win
       x_win += 1;
    }

    else if((in_a_row == size) && **board == 2){  // If there are 'size' O's on the left-to-right diagonal, it's O's win
       o_win += 1;
    }   


    in_a_row = 0;
    int n = 0; // Variable to aid in the travesal of right-to-left diagonal

    for(int i = size - 1; i >= 0; i--){   // Checks that consecutive items in the right-to-left diagonal are the same
      if(((*(*(board + i) + n)) == *(*(board)+ (size - 1)))){    // Algorithm used to traverse diagonal
  	in_a_row += 1;
      }
      n += 1;
    }
    if((in_a_row == size) && *(*(board + (size - 1))) == 1){  // If there are 'size' X's in the right-to-left diagonal, it's X's win
       x_win += 1;
    } 
    else if((in_a_row == size) && *(*(board +(size - 1))) == 2){  // If there are 'size' O's in the right-to-left diagonal, it's O's win
       o_win += 1;
    }
    

    if (x_win >= 1 && o_win >= 1){ // Verifies that at most 1 player won, invalid otherwise
      return 0;
    }
    return 1;   
}  

/* PARTIALLY COMPLETED:
 * This program prints Valid if the input file contains
 * a game board with either 1 or no winners; and where
 * there is at most 1 more X than O.
 * 
 * aurgc: CLA count
 * argv: CLA value
 */
int main(int argc, char *argv[]) {              

    if (argc != 2){  // Ensures that exactly 2 arguments are enterred
	printf("ERROR: Invaild number of arguments!\n");
    	exit(1);	
    }

    //Open the file and check if it opened successfully.
    FILE *fp = fopen(*(argv + 1), "r");
    if (fp == NULL) {
        printf("Cannot open file for reading\n");
        exit(1);
    }


    //Declare local variables.
    int size;

    get_dimensions(fp, &size);

   // int *board = malloc(sizeof(int) * size  * size);
    int ** board;  // 2D array name
    board = malloc(size * sizeof(int));
    if (board == NULL){    // Ensures that memory was allocated correctly
	printf("Cannot allocate memory for board\n");
	exit(1);	
    }	

    //Read the file line by line.
    //Tokenize each line wrt comma to store the values in your 2D array.
    char *line = NULL;
    size_t len = 0;
    char *token = NULL;
    for (int i = 0; i < size; i++) {

        if (getline(&line, &len, fp) == -1) {
            printf("Error while reading the file\n");
            exit(1);
        }

        token = strtok(line, COMMA);

	if(token == NULL){
	   printf("Could not correctly create board\n");
	   exit(1);
	}

        for (int j = 0; j < size; j++) {
            *(*(board + i) + j) = atoi(token);
            token = strtok(NULL, COMMA);
        }
    }

    int validity = n_in_a_row(board, size);  // Obtains whether the current state of the board is valid or invalid
    if(validity == 0) {
       printf("invalid\n");
    }
    else {
       printf("valid\n");
    }

    for(int i = 0; i < size; i++){  // Frees up the dynamicallly stored memory
	free(*(board + i));  // Have to free up each index of board
    } 
    free(board);
    board = NULL;

    //Close the file.
    if (fclose(fp) != 0) {
        printf("Error while closing the file\n");
        exit(1);
    } 

    return 0;       
}       

