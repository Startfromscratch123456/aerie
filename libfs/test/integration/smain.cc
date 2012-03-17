#include <arpa/inet.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdint.h>
#include <iostream>
#include <vector>
#include "ipc/ipc.h"
#include "common/debug.h"
#include "pxfs/server/server.h"


char*                  pathname = "/tmp/stamnos_pool"; // hardcoded; used by tests
int                    port;
pthread_attr_t         attr;


int
main(int argc, char *argv[])
{
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
	int   debug_level = 0;

	srandom(getpid());
	port = 20000 + (getpid() % 10000);

	char ch = 0;
	while ((ch = getopt(argc, argv, "csd:p:lT:"))!=-1) {
		switch (ch) {
			case 'T':
				/* test framework argument -- ignore */
				break;
			case 's':
				pathname = optarg;
			case 'd':
				debug_level = atoi(optarg);
				break;
			case 'p':
				port = atoi(optarg);
				break;
			case 'l':
				assert(setenv("RPC_LOSSY", "5", 1) == 0);
			default:
				break;
		}
	}

	dbg_init(debug_level, NULL);

	pthread_attr_init(&attr);
	// set stack size to 32K, so we don't run out of memory
	pthread_attr_setstacksize(&attr, 32*1024);

	printf("Starting file system server on port %d RPC_HEADER_SZ %d\n", port, RPC_HEADER_SZ);

	server::Server::Instance()->Start(pathname, 0, port);

	while (1) {
		sleep(1);
	}
}
