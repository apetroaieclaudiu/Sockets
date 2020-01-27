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
#include "RequestHandler.h"

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
					manageRequest(&sd, buffer);
				}
			}
		}
	}


	return 0;
}
