#define open_call	1
#define close_call	2
#define read_call	3
#define write_call	4
#define seek_call	5

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <strings.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>


int main(int agrc, char **argv) {
	int listener, conn, length;
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
		conn=accept(listener, (struct sockaddr *)&s2, &length);
		
		if (conn < 0) {
			printf("Connection failed!\n");
		} else {
			if (fork() == 0) {
				printf("\n\nRSTREAM:: data from stream:\n");
				while ( read(conn, &ch, 1) == 1) putchar(ch);
				putchar ( '\n' );
			}
		}
	}
	
}
