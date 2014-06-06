void * sbrk( int increment );

void * heapStart = 0;
void * heapEnd = 0;
int newMem = 0;

void shrink()
{
	int * lastFooter = (int *)sbrk(0) - 1;
	if(!(*lastFooter & 1))
	{
		int shrink = -(*lastFooter & 0xFFFFFFFE);
		sbrk(shrink);
		heapEnd += shrink;
	}
}

void * getLargestAvailableBlock(int size)
{
	char * blockstart = heapStart;
	int largest = 0;
	char * returnBlock;
	
	do
	{
		int blockheader = *((int *)blockstart);
		int used = blockheader & 1;
		int blocksize = blockheader & 0xFFFFFFFE;
				
		if(blocksize == 0)
			break;
			
		if(!used && blocksize >= size)
		{
			largest = blocksize;
			returnBlock = blockstart;
		}
			
		blockstart += blocksize;
	}
	while((void *)blockstart < sbrk(0));
	
	if(largest != 0)
		return returnBlock;
	
	heapEnd += size;
	newMem = 1;
	return sbrk(size);
}

void * my_worstfit_malloc(int size)
{
	int effectiveSize = size+2*sizeof(int);
	
	if(!heapStart)
		heapStart = sbrk(0);
	if(!heapEnd)
		heapEnd = sbrk(0);
		
	int * blockstart = 0;
	if(heapEnd-heapStart == 0)
	{
		blockstart = (int *)sbrk(effectiveSize);
		heapEnd += effectiveSize;
		newMem = 1;
	}
	else
		blockstart = (int *)getLargestAvailableBlock(effectiveSize);
			
	int previousSize = *blockstart & 0xFFFFFFFE;
	if(previousSize == 0 || previousSize == effectiveSize || newMem)
	{
		newMem = 0;
		*blockstart = effectiveSize | 0x00000001;
		*((char *)blockstart+effectiveSize-4) = effectiveSize | 0x00000001;
	}
	else if(previousSize > effectiveSize)
	{
		*blockstart = effectiveSize | 0x00000001;
		*((char *)blockstart+effectiveSize-4) = effectiveSize | 0x00000001;
		*((char *)blockstart+effectiveSize) = (previousSize-effectiveSize) & 0xFFFFFFFE;
		*((char *)blockstart+previousSize-4) = (previousSize-effectiveSize) & 0xFFFFFFFE;
	}
	
	shrink();
	
	return blockstart+1;
}

int * getPreviousFooter(int * blockstart)
{
	if((blockstart - 1) < (int *)heapStart)
		return (int *)0;
	else
		return (int *)(blockstart-1);
}

int * getNextHeader(int * blockstart)
{
	int blocksize = *blockstart & 0xFFFFFFFE;
	if((blockstart+blocksize/4) > (int *)heapEnd)
		return (int *)0;
	else
		return (int *)(blockstart+blocksize/4);
}

int * getNextFooter(int * blockstart)
{
	int thisblocksize = *blockstart & 0xFFFFFFFE;
	int nextblocksize = *getNextHeader(blockstart) & 0xFFFFFFFE;
	if(blockstart+thisblocksize/4+nextblocksize/4 > (int *)heapEnd)
		return (int *)0;
	else
		return (int *)(blockstart+thisblocksize/4+nextblocksize/4-1);
}

void my_free(void * ptr)
{
	int * blockstart = ((int *)ptr)-1;
	int blocksize = *blockstart & 0xFFFFFFFE;
	*blockstart &= 0xFFFFFFFE;
	*(blockstart+blocksize/4-1) &= 0xFFFFFFFE;
	
	int * start = blockstart;
	while(getNextHeader(start) != 0 && !(*getNextHeader(start) & 1))
	{
		*blockstart = *blockstart + (*getNextHeader(start) & 0xFFFFFFFE);
		*getNextFooter(start) = *blockstart + (*getNextHeader(start) & 0xFFFFFFFE);
		start = (int *)((char *)getNextFooter(start) + 4);
		
		if(*start == 0)
			break;
	}
	
	start = blockstart;
	while(getPreviousFooter(start) && !(*getPreviousFooter(start) & 1))
	{
		int * previousHeader = getPreviousFooter(start) - *getPreviousFooter(start)/4+1;
		*previousHeader = (*start + (*previousHeader & 0xFFFFFFFE)) & 0xFFFFFFFE;
		*((char *)previousHeader+*previousHeader-4) = *previousHeader;
		start = previousHeader;
	}
	
	shrink();
}
