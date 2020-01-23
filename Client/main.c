#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>

#define PORT 8080 
#define MAX_BUFFER_SIZE 1024

int main(int argc, char const *argv[]) 
{ 
	int sock = 0, valread; 
	struct sockaddr_in serv_addr; 
	char buffer[1024] = {0};
	FILE *received_file;
	ssize_t len;
	char received_length[MAX_BUFFER_SIZE];
	int file_size;
	int remain_data = 0;
	char aux[256];

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
	send(sock, argv[1], strlen(argv[1]), 0);
	recv(sock, buffer, MAX_BUFFER_SIZE, 0);
	printf("\nServer message: %s\n", buffer);
	file_size = atoi(buffer);
	fprintf(stdout, "\nFile size : %d\n", file_size);

	received_file = fopen("requestedFile", "w");
	if (received_file == NULL)
	{
		fprintf(stderr, "Failed to open file foo --> %s\n", strerror(errno));

		exit(EXIT_FAILURE);
	}

	remain_data = file_size;

	while ((remain_data > 0) && ((len = recv(sock, buffer, MAX_BUFFER_SIZE, 0)) > 0))
	{
		recv(sock, received_length, MAX_BUFFER_SIZE, 0);
		sprintf(aux, "%d", len);
		if (strcmp(aux, received_length) == 0) {
			fwrite(buffer, sizeof(char), len, received_file);
			remain_data -= len;
			fprintf(stdout, "Receive %ld bytes and we hope :- %d bytes\n", len, remain_data);
		}
		else {
			fprintf(stdout, "Missing some data, Retrying....\n");
		}
		send(sock, aux, MAX_BUFFER_SIZE, 0);		

	}
	fclose(received_file);

	close(sock);

	return 0; 
} 
