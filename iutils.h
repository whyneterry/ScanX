#ifndef _IUTILS_H
#define _IUTILS_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/select.h>
#include <errno.h>
#include <unistd.h>

#define DEFAULT_PORT 1

typedef struct scanArgv {
	struct sockaddr_in *sin;
	int ai_family;
	int socktype;
	char *ip;
	int port;
} scanArgv;

int justgetaddrinfo(int ai_family,
					int socktype,
					char *domain_name,
					char *service,
					struct addrinfo **info);

int createSIN(struct sockaddr_in *sin, int ai_family, char *ip, int port);

int nonblockSocketMode(int *socketfd);

int singleScan(int *socketfd, struct sockaddr_in *sin);

void *startScan(void *args);

scanArgv *makeScanArgs(struct sockaddr_in *sin, char *ip, int new_port);

#endif
