// -----------------------------------------------------------
// NAME : Gabriel Revells                    User ID: gcrevell
// DUE DATE : 12/11/2015
// PROGRAM ASSIGNMENT #3
// FILE NAME : client.c
// PROGRAM PURPOSE :
//    This file defines the API for client, which extents the
//    standard system calls open, close, read, write, and
//    seek across a socket.
// -----------------------------------------------------------

#define open_call		1
#define close_call		2
#define read_call		3
#define write_call		4
#define seek_call		5

#include "client.h"

int sd = -5;

// -----------------------------------------------------------
// FUNCTION  init :
//    This funciton is called to initialize the socket for
//    communication.
// PARAMETER USAGE :
//    char* host - The hostname.
//    int port - The port to communicate over.
// RETURN VALUE :
//    int 0 if successful, -1 if error
// -----------------------------------------------------------
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

// -----------------------------------------------------------
// FUNCTION  r_open :
//    Wrapper for the open call. Uses a socket to open on a
//    remote server.
// PARAMETER USAGE :
//    const char* filename - The name of the file to open.
//    int oflag - The flags to pass to open.
//    int mode - The access mode to use if creating a file.
// RETURN VALUE :
//    int Upon success the opened file descriptor is returned.
//    Negative otherwise.
// -----------------------------------------------------------
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
	
	oflag = htonl(oflag);
	mode = htonl(mode);
	
	bcopy(&oflag, &msg[size], sizeof(oflag));
	size += sizeof(oflag);
	
	bcopy(&mode, &msg[size], sizeof(mode));
	size += sizeof(mode);
	
	// Write message to socket
	if (write(sd, msg, size) < 0) {
		return -1;
	}
	
	// revieve fd back and return
	if (read(sd, msg, 8) <= 0) {

		return -1;
	}
	
	int out;
	int outErr;
	
	bcopy(msg, &out, sizeof(int));
	bcopy(&msg[4], &outErr, sizeof(int));
	
	errno = ntohl(outErr);
	
	free(msg);
	
	return ntohl(out);
}

// -----------------------------------------------------------
// FUNCTION  r_close :
//    A wrapper function for the close system call. Closes a
//    file on a remote server.
// PARAMETER USAGE :
//    int fd - The file descriptor to close.
// RETURN VALUE :
//    int Upon success, 0. Negative if error
// -----------------------------------------------------------
int r_close(int fd) {
	if (sd == -5) {
		return -2;
	}
	
	char msg[5];	// Opcode plus fd
	
	fd = htonl(fd);
	
	msg[0] = close_call;
	bcopy(&fd, &msg[1], sizeof(fd));
	
	if (write(sd, msg, 5) < 0) {
		return -1;
	}
	
	// revieve fd back and return
	if (read(sd, msg, 8) <= 0) {
		return -1;
	}
	
	int out;
	int outErr;
	
	bcopy(msg, &out, sizeof(int));
	bcopy(&msg[4], &outErr, sizeof(int));
	
	errno = ntohl(outErr);
	
	return ntohl(out);
}

// -----------------------------------------------------------
// FUNCTION  r_read :
//    Wrapper function for the read system call. Reads a string
//    from a remote file.
// PARAMETER USAGE :
//    int fd - The file descriptor of the file to read.
//    void *buf - The buffer to read values into.
//    int size - The number of bytes to read
// RETURN VALUE :
//    int The number of values read, or negative if error
// -----------------------------------------------------------
int r_read(int fd, void *buf, int size) {
	if (sd == -5) {
		return -2;
	}
	
	char opcode = read_call;
	if (write(sd, &opcode, 1) < 0) {
		return -1;
	}
	
	fd = htonl(fd);
	if (write(sd, (char *) &fd, sizeof(fd)) < 0) {
		return -1;
	}
	size = htonl(size);
	if (write(sd, (char *) &size, sizeof(size)) < 0) {
		return -1;
	}
	
	if (read(sd, (char *) &size, sizeof(size)) <= 0) {
		return -1;
	}
	size = ntohl(size);
	
	if (size > 0) {
		if (read(sd, buf, size)  <= 0) {
			return -1;
		}
	}
	
	int err;
	if (read(sd, &err, sizeof(errno)) <= 0) {
		return -1;
	}
	
	errno = ntohl(err);
	
	return size;
}

// -----------------------------------------------------------
// FUNCTION  r_write :
//    Wrapper function for write system call. Writes to a file
//    across a socket.
// PARAMETER USAGE :
//    int fd - The file descriptor to write to.
//    const void *buf - The string to write.
//    int size - The number of bytes to write.
// RETURN VALUE :
//    int The number of bytes written, or negative if error.
// -----------------------------------------------------------
int r_write(int fd, const void *buf, int size) {
	if (sd == -5) {
		return -2;
	}
	
	char opcode = write_call;
	if (write(sd, &opcode, 1) < 0) {
		return -1;
	}
	
	fd = htonl(fd);
	if (write(sd, (char *) &fd, sizeof(fd)) < 0) {
		return -1;
	}
	
	int size2 = htonl(size);
	if (write(sd, (char *) &size2, sizeof(size)) < 0) {
		return -1;
	}
	
	if (write(sd, buf, size) < 0) {
		return -1;
	}
	
	// revieve responce and return
	int out;
	int outErr;
	
	if (read(sd, (char *) &out, sizeof(int)) <= 0) {
		return -1;
	}
	if (read(sd, (char *) &outErr, sizeof(int)) <= 0) {
		return -1;
	}
	
	errno = ntohl(outErr);
	
	return ntohl(out);
}

// -----------------------------------------------------------
// FUNCTION  r_lseek :
//    Wrapper for lseek. Seeks a file over a socket.
// PARAMETER USAGE :
//    int fd - The file descriptor to seek.
//    int offset - The amount to offset from.
//    int whence - Where to offset from (SEEK_SET _CUR or _END)
// RETURN VALUE :
//    int The total offset of the file.
// -----------------------------------------------------------
int r_lseek(int fd, int offset, int whence) {
	if (sd == -5) {
		return -2;
	}
	
	char opcode = seek_call;
	if (write(sd, &opcode, 1) < 0) {
		return -1;
	}
	
	fd = htonl(fd);
	if (write(sd, (char *) &fd, sizeof(fd)) < 0) {
		return -1;
	}
	offset = htonl(offset);
	if (write(sd, (char *) &offset, sizeof(offset)) < 0) {
		return -1;
	}
	whence = htonl(whence);
	if (write(sd, (char *) &whence, sizeof(whence)) < 0) {
		return -1;
	}
	
	// revieve responce and return
	int out;
	int outErr;
	
	if (read(sd, (char *) &out, sizeof(int)) <= 0) {
		return -1;
	}
	if (read(sd, (char *) &outErr, sizeof(int)) <= 0) {
		return -1;
	}
	
	errno = ntohl(outErr);
	
	return ntohl(out);
}
