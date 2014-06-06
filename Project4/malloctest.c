#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "mymalloc.h"

#define malloc(x) my_worstfit_malloc(x)
#define free(x) my_free(x)

void * sbrk( int increment );
void * my_worstfit_malloc (int size );
void my_free (void *ptr);
void heapWalk( char * caption);    // YOU FINISH THIS METHOD BELOW MAIN

void *base; // The original base of the HEAP as given to us by the compiler

int main ()
{
	// Save this so you know where the absolute start of the heap is every time
	// you want to traverse the heap looking for a block.

	base = sbrk(0); // DO NOT MODIFY. THIS IS YOUR ORIGINAL BASE Of THE HEAP

	// MALLOC 40 bytes then free it
	int * arr1 = malloc( 10 * sizeof(int) ); // Note payload is even number of bytes
	free( arr1 );
	
	heapWalk("State of the heap after 1 malloc and 1 free - should be empty (shrunk so that heapbase == heaptop)");
		
	// MALLOC 50 bytes do NOT free
	char * string1= malloc( 50  ); // Note payload is even number of bytes
	strcpy( string1, "Hey World! I just malloced 50 bytes!" );
	//printf("%s\n", string1 );
	
	heapWalk("State of the heap after 1 malloc of 50 bytes, not freed");
	
	// MALLOC 50 bytes then free it
	char * string3= malloc( 50  ); // Note payload is even number of bytes
	strcpy( string3, "Hey World! I just malloced 50 bytes!" );
	free(string1);
	
	heapWalk("State of the heap after 1 malloc of 50 bytes, then freed previous allocation");
		
	// MALLOC 20 bytes then free it
	int * arr2 = malloc( 5 * sizeof(int) ); // Note payload is even number of bytes
	
	heapWalk("State of the heap after 1 malloc of 20 bytes - should use some space from the 50 free bytes");
	
	free(arr2);
	
	heapWalk("State of the heap after freeing previous 20 bytes, should recombine the 50 byte block");
	
	int * string4 = malloc(61);
	heapWalk("State of the heap after 1 malloc of 61 bytes. Should be rounded to 62 bytes");
	malloc(26);
	heapWalk("State of the heap after 1 malloc of 26 bytes. Should use part of the first 50 byte block");
	free(string4);
	
	heapWalk("State of the heap after freed previous 62 byte allocation");
	
	malloc( 16 );

	heapWalk("State of the heap after 1 malloc of 16 bytes.  Should use all of the remaining bits of the 50 byte block");

	return 0;
}

void heapWalk( char * caption)
{
	printf("\n%s\n\n",caption);
	
	if (base == sbrk(0))  // base == top means no blocks (in use or freed) exist on the HEAP
	{
		printf("The size of the heap is 0. There no allocated blocks in the heap.\n");
		return;
	}
	
	// If you are here then there must have been some blocks allocated on the heap. Assume the first one start at the first 4 bytes of the heap

	int blockNum=0;  // counter for which block we are one  1,2,3,4,5 etc.
	char * blockstart = base;  // mem addr if first byte in heap
	int baseoffset = 0;     // since we are at the very base of HEAP, the bytes offest is 0

	printf("HEAPBASE: %d\n", (int)base );
	printf("   BLOCK#  STATUS  STARTADDR  BASE_OFFS  PAYLOAD    HDR/FTR\tBLOCKSIZE\n");
	do
	{
		++blockNum;
		int blockheader = *blockstart;
		int STATUS = blockheader & 1;
		int blocksize = blockheader & 0xFFFFFFFE;
		int payload = blocksize - 2*sizeof(int);
		
		if(blocksize == 0)
			break;
		
		printf("      %d",blockNum);
		if(STATUS)
		{
			printf("     USED");
		}
		else
		{
			printf("     FREE");
		}
		printf("   %d      %d\t %d bytes   %d bytes\t   %d\n",(int)blockstart,baseoffset,payload,2*sizeof(int),blocksize);
			
		blockstart += blocksize;
		baseoffset += blocksize;

	} while ( (void *)blockstart < sbrk(0) );  // While we have not reached TOP of the HEAP

	printf("HEAP TOP: %d\n", (int) sbrk(0) );
}
