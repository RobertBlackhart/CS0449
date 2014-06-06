/*

  project-1.c

  MY NAME: Robert McDermot
  MY PITT EMAIL: rom66@pitt.edu
  
 */

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		fprintf(stderr,"Usage: %s <INPUT_FILE>\n",argv[0]);
		return EXIT_FAILURE;
	}
	
	FILE* inputFile = fopen(argv[1],"rb");
	if(inputFile == NULL)
	{
		fprintf(stderr,"Can't open %s for input.\n",argv[1]);
		return EXIT_FAILURE;
	}
	
	short int previous = -1, current = -1, start = 0, endValue = -1, overallCount = 0, peak = -1, nadir = -1;
	
	while(fread(&current,sizeof(current),1,inputFile) != 0)
	{
		overallCount++;
		
		if(previous == -1)
			previous = current;
			
		if(peak == -1 || current > peak)
			peak = current;
			
		if(nadir == -1 || current < nadir)
			nadir = current;
		
		if(previous < 0 && current >= 0 && start > 0)
		{
			if(current == 0)
				endValue = current;
			else
				endValue = previous;
		}
		
		if(previous < 0 && current >= 0 && start == 0)
		{
			start = overallCount;
			endValue = -1;
			peak = current;
			nadir = current;
		}
		
		if(endValue != -1 && peak != -1 && nadir != -1)
		{
			int waveCount = 0;
			int end = 0;
			if(current == 0)
			{
				waveCount = overallCount-start+1;
				end = overallCount;
			}
			else
			{
				waveCount = overallCount-start;
				end = overallCount - 1;
			}
				
			printf("%hi\t%hi\t%hi\t%hi\t%hi\n",end,endValue,waveCount,peak,nadir);
			start = overallCount;
			endValue = -1;
			peak = -1;
			nadir = -1;
		}
		
		previous = current;
	}
	
	return EXIT_SUCCESS;
}
