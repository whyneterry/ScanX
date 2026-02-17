#include <stdlib.h>

#include "iutils.h"

int justgetaddrinfo(int ai_family,
					int ai_socktype,
					char *domain_name,
					char *service,
					struct addrinfo **info)
{
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = ai_family;
	hints.ai_socktype = ai_socktype;

	int rc = getaddrinfo(domain_name, service, &hints, info);
	if (rc != 0) {
		return -1;
	}
	
	return 0;
}

int createSIN(struct sockaddr_in *sin, int ai_family, char *ip, int port)
{
	if (ip == NULL) {
		return -1;
	}

	memset(sin, 0, sizeof(struct sockaddr_in));
	sin->sin_family = ai_family;
	sin->sin_port = htons(port);
	inet_pton(AF_INET, ip, &(sin->sin_addr.s_addr));

	return 0;	
}

int nonblockSocketMode(int *socketfd)
{
	int flags = -1;
	flags = fcntl(*socketfd, F_GETFL);
	if (flags == -1) {
		return -1;
	}

	int rc = fcntl(*socketfd, F_SETFL, flags | O_NONBLOCK);
	if (rc != 0) {
		return -1;
	}

	return 0;
}

int singleScan(int *socketfd, struct sockaddr_in *sin)
{
	int port = ntohs(sin->sin_port);
	int rc = connect(*socketfd, (struct sockaddr *)sin, sizeof(*sin));
	
	if (rc == 0) {
			printf("[!]Port[%d] | Status[OPENED]\n", port);
	}
	else if (rc == -1) {
		fd_set writefds;
		FD_ZERO(&writefds);
		FD_SET(*socketfd, &writefds);
		if (FD_ISSET(*socketfd, &writefds) == 0) {
			printf("Error with FD_SET.");
			return -1;
		}		

		struct timeval timeout = { .tv_sec = 3, .tv_usec = 0 };

		int free = select(*socketfd + 1, NULL, &writefds, NULL, &timeout);
		if(free == -1) {
			printf("Error with select.\n");
			return -1;			
		}

		if (free > 0) {
			int error, rc;
			socklen_t errorlen = sizeof(error);
			rc = getsockopt(*socketfd, SOL_SOCKET, SO_ERROR, &error, &errorlen);
			if (rc != 0) {
				printf("Error with getsockopt.\n");
				return -1;
			}

			if (error == 0) {
				printf("Port[%d] | Status[OPENED]\n", port);
			}
			else {
				printf("Port[%d] | Status[CLOSED]\n", port);
			}
		}
		else if (free == 0) {
			printf("Port[%d] | Status[FILTERED]\n", port);
		}
	}

	return 0;
}

void *startScan(void *args)
{
	scanArgv *scanargs = (scanArgv *)args;

	int sockfd = socket(scanargs->ai_family, scanargs->socktype, 0);
	if (sockfd == -1) {
		printf("Error with socket\n");
		return NULL;
	}

	int rc = nonblockSocketMode(&sockfd);
	if (rc != 0) {
		printf("Error with changing socket's mode.\n");
		return NULL;
	}

	rc = createSIN(scanargs->sin, scanargs->ai_family, scanargs->ip, scanargs->port);

	rc = singleScan( &sockfd, scanargs->sin);
	if (rc != 0) {
		printf("There was an error in singleScan.\n");
	}	

	free(scanargs);	

	return NULL;
}

scanArgv *makeScanArgs(struct sockaddr_in *sin, char *ip, int new_port)
{
	scanArgv *new_args = calloc(1, sizeof(scanArgv));
	//memcpy(new_args, scanargs, sizeof(scanArgv));
	new_args->sin = sin;
	new_args->ai_family = AF_INET;
	new_args->socktype = SOCK_STREAM;
	new_args->ip = ip;
	new_args->port = new_port;

	return new_args;
}
