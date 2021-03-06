#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <poll.h>
#include "RequestHandler.h"

#define PORT 8080 
#define MAX_BUFFER_SIZE 1024

int main(int argc, char const *argv[]) 
{ 
	int sock = 0; 
	struct sockaddr_in serv_addr; 
	FILE *received_file;
	char receivedFrame[MAX_BUFFER_SIZE] = {0};
	int file_size;
	int remain_data = 0, offset = 0;
	char filename[MAX_BUFFER_SIZE];
	struct pollfd fds[1];

	if (argc != 2) {
		printf("\nInvalid number of arguments\n");
		return -1;
	}

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 

	// Convert IPv4 and IPv6 addresses from text to binary form 
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1; 
	} 


	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{ 
		printf("\nConnection Failed \n"); 
		return -1; 
	} 
	sprintf(filename, "requestedFile%d", getpid());
	sendFileSizeRequest(sock, argv[1]);
	file_size = getFileSize(sock);
	fprintf(stdout, "\nFile size : %d\n", file_size);

	received_file = fopen(filename, "w");
	if (received_file == NULL)
	{
		fprintf(stderr, "Failed to open file foo --> %s\n", strerror(errno));

		exit(EXIT_FAILURE);
	}

	fds[0].fd = sock;
	fds[0].events = POLLIN | POLLPRI;

	remain_data = file_size;
	offset = 0;

	sendFrameRequest(sock, argv[1], offset);
	while (remain_data > 0)
	{	
		int expectedSize;
		int dataOK;
		int rv;
		int length;
		if (file_size - offset >= MAX_BUFFER_SIZE)
			expectedSize = MAX_BUFFER_SIZE;
		else
			expectedSize = file_size - offset;

		if ((rv = poll(fds, 1, 10000) > 0)) {
			if( fds[0].revents & POLLIN) {
				dataOK = getFrame(sock, receivedFrame, expectedSize, &length);
				if (dataOK) {
					receivedFrame[length] = '\0';
					fwrite(receivedFrame, sizeof(char), strlen(receivedFrame), received_file);
					remain_data -= expectedSize;
					offset += expectedSize;
				}
			}
		}

		sendFrameRequest(sock, argv[1], offset);
	}

	fclose(received_file);
	close(sock);

	return 0; 
} 
