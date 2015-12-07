#ifndef client_h
#define client_h

#include <fcntl.h>			// Open
#include <unistd.h>			// Close
#include <sys/types.h>		// Read
#include <sys/uio.h>		// Read
#include <unistd.h>			// Read, write, lseek
#include <stdlib.h>			// malloc
#include <string.h>			// strcpy, strlen
#include <strings.h>			// bzero, bcopy
#include <stdio.h>			// printf
#include <netinet/in.h>		// socket constants
#include <sys/socket.h>		// accept
#include <netdb.h>			// getsockname
#include <errno.h>

int init(char* host, int port);
int r_open(const char* filename, int oflag, int mode);
int r_close(int fd);
int r_read(int fd, void *buf, int size);
int r_write(int fd, const void *buf, int size);
int r_lseek(int fd, int offset, int whence);

#endif
