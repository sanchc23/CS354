////////////////////////////////////////////////////////////////////////////////
// Main File:        cache1D.c
// This File:        cache2Dclash.c
// Other Files:      cache2Drows.c, cache2Dcols.c, cache1D.c
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
// Online sources:   N/A
//////////////////////////// 80 columns wide ///////////////////////////////////

int arr2D[128][8];

int main(int agrc, const char* argv[]){
  for(int iteration = 0; iteration < 100; iteration++){
    for(int row = 0; row <128; row += 64){
      for(int col = 0; col < 8; col++){
        arr2D[row][col] = iteration + row + col;
      }
    }
  }
  return 0;
}
