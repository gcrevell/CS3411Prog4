// -----------------------------------------------------------
// NAME : Gabriel Revells                    User ID: gcrevell
// DUE DATE : 12/11/2015
// PROGRAM ASSIGNMENT #3
// FILE NAME : xxxx.yyyy.zzzz (your unix file name)
// PROGRAM PURPOSE :
//    A couple of lines describing your program briefly
// -----------------------------------------------------------

// Local file in, copy to remote file

#include "client.h"

// -----------------------------------------------------------
// FUNCTION  main :
//    The main function. Reads a local file and writes it to a
//    remote file. Uses local file in.txt unless a third
//    argument is passed.
// PARAMETER USAGE :
//    int argc - The number of arguments.
//    char **argv - The arguments.
// RETURN VALUE :
//    int Main return.
// -----------------------------------------------------------
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
