#include "client.h"

int main(int argc, char** argv) {
	init(argv[1], atoi(argv[2]));
	
	printf("inited\n");
	
	int fd = r_open("out.txt", O_RDWR | O_CREAT, 0666);
	
	printf("Created file. fd = %d\n", fd);
	
	printf("closed? %d\n", r_close(fd));
}
