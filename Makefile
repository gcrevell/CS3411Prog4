CC = gcc
EXEC = server rclient1 rclient2
CCFLAGS = -Wall -std=c99
OBJS = rclient1.o rclient2.o server.o client.o

all: rclient1 rclient2 rserver

rclient1: rclient1.c client.c
	${CC} ${CCFLAGS} -lm -o rclient1 rclient1.c client.c

rclient2: rclient2.c client.c
	${CC} ${CCFLAGS} -lm -o rclient2 rclient2.c client.c

rserver: server.c
	${CC} ${CCFLAGS} -lm -o server server.c

.c.o:
	${CC} ${CCFLAGS} -c $<
    
clean:
	rm -r -f ${EXEC} ${OBJS} *.dSYM