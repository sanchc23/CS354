////////////////////////////////////////////////////////////////////////////////
// Main File:        heapAlloc.c
// This File:        heapAlloc.c
// Other Files:      
// Semester:         CS 354 Spring 2019
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
//////////////////////////// 80 columns wide //////////////////////////////////

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include "heapAlloc.h"

/*
 * This structure serves as the header for each allocated and free block.
 * It also serves as the footerVal for each free block but only containing size.
 */
typedef struct blockHeader {
        int size_status;
    /*
    * Size of the block is always a multiple of 8.
    * Size is stored in all block headers and free block footerVals.
    *
    * Status is stored only in headers using the two least significant bits.
    *   Bit0 => least significant bit, last bit
    *   Bit0 == 0 => free block
    *   Bit0 == 1 => allocated block
    *
    *   Bit1 => second last bit 
    *   Bit1 == 0 => previous block is free
    *   Bit1 == 1 => previous block is allocated
    * 
    * End Mark: 
    *  The end of the available memory is indicated using a size_status of 1.
    * 
    * Examples:
    * 
    * 1. Allocated block of size 24 bytes:
    *    Header:
    *      If the previous block is allocated, size_status should be 27
    *      If the previous block is free, size_status should be 25
    * 
    * 2. Free block of size 24 bytes:
    *    Header:
    *      If the previous block is allocated, size_status should be 26
    *      If the previous block is free, size_status should be 24
    *    footerVal:
    *      size_status should be 24
    */
} blockHeader;         

/* Global variable - DO NOT CHANGE. It should always point to the first block,
 * i.e., the block at the lowest address.
 */

blockHeader *heapStart = NULL;

blockHeader *nextFit = NULL;

/* 
 * Function for allocating 'size' bytes of heap memory.
 * Argument size: requested size for the payload
 * Returns address of allocated block on success.
 * Returns NULL on failure.
 * This function should:
 * - Check size - Return NULL if not positive or if larger than heap space.
 * - Determine block size rounding up to a multiple of 8 and possibly adding padding as a result.
 * - Use NEXT-FIT PLACEMENT POLICY to chose a free block
 * - Use SPLITTING to divide the chosen free block into two if it is too large.
 * - Update header(s) and footer as needed.
 * Tips: Be careful with pointer arithmetic and scale factors.
 */
void* allocHeap(int size) {
  int firstRun = 1;
    if(size < 0){ // In case the memory is full
    return NULL;
  }
  // Assigns nextFit to the start of the heap on the first iteration
  if(firstRun){

    //firstRun = 0;
    nextFit = heapStart;
  }
  
  int blockSize;  // Will hold the total size of the block (w/ padding and header)
  int realSize = 0;  
  blockHeader *currBlock = nextFit;
  blockHeader *setFooter = NULL;
  
  int overallSize = size + sizeof(blockHeader); // Size of header added to size
  
  //Padding necessary to make a multiple of 8
  int padding = 8 - (overallSize % 8); 
  if(padding != 8){
    blockSize = overallSize + padding;
  }
  
  // Otherwise, just keep size as is
  else{
    blockSize = overallSize;
  }
  
  // Variable to determine whether a full traversal has occured
  int loop = 1;
  while(loop ){
    
    
    if (currBlock == nextFit && firstRun != 1) {
      loop = 0;
    }
    
    firstRun = 0;
    // End of heap reached, loop to heapStart
    if(currBlock->size_status == 1){
      currBlock = heapStart;
    }
    //Detect allocation status of current block 
    if((currBlock->size_status & 1) == 0){

    // Determine if previous block is actually allocated, ensuring we have the right size
      if((currBlock->size_status & 2) == 2){
        realSize = (currBlock ->size_status) - 2;
      }
      else{
        realSize = (currBlock ->size_status); // recieve the number of blocks that is available
      }

      //  Size is exact - no changes necessary
     if(realSize == blockSize){
       currBlock->size_status += 1; 

       // Need to check next block's header for the p bit
       setFooter = (void*)currBlock + blockSize;
       setFooter->size_status = setFooter->size_status + 2; // Set p bit to 1

      // Move the pointer to a memory location past the header
      nextFit = currBlock;
       void* alloc  = (void*)currBlock + sizeof(blockHeader) ;
       return alloc;
     }
      
     // Splitting required
     else if(realSize > blockSize){

       int extra = realSize - blockSize; // Amount of extra space

       currBlock -> size_status = blockSize + 1;

       setFooter = (void*)currBlock + blockSize;

       // Update the header and footer of the new block
       setFooter -> size_status = extra + 2;
       setFooter = (void*) setFooter + extra - sizeof(blockHeader);
       setFooter -> size_status = extra;

       nextFit = currBlock;
       void* alloc = (void*)currBlock + sizeof(blockHeader);
       return alloc;
     }

      // Space is too small, go to next block
     else if(realSize < blockSize){ 
        currBlock = (void*)currBlock + realSize;
     }
    }

    // Current block is allocated - need to move past
    else if((currBlock->size_status & 1) == 1){
      // Previous block is ALSO allocated, save some time and move further
      if((currBlock -> size_status & 2) == 2){
          currBlock = (void*)currBlock + (currBlock->size_status - 2) - 1; 
      }
      // Previous block is free, check that one
      else{
        currBlock = (void*)currBlock + (currBlock -> size_status)-1;  // -1 since block is allocated to space
      }
    }

 }

// block can not be found so return NULL
return NULL;
}

