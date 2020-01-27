
#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 1024
#endif

int 	verifyMessageSize(int requestedSize, int receivedSize);
void 	manageRequest(int *sd, char *message);
void 	sendFrameRequest(int *sd, char *filename, int offset);
void 	sendFrame(int *sd, char *filename, int offset);
int	getFileSize(int *sd);
void 	sendFileSizeRequest(int *sd, char *filename);
void	sendFileSize(int *sd, char *filename);
int	getFrame(int *sd, char *message, int expectedSize);
