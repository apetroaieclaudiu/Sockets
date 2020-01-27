#include "RequestHandler.h"
#include <string.h>
#include <stdlib.h>

int	verifyMessageSize(int requestedSize, int receivedSize) {
	if (requestedSize != receivedSize)
		return 0;
	return 1;
}

void	manageRequest(int *sd, char *message) {
	int 	messageSize;
	int 	offset;
	char 	auxStr[MAX_BUFFER_SIZE];
	char 	filename[MAX_BUFFER_SIZE];
	const char deli[] = "&";

	messageSize = recv(*sd, message, MAX_BUFFER_SIZE, 0);
	if (strstr(message, "Size") != NULL) {
		auxStr = strtok(message, deli);
		strncpy(auxStr, auxStr, strlen(auxStr) - 1);
		sendFileSize(sd, auxStr);
	}
	else {
		filename = strtok(message, deli);
		strncpy(filename, filename, strlen(filename) - 1);
		auxStr = strtok(NULL, "");
		offset = atoi(auxStr);
		sendFrame(sd, filename, offset);
}

void	sendFrameRequest(int *sd, char *filename, int offset) {
	char requestMessage[MAX_BUFFER_SIZE];
	char offsetStr[MAX_BUFFER_SIZE];
	
	sprintf(offsetStr, "%d", offset);
	strcpy(requestMessage, filename);
	strcat(requestMessage, " & ");
	strcat(requestMessage, offsetStr);

	send(*sd, requestMessage, MAX_BUFFER_SIZE, 0);
}

void	sendFrame(int *sd, char *filename,int offset) {
	int fp = open(filename, O_RDONLY);
	int offst = offset;
	struct stat file_Stat;

	if (fp == -1) {
		fputs("File error", stderr);
		break;
	}

	if (fstat(fp, &file_stat) < 0) {
		fprintf(stderr, "error on sending greetings --> %s", srderror(errno));
		exit(EXIT_FAILURE);
	}

	sendfile(*sd, fp, &offst, MAX_BUFFER_SIZE);

	close(fp);
}

int 	getFileSize(int *sd) {
	char fileSize[MAX_BUFFER_SIZE];
	int size;

	recv(*sd, fileSize, MAX_BUFFER_SIZE, 0);
	size  = atoi(fileSize);

	return size;
}

void	sendFileSizeRequest(int *sd, char *filename) {
	char requestMessage[MAX_BUFFER_SIZE];
	
	strcpy(requestMessage, filename);
	strcat(requestMessage, " & Size");

	send(*sd, requestMessage, MAX_BUFFER_SIZE, 0);
}

void 	sendFileSize(int *sd, char *filename) {
	int 		fp = open(filename, O_READONLY);
	char 		fileSize[MAX_BUFFER_SIZE];
	struct stat 	file_stat;

	if (fp == -1) {
		fputs("File error", stderr);
		break;
	}

	if (fstat(fp, &file_stat) < 0) {
		fprintf(stderr, "error on sending greetings --> %s", srderror(errno));
		exit(EXIT_FAILURE);
	}

	sprintf(filesize, "%d", file_stat.st_size);
	send(*sd, filesize, MAX_BUFFER_SIZE, 0);

	close(fp);
}

int	getFrame(int *sd, char *message, int expectedSize) {
	int len;
	
	len = recv(*sd, message, MAX_BUFFER_SIZE, 0);

	if (verifyMessageSize(expectedSize, len))
		return 1;
	return 0;
}
