#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <getopt.h>
#include <iostream>
#include <list>
#include "pxfs/client/libfs.h"
#include "tool/testfw/integrationtest.h"
#include "tool/testfw/testfw.h"

void test(const char*);

int
main(int argc, char *argv[])
{
	pthread_attr_t    attr;
	int               ret;
	int               debug_level = -1;
	char              ch = 0;
	char*             xdst=NULL;
	const char*       client_tag = "Client";	
	extern int        opterr;
	
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	opterr=0;
	while ((ch = getopt(argc, argv, "d:h:li:t:T:"))!=-1) {
		switch (ch) {
			case 'T':
				/* test framework argument -- ignore */
				break;
			case 'd':
				debug_level = atoi(optarg);
				break;
			case 'h':
				xdst = optarg;
				break;
			case 'l':
				assert(setenv("RPC_LOSSY", "5", 1) == 0);
				break;
			case 't':
				client_tag = optarg;
				break;
			default:
				break;
		}
	}

	if (!xdst) {
		return -1;
	}

	// set stack size to 32K, so we don't run out of memory
	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr, 32*1024);
	
	libfs_init(argc, argv);

	test(client_tag);

	libfs_shutdown();
	return ret;
}
