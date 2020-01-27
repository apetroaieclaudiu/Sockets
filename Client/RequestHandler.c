#include "RequestHandler.h"
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

void	remove_spaces(char *s) {
	const char *d = s;
	do
	{
		while (*d == ' ') {
			++d;
		}
	}while (*s++ == *d++);
}

int	verifyMessageSize(int requestedSize, int receivedSize) {
	if (requestedSize != receivedSize)
		return 0;
	return 1;
}

void	manageRequest(int sd, char *message) {
	int 	messageSize;
	char	mess[MAX_BUFFER_SIZE];
	int 	offset;
	char 	*auxStr, *filename;
	const char deli[] = "&";
	strcpy(mess, message);

	if (strstr(mess, "Size") != NULL) {
		auxStr = strtok(mess, deli);
		strncpy(auxStr, auxStr, strlen(auxStr));
		remove_spaces(auxStr);
		sendFileSize(sd, auxStr);
	}
	else {
		filename = strtok(mess, deli);
		strncpy(filename, filename, strlen(filename));
		auxStr = strtok(NULL, "");
		offset = atoi(auxStr);
		remove_spaces(filename);
		sendFrame(sd, filename, offset);
	}
}

void	sendFrameRequest(int sd, char *filename, int offset) {
	char requestMessage[MAX_BUFFER_SIZE];
	char offsetStr[MAX_BUFFER_SIZE];

	sprintf(offsetStr, "%d", offset);
	strcpy(requestMessage, filename);
	strcat(requestMessage, " & ");
	strcat(requestMessage, offsetStr);

	send(sd, requestMessage, MAX_BUFFER_SIZE, 0);
}

void	sendFrame(int sd, char *filename,int offset) {
	int		offst = offset;	
	char		auxStr[MAX_BUFFER_SIZE];
	int 		fp;
	char 		fileSize[MAX_BUFFER_SIZE];
	struct stat 	file_stat;
	int		length;
	int error = 0;
	socklen_t len = sizeof (error);
	int retval = getsockopt (sd, SOL_SOCKET, SO_ERROR, &error, &len);

	if (retval != 0) {
		/* there was a problem getting the error code */
		fprintf(stderr, "error getting socket error code: %s\n", strerror(retval));
		return;
	}

	if (error != 0) {
		/* socket has a non zero error status */
		fprintf(stderr, "socket error: %s\n", strerror(error));
	}

	strcpy(auxStr, filename);
	auxStr[strlen(auxStr) - 1] = 0;
	fp= open(auxStr, O_RDONLY);

	if (fp == -1) {
		fputs("File error", stderr);
		return ;
	}

	if (fstat(fp, &file_stat) < 0) {
		fprintf(stderr, "error on sending greetings --> %s", strerror(errno));
		exit(EXIT_FAILURE);
	}

	length = sendfile(sd, fp, &offst, MAX_BUFFER_SIZE);
	close(fp);
}

int 	getFileSize(int sd) {
	char fileSize[MAX_BUFFER_SIZE];
	int size;

	recv(sd, fileSize, MAX_BUFFER_SIZE, 0);
	size  = atoi(fileSize);

	return size;
}

void	sendFileSizeRequest(int sd, char *filename) {
	char requestMessage[MAX_BUFFER_SIZE];
	strcpy(requestMessage, filename);
	strcat(requestMessage, " & Size");
	send(sd, requestMessage, MAX_BUFFER_SIZE, 0);
}

void 	sendFileSize(int sd, char *filename) {
	char		auxStr[MAX_BUFFER_SIZE];
	int 		fp;
	char 		fileSize[MAX_BUFFER_SIZE];
	struct stat 	file_stat;

	strcpy(auxStr, filename);
	auxStr[strlen(auxStr) - 1] = 0;
	fp= open(auxStr, O_RDONLY);

	if (fp == -1) {
		fputs("File error", stderr);
		return ;
	}

	if (fstat(fp, &file_stat) < 0) {
		fprintf(stderr, "error on sending greetings --> %s", strerror(errno));
		exit(EXIT_FAILURE);
	}

	sprintf(fileSize, "%d", file_stat.st_size);
	send(sd, fileSize, MAX_BUFFER_SIZE, 0);
	close(fp);
}

int	getFrame(int sd, char *message, int expectedSize, int *length) {
	*length = recv(sd, message, MAX_BUFFER_SIZE, 0);

	if (verifyMessageSize(expectedSize, *length))
		return 1;
	return 0;
}
