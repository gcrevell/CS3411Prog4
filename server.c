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
							read(conn, &ch, 1);
						} while ((filename[cnt++] = ch) != 0);
						
						int oflag;
						int mode;
						
						read(conn, (char *) &oflag, sizeof(oflag));
						read(conn, (char *) &mode, sizeof(mode));
						
						int fd = open(filename, oflag/* | O_CREAT*/, mode);
						
						char out[8];
						
						bcopy(&fd, out, sizeof(fd));
						bcopy(&errno, &out[4], sizeof(errno));
						
						write(conn, out, 8);
					} else if (ch == close_call) {
						// close file
						int fd;
						
						read(conn, (char *) &fd, sizeof(fd));
						
						fd = close(fd);
						
						write(conn, (char *) &fd, sizeof(fd));
						write(conn, (char *) &errno, sizeof(errno));
					} else if (ch == read_call) {
						// Read fd
						int fd;
						read(conn, (char *) &fd, sizeof(fd));
						
						// Read buffer size
						int size;
						read(conn, (char *) &size, sizeof(size));
						
						// Read max of that number of characters
						char * buf = malloc(size);
						int ret = read(fd, buf, size);
						
						// Write actual num of characters read
						write(conn, (char *) &ret, sizeof(ret));
						
						// Write characters read
						if (ret > 0) {
							write(conn, buf, ret);
						}
						
						// Write errno
						write(conn, (char *) &errno, sizeof(errno));
						
						free(buf);
					} else if (ch == write_call) {
						// Read fd
						int fd;
						read(conn, (char *) &fd, sizeof(fd));
						
						// Read size of message
						int size;
						read(conn, (char *) &size, sizeof(size));
						
						// Read message
						char * buf = malloc(size);
						read(conn, buf, size);
						
						// Write message to fd
						int ret = write(fd, buf, size);
						
						// Write responce from write
						write(conn, (char *) &ret, sizeof(ret));
						
						// Write errno
						write(conn, (char *) &errno, sizeof(errno));
						
						free(buf);
					} else if (ch == seek_call) {
						// Read fd
						int fd;
						read(conn, (char *) &fd, sizeof(fd));
						
						// Read offset int
						int offset;
						read(conn, (char *) &offset, sizeof(offset));
						
						// Read whence int
						int whence;
						read(conn, (char *) &whence, sizeof(whence));
						
						// Seek
						int ret = lseek(fd, offset, whence);
						
						// Write responce from lseek
						write(conn, (char *) &ret, sizeof(ret));
						
						// Write errno
						write(conn, (char *) &errno, sizeof(errno));
					} else {
						printf("ERROR: An unknown opcode was recieved.");
					}
				}
				return 0;
			}
		}
	}
	
}
