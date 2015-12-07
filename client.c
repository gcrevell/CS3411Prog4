#define open_call		1
#define close_call		2
#define read_call		3
#define write_call		4
#define seek_call		5

#include "client.h"

int sd = -5;

int init(char* host, int port) {
	sd = socket(AF_INET , SOCK_STREAM , 0);
	
	struct sockaddr_in remote;
	bzero((char *) &remote, sizeof(remote));
	remote.sin_family = (short) AF_INET;
	
	struct hostent* h = gethostbyname(host);
	
	bcopy ((char *)h->h_addr_list[0], (char *)&remote.sin_addr, h->h_length) ;
	remote.sin_port = htons(port);
	
	if (connect(sd, (struct sockaddr *)&remote, sizeof(remote)) < 0) {
		// Connection failed.
		printf("Connection failed.\n");
		return -1;
	}
	
	return 0;
}

int r_open(const char* filename, int oflag, int mode) {
	if (sd == -5) {
		return -2;
	}
	
	// Write open command to msg
	int size = 1 +
		strlen(filename) + 1 +
		sizeof(oflag) +
		sizeof(mode);
	
	char * msg = malloc(size);
	
	size = 0;
	
	msg[size++] = open_call;
	
	strcpy(&msg[size], filename);
	size += strlen(filename);
	msg[size++] = 0;
	
	bcopy(&oflag, &msg[size], sizeof(oflag));
	size += sizeof(oflag);
	
	bcopy(&mode, &msg[size], sizeof(mode));
	size += sizeof(mode);
	
	// Write message to socket
	write(sd, msg, size);
	
	// revieve fd back and return
	read(sd, msg, 8);
	
	int out;
	int outErr;
	
	bcopy(msg, &out, sizeof(int));
	bcopy(&msg[4], &outErr, sizeof(int));
	
	errno = outErr;
	
	free(msg);
	
	return out;
}

int r_close(int fd) {
	if (sd == -5) {
		return -2;
	}
	
	char msg[5];	// Opcode plus fd
	
	msg[0] = close_call;
	bcopy(&fd, &msg[1], sizeof(fd));
	
	write(sd, msg, 5);
	
	// revieve fd back and return
	read(sd, msg, 8);
	
	int out;
	int outErr;
	
	bcopy(msg, &out, sizeof(int));
	bcopy(&msg[4], &outErr, sizeof(int));
	
	errno = outErr;
	
	return out;
}

int r_read(int fd, void *buf, int size) {
	if (sd == -5) {
		return -2;
	}
	
	char opcode = read_call;
	write(sd, &opcode, 1);
	
	write(sd, (char *) &fd, sizeof(fd));
	write(sd, (char *) &size, sizeof(size));
	
	read(sd, (char *) &size, sizeof(size));
	
	if (size > 0) {
		read(sd, buf, size);
	}
	
	read(sd, &errno, sizeof(errno));
	
	return size;
}

int r_write(int fd, const void *buf, int size) {
	if (sd == -5) {
		return -2;
	}
	
	char opcode = write_call;
	write(sd, &opcode, 1);
	
	write(sd, (char *) &fd, sizeof(fd));
	write(sd, (char *) &size, sizeof(size));
	
	write(sd, buf, size);
	
	// revieve responce and return
	int out;
	int outErr;
	
	read(sd, (char *) &out, sizeof(int));
	read(sd, (char *) &outErr, sizeof(int));
	
	errno = outErr;
	
	return out;
}

int r_lseek(int fd, int offset, int whence) {
	if (sd == -5) {
		return -2;
	}
	
	char opcode = seek_call;
	write(sd, &opcode, 1);
	
	write(sd, (char *) &fd, sizeof(fd));
	write(sd, (char *) &offset, sizeof(offset));
	write(sd, (char *) &whence, sizeof(whence));
	
	// revieve responce and return
	int out;
	int outErr;
	
	read(sd, (char *) &out, sizeof(int));
	read(sd, (char *) &outErr, sizeof(int));
	
	errno = outErr;
	
	return out;
}
