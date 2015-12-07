// Local file in, copy to remote file

#include "client.h"

int main(int argc, char **argv) {
	if (argc < 3) {
		// Not enough args
		char* buf = {"Too few aruments. Use format rclient1 <hostname> <port>\n"};
		write(1, buf, strlen(buf));
		exit(-1);
	}
	
	init(argv[1], atoi(argv[2]));
	
	int outFd;
	if ((outFd = r_open("out.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666)) < 0) {
		// Failed to open remote out
		printf("outFd = %d\n", 0666);
		char* buf = {"Failed to open the output file on the remote machine.\n"};
		write(1, buf, strlen(buf));
		exit(-2);
	}
	
	int inFd;
	char buf[1024];
	
	if (argc > 3) {
		inFd = open(argv[3], O_RDONLY);
	} else {
		inFd = open("in.txt", O_RDONLY);
	}
	
	int cnt;
	while ((cnt = read(inFd, buf, 1024)) > 0) {
		if (r_write(outFd, buf, cnt) < 0) {
			// Write failed
			char* buf = {"The write failed.\n"};
			write(1, buf, strlen(buf));
		}
	}
	
	r_close(outFd);
	close(inFd);
}
