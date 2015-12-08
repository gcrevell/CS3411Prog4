// -----------------------------------------------------------
// NAME : Gabriel Revells                    User ID: gcrevell
// DUE DATE : 12/11/2015
// PROGRAM ASSIGNMENT #3
// FILE NAME : xxxx.yyyy.zzzz (your unix file name)
// PROGRAM PURPOSE :
//    A couple of lines describing your program briefly
// -----------------------------------------------------------

// Read from remote file, copy to local file.

#include "client.h"

// -----------------------------------------------------------
// FUNCTION  main :
//    The main function. Reads a remote file using client and
//    writes it to a local file. Uses remote file in.txt unless
//    a third argument is passed.
// PARAMETER USAGE :
//    int argc - The number of arguments.
//    char **argv - The arguments.
// RETURN VALUE :
//    int Main return.
// -----------------------------------------------------------
int main(int argc, char **argv) {
	if (argc < 3) {
		// Not enough args
		char* buf = {"Too few aruments. Use format rclient2 <hostname> <port>\n"};
		write(1, buf, strlen(buf));
		exit(-1);
	}
	
	init(argv[1], atoi(argv[2]));
	
	int inFd;
	if (argc > 3) {
		if ((inFd = r_open(argv[3], O_RDONLY, 0)) < 0) {
			// Failed to open remote out
			char* buf = {"Failed to open the input file on the remote machine.\n"};
			write(1, buf, strlen(buf));
			exit(-2);
		}
	} else {
		if ((inFd = r_open("in.txt", O_RDONLY, 0)) < 0) {
			// Failed to open remote out
			char* buf = {"Failed to open the input file on the remote machine.\n"};
			write(1, buf, strlen(buf));
			exit(-2);
		}
	}
	
	int outFd;
	char buf[1024];
	
	outFd = open("out.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
	
	int cnt;
	while ((cnt = r_read(inFd, buf, 1024)) > 0) {
		if (write(outFd, buf, cnt) < 0) {
			// Write failed
			char* buf = {"The write failed.\n"};
			write(1, buf, strlen(buf));
		}
	}
	
	r_close(inFd);
	close(outFd);
}
