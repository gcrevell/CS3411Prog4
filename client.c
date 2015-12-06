#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <strings.h>
#include <netdb.h> 
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

int main(int argc, char **argv) {
	char* remhost;
	u_short remport;
	struct sockaddr_in remote;
	struct hostent* h;
	char msg[] = { "the quick brown fox does not jump over the super lazy dog" };
	
	if (argc < 3) {
		printf("There are too few arguments!");
		exit(0);
	}
	
	remhost = argv[1];
	remport = atoi(argv[2]);
	
	int sd = socket(AF_INET , SOCK_STREAM , 0);
	
	bzero((char *) &remote, sizeof(remote));
	remote.sin_family = ( short ) AF_INET;
	h = gethostbyname(remhost);
	bcopy ((char *)h->h_addr, (char *)&remote.sin_addr, h->h_length) ;
	remote.sin_port = htons(remport);
	
	int ret = connect(sd, (struct sockaddr *)&remote, sizeof(remote));
	
	if (ret < 0) {
		printf("Connection failed.\n");
		exit(0);
	} else {
		printf("Connection established!\n");
	}
	
	write(sd, msg, sizeof(msg));
}
