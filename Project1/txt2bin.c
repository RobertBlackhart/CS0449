/*

  txt2bin.c

  MY NAME: Robert McDermot
  MY PITT EMAIL: rom66@pitt.edu
  
 */

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	if(argc < 3)
	{
		fprintf(stderr,"Usage: %s <INPUT_FILE> <OUTPUT_FILE>\n",argv[0]);
		return EXIT_FAILURE;
	}
	
	FILE* inputFile = fopen(argv[1], "rt" );
	if(inputFile == NULL)
	{
		fprintf(stderr,"Can't open %s for input.\n",argv[1]);
		return EXIT_FAILURE;
	}
	FILE* outputFile = fopen(argv[2], "wb" );
	if(outputFile == NULL)
	{
		fprintf(stderr,"Can't open %s for output.\n",argv[2]);
		return EXIT_FAILURE;
	}
	
	short int x;
	
	while(fscanf(inputFile,"%hi",&x) != EOF)
	{
		fwrite(&x,sizeof(x),1,outputFile);
	}
	
	fclose(inputFile);
	fclose(outputFile);
	
	inputFile = fopen(argv[2],"rb");
	if(inputFile == NULL)
	{
		fprintf(stderr,"Can't open %s for input.\n",argv[2]);
		return EXIT_FAILURE;
	}
	
	while(fread(&x,sizeof(x),1,inputFile) != 0)
	{
		printf("%hi\n",x);
	}
	
	fclose(inputFile);
	
	return EXIT_SUCCESS;
}
