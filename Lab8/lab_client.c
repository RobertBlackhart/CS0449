#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>



int main(int argc, char ** argv) {
    int con_fd = 0;
    int ret = 0;
    struct sockaddr_in serv_addr;

    unsigned short port = 0;
    //char * file_name = NULL;


    if (argc != 3) {
	printf("Usage: ./client <port> <file-name>\n");
	return -1;
    }

    port = atoi(argv[1]);
    //file_name = argv[2];
    
    FILE* outputFile = fopen(argv[2],"wb");
	if(outputFile == NULL)
	{
		fprintf(stderr,"Can't open %s for output.\n",argv[2]);
		return -1;
	}

    con_fd = socket(PF_INET, SOCK_STREAM, 0);

    if (con_fd == -1) {
	perror("Socket Error\n");
	return -1;
    }

    
    memset(&serv_addr, 0, sizeof(struct sockaddr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");;
    
  
    ret = connect(con_fd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr));

    if (ret < 0) {
	perror("Connect error\n");
	return -1;
    }

	void * buffer = malloc(1024*sizeof(char));
	
	while(1)
	{
		int bytesRead = read(con_fd,buffer,sizeof(buffer));
		if(bytesRead == 0)
			break;
			
		if(bytesRead < 0)
		{
			fprintf(stderr,"Error while reading from socket. Exiting\n");
			exit(-1);
		}
		
		void * p = buffer;
		while(bytesRead > 0)
		{
			int bytesWritten = fwrite(buffer,sizeof(char),bytesRead,outputFile);
			if(bytesWritten < 0)
			{
				fprintf(stderr,"Error while writitng to the file. Exiting\n");
				exit(-1);
			}
			bytesRead -= bytesWritten;
			p += bytesWritten;
		}
	}
	
    //write(con_fd, "Hello\n", strlen("Hello\n"));

    
    close(con_fd);
    fclose(outputFile);


    return 0;
}
