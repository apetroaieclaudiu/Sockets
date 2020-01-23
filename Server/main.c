#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include "LinkedList.h"

#define MAX_BUFFER_SIZE 1024
#define TRUE   1
#define FALSE  0
#define PORT 8080

int main(int argc , char *argv[])
{
	char source[MAX_BUFFER_SIZE + 1], file_size[MAX_BUFFER_SIZE];
	int opt = TRUE;
	int master_socket , addrlen , new_socket , client_socket[30] ,
	    max_clients = 30 , activity, i , valread , sd;
	int max_sd;
	struct sockaddr_in address;
	struct stat file_stat;
	char buffer[MAX_BUFFER_SIZE];
	int offset;
	int remain_data, sent_bytes;
	fd_set readfds;

	char *message = "Welcome aboard! Connection succesfull. \r\n";

	for (i = 0; i < max_clients; i++)
	{
		client_socket[i] = 0;
	}

	if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
				sizeof(opt)) < 0 )
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT );

	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	printf("Listener on port %d \n", PORT);

	if (listen(master_socket, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	addrlen = sizeof(address);
	puts("Waiting for connections ...");

	while(TRUE)
	{
		FD_ZERO(&readfds);

		FD_SET(master_socket, &readfds);
		max_sd = master_socket;

		for ( i = 0 ; i < max_clients ; i++)
		{
			sd = client_socket[i];

			if(sd > 0)
				FD_SET( sd , &readfds);

			if(sd > max_sd)
				max_sd = sd;
		}

		activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);

		if ((activity < 0) && (errno!=EINTR))
		{
			printf("select error");
		}

		if (FD_ISSET(master_socket, &readfds))
		{
			if ((new_socket = accept(master_socket,
							(struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}

			printf("New connection , socket fd is %d , ip is : %s , port : %d\n"
					,new_socket, inet_ntoa(address.sin_addr) , ntohs
					(address.sin_port));

			for (i = 0; i < max_clients; i++)
			{
				if( client_socket[i] == 0 )
				{
					client_socket[i] = new_socket;
					printf("Adding to list of sockets as %d\n" , i);

					break;
				}
			}
		}

		for (i = 0; i < max_clients; i++)
		{
			sd = client_socket[i];

			if (FD_ISSET( sd , &readfds))
			{
				if ((valread = read( sd , buffer, 1024)) == 0)
				{
					getpeername(sd , (struct sockaddr*)&address , \
							(socklen_t*)&addrlen);
					printf("Host disconnected , ip %s , port %d \n" ,
							inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

					close( sd );
					client_socket[i] = 0;
				}

				else
				{
					buffer[valread] = '\0';
					int fp = open(buffer, O_RDONLY);
					int len;
					size_t newLen;
					char length[10];
					int old_offset;

					if (fp==-1) {
						fputs ("File error",stderr); 
						break;
					}

					if (fstat(fp, &file_stat) < 0) {
						fprintf(stderr, "Error fstat --> %s", strerror(errno));
						exit(EXIT_FAILURE);
					}
					offset = 0;
					old_offset = 0;
					remain_data = file_stat.st_size;
					sprintf(file_size, "%d", file_stat.st_size);
					printf("The size of the file is: %s\n", file_size);
					/* Sending file size */
					len = send(sd, file_size, sizeof(file_size), 0);
					if (len < 0)
					{
						fprintf(stderr, "Error on sending greetings --> %s", strerror(errno));

						exit(EXIT_FAILURE);
					}
					while (((sent_bytes = sendfile(sd, fp, &offset, MAX_BUFFER_SIZE)) > 0) && (remain_data > 0))
					{
						sprintf(length, "%d", sent_bytes);
						send(sd,  length, MAX_BUFFER_SIZE, 0);
						recv(sd, file_size, MAX_BUFFER_SIZE, 0);
						printf("\nRec:%s\n\nSend:%s\n", file_size, length); 
						if (strcmp(file_size, length) == 0) {
							fprintf(stdout, "1. Server sent %d bytes from file's data, offset is now : %ld and remaining data = %d\n", sent_bytes, offset, remain_data);
							remain_data -= sent_bytes;
							fprintf(stdout, "2. Server sent %d bytes from file's data, offset is now : %ld and remaining data = %d\n", sent_bytes, offset, remain_data);
						}
						else {
							fprintf(stdout, "Incomplete package. Retrying data transmission");
							offset = old_offset;
						}
						old_offset = offset;

					}
				}
			}
		}
	}


	return 0;
}
