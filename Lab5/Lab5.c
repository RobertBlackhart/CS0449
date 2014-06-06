#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// ------------------------------------------------------
// HELPFUL DEBUGGING UTILITIES - NOT PART OF THE SOLUTION

void bprintInt( long n)
{
	int i;
	int j = 0;
	for (i = 31 ; i >=0  ; --i , j++)
 	{
		if(j==4)
		{
			printf("|");
			j=0;
		}
       if ( (n >> i) & 1)
          printf("1" );
       else
          printf("0" );
    }
}

void bprintByte( char n)
{
	int i;
	int j = 0;
	for (i = 7 ; i >=0  ; --i, j++ )
 	{
		if(j==4)
		{
			printf("|");
			j=0;
		}
       if ( (n >> i) & 1)
          printf("1" );
       else
          printf("0" );
    }
}
void bprintShort( short n)
{
	int i;
	int j = 0;
	for (i = 15 ; i >=0  ; --i, j++ )
 	{
		if(j==4)
		{
			printf("|");
			j=0;
		}
       if ( (n >> i) & 1)
          printf("1" );
       else
          printf("0" );
    }
}
// ------------------------------------------------------

int main( int argc, char *argv[] )
{
	FILE* txtFile, *binFile;
	int shortsReadIn=0,  _12bitValsWritten=0, _12bitValsReadBackIn=0;
	shortsReadIn = shortsReadIn;
	_12bitValsReadBackIn = _12bitValsReadBackIn;
	short s1,s2; /* the pair read each time */

	if (argc<3)
	{
		printf("usage: %s  <infilename> <outfilename> (i.e. ./a.out sine.txt sxine.bin)\n", argv[0] );
		return EXIT_FAILURE;
	}

	txtFile = fopen( argv[1], "r" );
	if (txtFile == NULL )
	{
		printf("Can't open %s for input. Program halting\n",argv[1] );
		return EXIT_FAILURE;
	}

	binFile = fopen( argv[2], "w" );
	if (binFile == NULL )
	{
		printf("Can't open %s for output. Program halting\n",argv[2] );
		return EXIT_FAILURE;
	}

	while(1)  // giving you the read loop so you dont use incorrect idiom of  while !EOF
	{
		// * * * *  Y O U R   C O D E   H E R E * * * *
		// every iteration of this loop should read in two shorts from the text file and write three bytes to the binary file
		
		if(fscanf(txtFile, "%hi", &s1 ) == 1)
		{			
			 if(fscanf(txtFile, "%hi", &s2) == 1)
			 {
				 //at least 2 shorts were left
				 s1 = s1 << 4;
				 short temp = s2 >> 8;
				 temp = temp & 0x000F;
				 s1 = s1 | temp;
				 fwrite(&s1,sizeof(short),1,binFile);
				 char c = s2;
				 fwrite(&c,sizeof(char),1,binFile);
				 _12bitValsWritten += 2;
			 }
			 else
			 {
				 //only 1 short was left - it's in s1
				 s1 = s1 << 4;
				 fwrite(&s1,sizeof(short),1,binFile); //check if this needs shifted or not when comparing to the answer
				 _12bitValsWritten++;
			 }
		}
		else
		{
			//there were no shorts left
			break;
		}
	}

	//printf("%d integers written to %s (3 bytes per 2 integers written)\nNow READING/ECHOING values from binary outfile\n",_12bitValsWritten, argv[2] );	
	fclose( txtFile );
	fclose( binFile );

/* READ BACK AND ECHO THE VALUES WRITTEN OUT */

	binFile = fopen( argv[2], "rb" );
	if (binFile == NULL )
	{
		printf("Can't open %s for input. Program halting\n",argv[2] );
		return EXIT_FAILURE;
	}

	while (1) // giving you the correct form of the read so you don't use  bad-> while !EOF
	{
		// * * * *  Y O U R   C O D E   H E R E * * * *
		// every iteration of this loop should read in ONE shorts and ONE byte from the binary file
		// from that short and byte reconstitute and write to stdout the TWO oringinal shorts to
		// that were compressed into them.
		
		char c = 0;
		
		if(fread(&s1, sizeof(s1), 1, binFile) == 1)
		{
			if(fread(&c, sizeof(c), 1, binFile) == 1)
			{
				s2 = s1 & 0x000F; //get the least significant 4 bits
				s2 = s2 << 12; //shift them over by 12 and back by 4 to keep signedness (8 0's at end)
				s2 = s2 >> 4;
				short temp = c & 0x00FF;
				s2 = s2 | temp; //bitwise or the least significant bits with c
				s1 = s1 >> 4; //move s1 bits back to right by 4
				printf("%hi\n%hi\n",s1,s2);
			}
			else
			{
				s1 = s1 >> 4;
				s1 = s1 & 0x8FFF;
				printf("%hi\n",s1);
			}
		}
		else
		{
			break;
		}
	}
	fclose( binFile );

	return EXIT_SUCCESS;
}