/* 
 * Function for freeing up a previously allocated block.
 * Argument ptr: address of the block to be freed up.
 * Returns 0 on success.
 * Returns -1 on failure.
 * This function should:
 * - Return -1 if ptr is NULL.
 * - Return -1 if ptr is not a multiple of 8.
 * - Return -1 if ptr is outside of the heap space.
 * - Return -1 if ptr block is already freed.
 * - USE IMMEDIATE COALESCING if one or both of the adjacent neighbors are free.
 * - Update header(s) and footerVal as needed.
 */                    
int freeHeap(void *ptr) {         

  // Error condition for null pointer
  if(ptr == NULL){
    return -1;
  }
  // Condition for checking if the correct block is found
  int findBlock = 1;

  // Pointers to help with the parsing
  blockHeader *endfooterVal = NULL;
  blockHeader *currBlock = heapStart;
  int realSize = 0; // Variable to hold p-bit and a-bit values


  while(findBlock){

    // End of memory has been reached
    if(currBlock->size_status == 1){
      findBlock = 0;
      endfooterVal = currBlock;
    }

    // Free block
    else if((currBlock->size_status & 1) == 0){
      // Subtract the footer
      if((currBlock->size_status & 2) == 2){
        realSize = (currBlock->size_status) -2;
      }
      else{
        realSize = currBlock->size_status;
      }
      // Moves the pointer to the next block
      currBlock = (void*)currBlock + realSize; 

    }

    // Allocated block
    else if((currBlock->size_status & 1) == 1){
      if((currBlock->size_status & 2) == 2){
        currBlock = (void*)currBlock + (currBlock->size_status - 2) - 1;
      }
      else{
        currBlock = (void*)currBlock + (currBlock->size_status) - 1;
      }
    }
  }
  if(ptr < (void*)heapStart || ptr > (void*)endfooterVal){
    printf("out of memory \n");
    return -1;
  }

  // Need a new pointer block, as it accomodates all types
  blockHeader *tempBlock = ptr - sizeof(blockHeader); // Move to the header

  int sizeTempBlock;
  int totalSize;

  blockHeader *footerValLoc; // New footer location after coalescing
  blockHeader *newHeader; // New header location after coalescing

  // Error condition where block is already freed
  if((tempBlock->size_status & 1) == 0){
    printf("Already free\n");
    return -1;
  }
  // Error condition where the block is indivisible by 8
  sizeTempBlock = (tempBlock->size_status - 1) - (tempBlock->size_status & 2);
  if(sizeTempBlock % 8 != 0){
    printf("Not divisible by 8\n");
    return -1;
  }



  totalSize = sizeTempBlock;

  blockHeader *freeBlock = tempBlock;

  tempBlock = (void*)tempBlock + sizeTempBlock;

  // Detect if the pointer is free
  if((tempBlock -> size_status & 1) == 0){
    printf("Pointer is free\n");
    // Remove p-bit (we know previous block is allocated)
    totalSize = totalSize + (tempBlock->size_status) - 2;

    // Pointer to the footer
    footerValLoc = (void*)tempBlock + (tempBlock->size_status - 2) - sizeof(blockHeader);
  }

  else{
    printf("Pointer Free\n");
    footerValLoc = (void*)freeBlock + sizeTempBlock - sizeof(blockHeader);
  }
  // Detect if previous block is free
  if((freeBlock-> size_status & 2) == 0){
    printf("Previous block free\n");
    blockHeader *footerVal = (void*)freeBlock - sizeof(blockHeader);
    totalSize = totalSize + footerVal->size_status;

    newHeader = (void*)footerVal - (footerVal->size_status) + sizeof(blockHeader);

  }
  else{
    printf("New Header loc\n");
    newHeader = freeBlock;
  }
  newHeader->size_status = totalSize + 2;

  footerValLoc->size_status = totalSize;

  blockHeader *nextHeader;
  nextHeader = (void*)footerValLoc + sizeof(blockHeader);
  if((nextHeader->size_status &2) == 2){
    printf("nextHeader\n");
      nextHeader->size_status = nextHeader->size_status - 2;
  }
  return 0;
}

