#include <stdlib.h>
#include <pthread.h>

#include "dbg.h"
#include "iutils.h"

int parseArgs(int argc, char *argv[], char **ip, int *port)
{
	log_info("argc: %d, argv[1]: %s", argc, argv[1]);
	struct sockaddr_in temp;
	memset(&temp, 0, sizeof(temp));
	if (argc <= 1) {
		return -1;
	}
	else if (argc == 4) {
		if ( strncmp(argv[2], "-p", 2) != 0 ) {
			return -1;
		}
		else {
			*port = atoi(argv[3]);
			int rc = inet_pton(AF_INET, argv[1], &temp);
			if (rc == 0) {
				return -1;
			}
			*ip = argv[1];
		}
	}
	else if (argc == 2) {
		int rc = inet_pton(AF_INET, argv[1], &temp);
		if (rc == 0) {
			return -1;
		}
		*ip = argv[1];
		*port = DEFAULT_PORT;
	}

	return 0;
}

int main(int argc, char *argv[])
{
	
	char *ip = NULL;
	int port;
	int rc = parseArgs(argc, argv, &ip, &port);
	check(rc == 0, "Invalid usage. USAGE: ./scanx <IPv4_addr> -p <port_number>");

	pthread_t thread1, thread2, thread3;
	
	for (port = port; port < 1024; port += 3) {
		struct sockaddr_in sin;	
		pthread_create(&thread1, NULL, startScan, makeScanArgs(&sin, ip, port));
		pthread_create(&thread2, NULL, startScan, makeScanArgs(&sin, ip, port + 1));
		pthread_create(&thread3, NULL, startScan, makeScanArgs(&sin, ip, port + 2));
		
		pthread_join(thread1, NULL);
		pthread_join(thread2, NULL);
		pthread_join(thread3, NULL);
	}

	return 0;
	
error:
	return -1;
}
