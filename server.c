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
					
					printf("Read character is %c and its ascii value is %d\n", ch, ch);
					
					if (ch == open_call) {
						// Open call.
						// Read in filename
						char filename[1024];
						int cnt = 0;
						
						do {
							read(conn, &ch, 1);
						} while ((filename[cnt++] = ch) != 0);
						
						printf("filename is %s\n", filename);
						
						int oflag;
						int mode;
						
						read(conn, (char *) &oflag, sizeof(oflag));
						read(conn, (char *) &mode, sizeof(mode));
						
						printf("oflags = |%d|\n", oflag);
						printf("mode = %d\n", mode);
						
						int fd = open(filename, oflag, mode);
						
						printf("fd = %d\n", fd);
						printf("errno = %d\n", errno);
						
						char out[8];
						
						bcopy(&fd, out, sizeof(fd));
						bcopy(&errno, &out[4], sizeof(errno));
						
						printf("writing\n");
						write(conn, out, 8);
						printf("wrote\n");
					} else if (ch == close_call) {
						// close file
						
						printf("close\n");
						
						int fd;
						
						read(conn, (char *) &fd, sizeof(fd));
						
						printf("fd = %d\n", fd);
						fd = close(fd);
						
						write(conn, (char *) &fd, sizeof(fd));
						write(conn, (char *) &errno, sizeof(errno));
					}
				}
				return 0;
			}
		}
	}
	
}
