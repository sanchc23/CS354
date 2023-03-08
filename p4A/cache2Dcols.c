////////////////////////////////////////////////////////////////////////////////
// Main File:        cache1D.c
// This File:        cache2Dcols.c
// Other Files:      cache2Drows.c, cache1D.c, cache2Dclash.c
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

int arr2D[3000][500];

int main(int argc, const char* argv[]){
  for(int col = 0; col < 500 ; col++){
    for(int row = 0; row < 3000; row++){
      arr2D[row][col] = row + col;
    }
  }
  return 0;
}
