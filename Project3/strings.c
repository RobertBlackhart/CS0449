#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void readStrings(FILE *inputFile);
int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		fprintf(stderr,"Usage: %s <INPUT_FILE>\n",argv[0]);
		return EXIT_FAILURE;
	}
	
	FILE* inputFile = fopen(argv[1],"rb");
	if(!inputFile)
	{
		fprintf(stderr,"Can't open %s for input.\n",argv[1]);
		return EXIT_FAILURE;
	}
	
	readStrings(inputFile);
	fclose(inputFile);
	exit(0);
}

void readStrings(FILE *inputFile)
{
	char *buffer = calloc(5,sizeof(char));
	if(!buffer)
	{
		fprintf(stderr,"Could not allocate enough memory for buffer, exiting\n");
		exit(-1);
	}
	char current = 0;
	int length = 0;
	int capacity = 4;
	
	while(fread(&current, sizeof(char), 1, inputFile) == 1)
	{
		int value = (int)current;
		if(value > 31 && value < 127)
		{
			if(length < capacity)
			{
				buffer[length] = current;
			}
			else
			{
				capacity *= 2;
				buffer = realloc(buffer,sizeof(char)*capacity+1);
				if(!buffer)
				{
					fprintf(stderr,"Could not allocate enough memory for buffer, exiting\n");
					exit(-1);
				}
				
				buffer[length] = current;
			}
			
			length++;
		}
		else
		{
			if(length > 3)
			{
				int i=0;
				for(;i<length; i++)
				{
					printf("%c",buffer[i]);
				}
				printf("\n");
			}
			length = 0;
			free(buffer);
			buffer = calloc(5,sizeof(char));
			capacity = 4;
		}
	}
	
	free(buffer);
}
