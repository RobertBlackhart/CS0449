/*
 * Robert McDermot
 * 2013-4-5
 * mymalloc.h
 */

void * sbrk( int increment );

void * heapStart = 0;
void * heapEnd = 0;
int newMem = 0;

char * getLargestAvailableBlock(int size);
char * getPreviousFooter(char * blockstart);
char * getNextHeader(char * blockstart);
char * getNextFooter(char * blockstart);
void shrink();

/**
 * Returns to the caller the address of a suitable block of memory, allocating
 * more from the system if necessary.
 * 
 * @param	size	the size of the payload
 * @return			a pointer to a block of memory or NULL if no suitable block can be found
 */
void * my_worstfit_malloc(int size)
{
	if(size < 0)
		return NULL;
		
	if(size%2)
		size++;
		
	int effectiveSize = size+2*sizeof(int);
	
	if(!heapStart)
		heapStart = sbrk(0);
	if(!heapEnd)
		heapEnd = sbrk(0);
		
	char * blockstart = 0;
	if(heapEnd-heapStart == 0)
	{
		blockstart = (char *)sbrk(effectiveSize);
		heapEnd += effectiveSize;
		newMem = 1;
	}
	else
		blockstart = getLargestAvailableBlock(effectiveSize);
	
	int previousSize = *blockstart & 0xFFFFFFFE;
	if(newMem || previousSize == 0 || previousSize == effectiveSize) //newMem=1 means that sbrk(offset) was called and previousSize is a garbage value
	{
		newMem = 0;
		*blockstart = effectiveSize | 0x00000001;
		*(blockstart+effectiveSize-4) = effectiveSize | 0x00000001;
	}
	else if(previousSize > effectiveSize)
	{
		*blockstart = effectiveSize | 0x00000001;
		*(blockstart+effectiveSize-4) = effectiveSize | 0x00000001;
		*(blockstart+effectiveSize) = (previousSize-effectiveSize) & 0xFFFFFFFE;
		*(blockstart+previousSize-4) = (previousSize-effectiveSize) & 0xFFFFFFFE;
	}
	
	shrink();
	
	return blockstart+4;
}

/**
 * Frees the memory at the given location.  If there are other free blocks
 * surrounding this block, all of the blocks are combined to make one large
 * free block.  If after combining, the block is at the top of the heap, 
 * shrinks the heap by calling sbrk() with a negative offset.
 * 
 * @param	ptr		the address of the block of memory to be freed
 */
void my_free(void * ptr)
{
	char * blockstart = (char *)ptr-4;
	int blocksize = *blockstart & 0xFFFFFFFE;
	*blockstart &= 0xFFFFFFFE;
	*(blockstart+blocksize-4) &= 0xFFFFFFFE;
	
	char * start = blockstart;
	while(getNextHeader(start) != 0 && !(*getNextHeader(start) & 1))
	{
		*blockstart = *blockstart + (*getNextHeader(start) & 0xFFFFFFFE);
		if(!(int)getNextFooter(blockstart))
			break;
		*getNextFooter(start) = *blockstart + (*getNextHeader(start) & 0xFFFFFFFE);
		start = getNextFooter(start) + 4;
		
		if(*start == 0)
			break;
	}
		
	start = blockstart;
	while(getPreviousFooter(start) && !(*getPreviousFooter(start) & 1))
	{
		char * previousHeader = getPreviousFooter(start) - *getPreviousFooter(start)+4;
		*previousHeader = (*start + (*previousHeader & 0xFFFFFFFE)) & 0xFFFFFFFE;
		*(previousHeader+*previousHeader-4) = *previousHeader;
		start = previousHeader;
	}
	
	shrink();
}

/**
 * A utility method to get the footer of the block following the block
 * indicated by blockstart.
 * 
 * @param	blockstart	a pointer to the header of a memory block
 * @return				a pointer to the footer of the next memory block
 */
char * getPreviousFooter(char * blockstart)
{
	if((blockstart - 4) < (char *)heapStart)
		return (char *)0;
	else
		return blockstart-4;
}

/**
 * A utility method to get the header of the block following the block
 * indicated by blockstart.
 * 
 * @param	blockstart	a pointer to the header of a memory block
 * @return				a pointer to the header of the next memory block
 */
char * getNextHeader(char * blockstart)
{
	int blocksize = *blockstart & 0xFFFFFFFE;
	if((blockstart+blocksize) >= (char *)heapEnd)
		return (char *)0;
	else
		return blockstart+blocksize;
}

/**
 * A utility method to get the footer of the block previous to the block
 * indicated by blockstart.
 * 
 * @param	blockstart	a pointer to the header of a memory block
 * @return				a pointer to the footer of the previous memory block
 */
char * getNextFooter(char * blockstart)
{
	if(!getNextHeader(blockstart))
		return (char *)0;
		
	int thisBlockSize = *blockstart & 0xFFFFFFFE;
	int nextBlockSize = *getNextHeader(blockstart) & 0xFFFFFFFE;
	if(blockstart+thisBlockSize+nextBlockSize >= (char *)heapEnd)
		return (char *)0;
	else
		return blockstart+thisBlockSize+nextBlockSize-4;
}

/**
 * Checks each block in the heap in succession starting at the first and keeps track of the largest unused block.
 * If no blocks are large enough to hold at least _size_ or there are no blocks on the heap, sbrk(size) is called
 * and the return value from sbrk() is returned.  newMem is set to 1 to indicate that there was no
 * previous memory block used.
 * 
 * @param	size	the size of memory block needed to store the payload + header/footer
 * @return 			the address of the biggest available block or if none are suitable, the address returned by sbrk(size)
 */
char * getLargestAvailableBlock(int size)
{
	char * blockstart = heapStart;
	char * returnBlock = 0;
	
	while(blockstart < (char *)sbrk(0))
	{
		int used = *blockstart & 1;
		int blocksize = *blockstart & 0xFFFFFFFE;
				
		if(!used && blocksize >= size)
			returnBlock = blockstart;
			
		blockstart += blocksize;
	}
	
	if(returnBlock)
		return returnBlock;
	else
	{
		heapEnd += size;
		newMem = 1;
		return (char *)sbrk(size);
	}
}

/**
 * Shrinks the size of the heap if the last block on the heap is free.
 * Will also modify heapEnd to the new value.
 */
void shrink()
{
	char * lastFooter = (char *)sbrk(0) - 4;
	if(!(*lastFooter & 1))
	{
		int shrink = -(*lastFooter & 0xFFFFFFFE);
		sbrk(shrink);
		heapEnd += shrink;
	}
}
