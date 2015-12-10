// -----------------------------------------------------------
// NAME : Gabriel Revells                    User ID: gcrevell
// DUE DATE : 12/11/2015
// PROGRAM ASSIGNMENT #3
// FILE NAME : server.c
// PROGRAM PURPOSE :
//    This file is used as the server side of the server-client.
//    It will create a socket then listen for connections on
//    that socket. Upon accepting a socket, the program forks
//    to allow a child process to handle the incoming connection
//    while the parent waits for another connection.
// -----------------------------------------------------------

#define open_call		1
#define close_call		2
#define read_call		3
#define write_call		4
#define seek_call		5

#include <strings.h>		// bzero
#include <stdio.h>			// printf
#include <errno.h>			// errno val
#include <unistd.h>			// write
#include <fcntl.h>			// open
#include <sys/socket.h>		// accept
#include <netinet/in.h>		// socket constants
#include <stdlib.h>			// malloc, free
#include <arpa/inet.h>

// -----------------------------------------------------------
// FUNCTION  main :
//    This method runs the server side of the server-client.
//    It will create a socket then listen for connections on
//    that socket. Upon accepting a socket, the program forks
//    to allow a child process to handle the incoming connection
//    while the parent waits for another connection.
// PARAMETER USAGE :
//    int argc - The number of arguments.
//    char **argv - The arguments.
// RETURN VALUE :
//    int The main return.
// -----------------------------------------------------------
int main(int agrc, char **argv) {
	int listener, conn;
	socklen_t length;
	char ch;
	struct sockaddr_in s1, s2 ;
	
	listener = socket( AF_INET, SOCK_STREAM, 0 );
	bzero((char *) &s1, sizeof(s1));
	s1.sin_family = ( short ) AF_INET;
	s1.sin_addr.s_addr = htonl(INADDR_ANY);
	
	s1.sin_port = htons(0);
	bind(listener, (struct sockaddr *)&s1, sizeof(s1));
	length = sizeof(s1);
	getsockname(listener, (struct sockaddr *)&s1, &length);
	printf("RSTREAM:: assigned port number %d\n", ntohs ( s1.sin_port ) ) ;
	
	listen(listener, 5);
	length = sizeof(s2);
	
	while (1) {
		conn = accept(listener, (struct sockaddr *)&s2, &length);
		
		if (conn < 0) {
			printf("Connection failed!\n");
		} else {
			if (fork() == 0) {
				printf("connected\n");
				while (read(conn, &ch, 1) != 0) {
					
					if (ch == open_call) {
						// Open call.
						// Read in filename
						char filename[1024];
						int cnt = 0;
						
						do {
							if (read(conn, &ch, 1) <= 0) {
								return -1;
							}
						} while ((filename[cnt++] = ch) != 0);
						
						int oflag;
						int mode;
						
						if (read(conn, (char *) &oflag, sizeof(oflag)) <= 0) {
							return -1;
						}
						if (read(conn, (char *) &mode, sizeof(mode)) <= 0) {
							return -1;
						}
						
						oflag = ntohl(oflag);
						mode = ntohl(mode);
						
						int fd = open(filename, oflag | O_CREAT, mode);
						
						char out[8];
						
						fd = htonl(fd);
						errno = htonl(errno);
						
						bcopy(&fd, out, sizeof(fd));
						bcopy(&errno, &out[4], sizeof(errno));
						
						if (write(conn, out, 8) < 0) {
							return -1;
						}
					} else if (ch == close_call) {
						// close file
						int fd;
						
						if (read(conn, (char *) &fd, sizeof(fd)) <= 0) {
							return -1;
						}
						
						fd = htonl(close(ntohl(fd)));
						errno = htonl(errno);
						
						if (write(conn, (char *) &fd, sizeof(fd)) < 0) {
							return -1;
						}
						if (write(conn, (char *) &errno, sizeof(errno)) < 0) {
							return -1;
						}
					} else if (ch == read_call) {
						// Read fd
						int fd;
						if (read(conn, (char *) &fd, sizeof(fd)) <= 0) {
							return -1;
						}
						
						fd = ntohl(fd);
						
						// Read buffer size
						int size;
						if (read(conn, (char *) &size, sizeof(size)) <= 0) {
							return -1;
						}
						size = ntohl(size);
						
						// Read max of that number of characters
						char * buf = malloc(size);
						int ret = htonl(read(fd, buf, size));
						
						// Write actual num of characters read
						if (write(conn, (char *) &ret, sizeof(ret)) < 0) {
							return -1;
						}
						
						// Write characters read
						if (ret > 0) {
							if (write(conn, buf, ret) < 0) {
								return -1;
							}
						}
						
						errno = htonl(errno);
						// Write errno
						if (write(conn, (char *) &errno, sizeof(errno)) < 0) {
							return -1;
						}
						
						free(buf);
					} else if (ch == write_call) {
						// Read fd
						int fd;
						if (read(conn, (char *) &fd, sizeof(fd)) <= 0) {
							return -1;
						}
						fd = ntohl(fd);
						
						// Read size of message
						int size;
						if (read(conn, (char *) &size, sizeof(size))  <= 0) {
							return -1;
						}
						size = ntohl(size);
						
						// Read message
						char * buf = malloc(size);
						if (read(conn, buf, size) <= 0) {
							return -1;
						}
						
						// Write message to fd
						int ret = htonl(write(fd, buf, size));
						
						// Write responce from write
						if (write(conn, (char *) &ret, sizeof(ret)) < 0) {
							return -1;
						}
						
						// Write errno
						errno = htonl(errno);
						if (write(conn, (char *) &errno, sizeof(errno)) < 0) {
							return -1;
						}
						
						free(buf);
					} else if (ch == seek_call) {
						// Read fd
						int fd;
						if (read(conn, (char *) &fd, sizeof(fd)) <= 0) {
							return -1;
						}
						fd = ntohl(fd);
						
						// Read offset int
						int offset;
						if (read(conn, (char *) &offset, sizeof(offset)) <= 0) {
							return -1;
						}
						offset = ntohl(offset);
						
						// Read whence int
						int whence;
						if (read(conn, (char *) &whence, sizeof(whence)) <= 0) {
							return -1;
						}
						whence = ntohl(whence);
						
						// Seek
						int ret = htonl(lseek(fd, offset, whence));
						
						// Write responce from lseek
						if (write(conn, (char *) &ret, sizeof(ret)) < 0) {
							return -1;
						}
						
						// Write errno
						errno = htonl(errno);
						if (write(conn, (char *) &errno, sizeof(errno)) < 0) {
							return -1;
						}
					} else {
						printf("ERROR: An unknown opcode was recieved.");
					}
				}
				return 0;
			}
		}
	}
	
}
