///////////////////////////////////////////////////////////////////////////////
//
// Copyright 2019 Jim Skrentny
// Posting or sharing this file is prohibited, including any changes/additions.
//
///////////////////////////////////////////////////////////////////////////////
// Main File:        hill_climb.c
// This File:        hill_climb.c
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

#include <stdio.h>   
#include <stdlib.h>     
#include <string.h>  

char *COMMA = ","; 

/*
 * USE THIS:
 * Structure representing the Matrix
 * numRows: number of rows
 * numCols: number of columns
 * matrix: 2D heap allocated array of integers
 */
typedef struct {    
    int numRows;
    int numCols;
    int **matrix;
} Matrix;

/* TODO:
 * Get the dimensions of the matrix that are on
 * the first line of the input file.
 * 
 * infp: file pointer for input file
 * nRows: pointer to number of rows in the matrix   
 * nCols: pointer to number of columns in the matrix
 */
void get_dimensions(FILE *infp, int *nRows, int *nCols) {  // Reusing the majority of the get_dimensions function from n_in_a_row
   char *line = NULL;
   size_t len = 0;
   if(getline(&line, &len, infp) == -1) {     // Ensures the file may be read
      printf("Error in reading the file\n");
      exit(1);
   }

  char *token = NULL;

  token = strtok(line, COMMA);  // Sets first input on first line to be  # of rows
  *nRows = atoi(token);      
  
  token = strtok(NULL, COMMA); // Sets second input on the first line to be # of columns
  *nCols = atoi(token);    
}   	
            

/* TODO:
 * Continually find and print the largest neighbor
 * that is also larger than the current position
 * until nothing larger is found.       
 * 
 * outfp: file pointer for output file  
 * mstruct: pointer to the Matrix structure
 */
void hill_climb(FILE *outfp, Matrix *mstruct) {  
   
    int currI = 0; // Used to move after a larger number is found
    int currJ = 0

    int currNum = (*(*mstruct -> matrix); // Stores the current working number used for comparisons
    int lastNum = currNum; // Stores last value for comparison in the exit condition

    fprintf(outfp, "%i", currNum);

    int tempI = 0; // Store the indices of larger numbers found
    int tempJ = 0;
    int largestNum = 1;  // 'Boolean' that determines the exit condition
    
    while(largestNum != 0){ // Exit condition - when largest number is found
	largestNum = 0;
   	for(int i = (currI - 1); i <= (currI + 1); i++){ // Checks the neighbors of the current index
       		for(int j = (currJ - 1); j <= (currJ + 1); j++){

	      		// Ensures that the indices stay in the bounds of the matrix
	       		if((i >= 0) && (j >= 0) && (i < ( mstruct -> numRows)) && (j < (mstruct -> numCols))) {
		    		if(currNum < (*(*(mstruct -> matrix + i) + j))){ // If a neighbor is larger, store that value 
		    			currNum = *(*(mstruct -> matrix + i) + j);
		    			tempI = i;  // Stores the indices of that value
		    			tempJ = j;
						largestNum = 1;
		   		}
	      		}
       		}
    	}

    if((currI == tempI) && (currJ = tempJ)) { // If the number hasn't changed, exit
   		fprintf(outfp, "\n");
		largestNum = 0;
		break;
    }
    
   	fprintf(outfp, " %i", currNum); // Print new 'larger' number
   	//lastNum = currNum; 
   	currI = tempI;            // Update the indices for future iterations  
   	currJ = tempJ;          

    }
}    

/* TODO:
 * This program reads an m by n matrix from the input file
 * and outputs to a file the path determined from the
 * hill climb algorithm.    
 *
 * argc: CLA count
 * argv: CLA values
 */
int main(int argc, char *argv[]) {
 
   // Ensures that exactly 3 inputs exist
   if(argc != 3 ){    
      printf("Error: Invaild number of arguments!\n");
      exit(1);
   }

   // Ensure the file opened properly
   FILE  *fp = fopen(*(argv + 1), "r");
   if(fp == NULL){
      printf("Cannot open file for reading\n");
   }

   // Local variable declaration
   int rows; // # of rows in matrix
   int cols; // # of columns in matrix

   // Allocate memory for Matrix, and ensure it worked
   Matrix *matr = malloc(sizeof(Matrix)); 
   if (matr == NULL) {
	printf("Cannot allocate memory for board\n");
	exit(1);
   }

   get_dimensions(fp, &rows, &cols); // Retrieve the dimensions of the matrix

   matr -> numRows = rows; // Assign rows into the Matrix structure
   matr -> numCols = cols; // Assign colums into the Matrix structure

   int **tempArray; // Placeholder array for the Matrix structure - changed the asterisk

   // Allocate 2D array for the matrix with the dimensions specified
   tempArray = malloc(sizeof(*int) * rows);
   if(tempArray == NULL){  // Ensure the matrix itself was properly constructed                     
      printf("Cannot allocate memory for board\n ");
      exit(1);
   }

   for(int i = 0; i < rows; i++){
   	(*(tempArray + i)) =  malloc(sizeof(int) * cols); 
   if(*(tempArray + i) == NULL){ // Ensure the matrix was properly constructed
	   printf("Cannot allocate memory for board\n");
	   exit(1);
	}
   }  

  // Reads each line of the input, and constructs the matrix - same as n_in_a_row
  char *line = NULL;
  size_t len = 0;
  char *token = NULL;
  for(int i = 0; i < matr -> numRows; i++){
  
     if(getline(&line, &len, fp) == -1){
     	printf("Error reading the file\n");
	exit(1);
     }
     
     token = strtok(line, COMMA); // Tokenize with commas to construct the matrix
     if(token ==  NULL){
     	printf("Could not properly create board\n");
	exit(1);
     }
    
     for(int j = 0; j < matr -> numCols; j++){
     	*(*(tempArray + i)+j) = atoi(token);  
	token = strtok(NULL, COMMA);
     }
  } 
  
 matr -> matrix = tempArray;  // Assign the established 2D array as the matrix

  FILE *fp_out = fopen(*(argv + 2), "w");  // Opens the file, sets it to write
  if(fp_out == NULL){
     printf("Cannot open file for printing\n");
     exit(1);
  }
  
  hill_climb(fp_out, matr); 


  // Frees all dynamically allocated memory
  for(int i = 0; i < matr -> numRows; i++){
     free(matr -> *(matrix + i); // Free up individual indices          
  }	  
  free(matr);
  matr = NULL;
  
  // Close the input file  
  if(fclose(fp) != 0){
     printf("Error while closing input file\n");
     exit(1);
  }

  // Close the output file
  if(fclose(fp_out) != 0){
     printf("Error while closing output file\n");
     exit(1);
  }

 return 0; 
}   