/*
 * Function used to initialize the memory allocator.
 * Intended to be called ONLY once by a program.
 * Argument sizeOfRegion: the size of the heap space to be allocated.
 * Returns 0 on success.
 * Returns -1 on failure.
 */                    
int initHeap(int sizeOfRegion) {         

    static int allocated_once = 0; //prevent multiple initHeap calls

    int pagesize;  // page size
    int padsize;   // size of padding when heap size not a multiple of page size
    int allocsize; // size of requested allocation including padding
    void* mmap_ptr; // pointer to memory mapped area
    int fd;

    blockHeader* endMark;
  
    if (0 != allocated_once) {
        fprintf(stderr, 
        "Error:mem.c: InitHeap has allocated space during a previous call\n");
        return -1;
    }
    if (sizeOfRegion <= 0) {
        fprintf(stderr, "Error:mem.c: Requested block size is not positive\n");
        return -1;
    }

    // Get the pagesize
    pagesize = getpagesize();

    // Calculate padsize as the padding required to round up sizeOfRegion 
    // to a multiple of pagesize
    padsize = sizeOfRegion % pagesize;
    padsize = (pagesize - padsize) % pagesize;

    allocsize = sizeOfRegion + padsize;

    // Using mmap to allocate memory
    fd = open("/dev/zero", O_RDWR);
    if (-1 == fd) {
        fprintf(stderr, "Error:mem.c: Cannot open /dev/zero\n");
        return -1;
    }
    mmap_ptr = mmap(NULL, allocsize, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (MAP_FAILED == mmap_ptr) {
        fprintf(stderr, "Error:mem.c: mmap cannot allocate space\n");
        allocated_once = 0;
        return -1;
    }
  
    allocated_once = 1;

    // for double word alignment and end mark
    allocsize -= 8;

    // Initially there is only one big free block in the heap.
    // Skip first 4 bytes for double word alignment requirement.
    heapStart = (blockHeader*) mmap_ptr + 1;

    // Set the end mark
    endMark = (blockHeader*)((void*)heapStart + allocsize);
    endMark->size_status = 1;

    // Set size in header
    heapStart->size_status = allocsize;

    // Set p-bit as allocated in header
    // note a-bit left at 0 for free
    heapStart->size_status += 2;

    // Set the footer
    blockHeader *footerVal = (blockHeader*) ((char*)heapStart + allocsize - 4);
    footerVal->size_status = allocsize;
  
    return 0;
}         
                 
/* 
 * Function to be used for DEBUGGING to help you visualize your heap structure.
 * Prints out a list of all the blocks including this information:
 * No.      : serial number of the block 
 * Status   : free/used (allocated)
 * Prev     : status of previous block free/used (allocated)
 * t_Begin  : address of the first byte in the block (where the header starts) 
 * t_End    : address of the last byte in the block 
 * t_Size   : size of the block as stored in the block header
 */                     
void DumpMem() {  

    int counter;
    char status[5];
    char p_status[5];
    char *t_begin = NULL;
    char *t_end   = NULL;
    int t_size;

    blockHeader *current = heapStart;
    counter = 1;

    int used_size = 0;
    int free_size = 0;
    int is_used   = -1;

    fprintf(stdout, "************************************Block list***\
                    ********************************\n");
    fprintf(stdout, "No.\tStatus\tPrev\tt_Begin\t\tt_End\t\tt_Size\n");
    fprintf(stdout, "-------------------------------------------------\
                    --------------------------------\n");
  
    while (current->size_status != 1) {
        t_begin = (char*)current;
        t_size = current->size_status;
    
        if (t_size & 1) {
            // LSB = 1 => used block
            strcpy(status, "used");
            is_used = 1;
            t_size = t_size - 1;
        } else {
            strcpy(status, "Free");
            is_used = 0;
        }

        if (t_size & 2) {
            strcpy(p_status, "used");
            t_size = t_size - 2;
        } else {
            strcpy(p_status, "Free");
        }

        if (is_used) 
            used_size += t_size;
        else 
            free_size += t_size;

        t_end = t_begin + t_size - 1;
    
        fprintf(stdout, "%d\t%s\t%s\t0x%08lx\t0x%08lx\t%d\n", counter, status, 
        p_status, (unsigned long int)t_begin, (unsigned long int)t_end, t_size);
    
        current = (blockHeader*)((char*)current + t_size);
        counter = counter + 1;
    }

    fprintf(stdout, "---------------------------------------------------\
                    ------------------------------\n");
    fprintf(stdout, "***************************************************\
                    ******************************\n");
    fprintf(stdout, "Total used size = %d\n", used_size);
    fprintf(stdout, "Total free size = %d\n", free_size);
    fprintf(stdout, "Total size = %d\n", used_size + free_size);
    fprintf(stdout, "***************************************************\
                    ******************************\n");
    fflush(stdout);

    return;  
}  
