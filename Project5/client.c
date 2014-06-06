/**
 * Robert McDermot, rom66
 * Project 5
 * 4/17/13
 * client.c
 * compile with: gcc -Wall -Wextra -O2 -m32 -pthread client.c -o client
 * execute with: ./client <ip address> <port> [ip address] [port]...
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

//although project description said block size should be 512, the server is giving blocks of 1024
#define BLOCK_SIZE 1024

/**
 * A simple struct to make it easy to store and access the ip and port of each SERVER
 */
typedef struct SERVER
{
  char * ip;
  int port;
} SERVER;

pthread_mutex_t threadNumMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t numFilesMutex = PTHREAD_MUTEX_INITIALIZER;
int NUMSERVERS;
int threadNum = 0;
int numFiles = 0;

/**
 * A method to save each chunk of data from the server.  Depending on the incoming file size, 
 * it was not always possible to save it in a memory buffer.  This method names the file
 * according to the chunk number that was requested so that they can be reassembled in order later.
 * 
 * @param	buffer		a pointer to the chunk data
 * @param	chunkNum	the number of the chunk requested from the server for naming the file
 * @param	bufferSize	the number of bytes in the chunk
 */
void makeTempFile(void * buffer, int chunkNum, int bufferSize)
{
	//work around for string concatination in C
	char fileName[50];
	sprintf(fileName, "%d.tmp", chunkNum);
	
	//check that we can write to the file
	FILE * outputFile = fopen(fileName,"wb");
	if(outputFile)
	{
		fwrite(buffer,1,bufferSize,outputFile);
		fclose(outputFile);
	}
	
	//increment the numFiles global variable so that we know how many to put back together
	pthread_mutex_lock(&numFilesMutex);
		numFiles++;
	pthread_mutex_unlock(&numFilesMutex);
}

/**
 * This method is responsible for repeatedly requesting chunks of a file from the server.
 * It sets up the socket and then writes a chunk number to the server.  It then listens
 * for the requested data and finally calls makeTempFile() to save the received data.
 * 
 * @param	serverList	a pointer to the list of SERVER structs generated from the command line
 * @return	1 if there were no unexpected issues talking with the server, 0 otherwise. This value
 * 			is unused, but required for the pthread_create function
 */
void * getFileInParallel(void * serverList)
{
	pthread_mutex_lock(&threadNumMutex);
		int myThreadNum = threadNum;
		threadNum++;
	pthread_mutex_unlock(&threadNumMutex);
	
	//attempt is used as an indexing variable along with the thread number
	int attempt = 1;
	
	while(1)
	{
		SERVER * servers = (SERVER *)serverList;
		struct sockaddr_in serv_addr;
		
		memset(&serv_addr, 0, sizeof(struct sockaddr));
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(servers[myThreadNum].port);
		serv_addr.sin_addr.s_addr = inet_addr(servers[myThreadNum].ip);
		
		int conFD = socket(PF_INET, SOCK_STREAM, 0);
		int ret = connect(conFD, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr));
		
		if (ret < 0)
			return (void*)0;
		
		//send chunk number as string
		int chunkNum = myThreadNum+attempt*NUMSERVERS-NUMSERVERS;
		char intBuf[50];
		sprintf( intBuf, "%d", chunkNum );
		ret = write(conFD, intBuf, strlen(intBuf));
		
		//don't continue if we couldn't write to the socket
		if(ret < 0)
			return (void*)0;
		
		void * buffer = calloc(BLOCK_SIZE,1);
		int bytesRead = read(conFD, buffer, BLOCK_SIZE);
		
		//don't continue if we couldn't read from the socket
		if(bytesRead < 0)
			return (void*)0;
		
		//if there was nothing to read, clean up and exit
		if(bytesRead == 0)
		{
			free(buffer);
			break;
		}
		
		//write the data we just got to a temp file then prepare to get the next chunk
		makeTempFile(buffer,chunkNum,bytesRead);
		free(buffer);
		attempt++;
		close(conFD);
	}
	
	return (void*)1;
}

/**
 * We set up the list of SERVER structs from the command line arguments and then 
 * call pthread_create to make a new thread for each one.  Next we call pthread_join on
 * each thread and block until they all complete.  Then we reassemble all of the .tmp files
 * into 1 file (called "receivedFile") and remove the .tmp files
 */
int main(int argc, char ** argv)
{
	if (argc<3 || !argc%2)
	{
		printf("Usage: %s <ipaddr1> <port1>\n",argv[0]);
		return -1;
    }
    
    NUMSERVERS = (argc-1)/2;
    pthread_t threads[NUMSERVERS];
    void * threadSuccess[NUMSERVERS];
    SERVER servers[NUMSERVERS];
    
    int i=1;
    for(i=1; i<argc; i+=2)
    {
		struct SERVER temp = {argv[i], atoi(argv[i+1])};
		servers[i/2] = temp;
	}
	for(i=0; i<NUMSERVERS; i++)
		pthread_create(&threads[i], NULL, getFileInParallel, &servers);
	for(i=0; i<NUMSERVERS; i++)
		pthread_join(threads[i], &threadSuccess[i]);
	
	//delete the output file if it already exists because we will be using append mode
	remove("receivedFile");
	
	FILE * outputFile = fopen("receivedFile","ab");
	if(outputFile == NULL)
	{
		printf("Can't open \"receivedFile\" for output.\n");
		return -1;
	}
	
	//get each .tmp file in order and append its contents to our output file
	for(i=0; i<numFiles; i++)
	{
		char fileName[50];
		sprintf(fileName, "%d.tmp", i);
		FILE * temp = fopen(fileName,"rb");
		void * buffer = calloc(BLOCK_SIZE,1);
		int bytesRead = fread(buffer,1,BLOCK_SIZE,temp);
		if(!bytesRead)
			continue;
		fwrite(buffer,bytesRead,1,outputFile);
		free(buffer);
		fclose(temp);
		
		//remove the .tmp file
		remove(fileName);
	}
	
	fclose(outputFile);
    
    return 0;
}
