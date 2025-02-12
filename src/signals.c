#include "signals.h"

struct sigaction sa;

void signal_handler_init() {
	//printf("%s\n", __func__);
	sa.sa_handler = signal_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGHUP, &sa, NULL);  // (SIGHUP  1)  controlling parent/terminal exits
	sigaction(SIGINT, &sa, NULL);  // (SIGINT  2)  ctrl-c
	sigaction(SIGTERM, &sa, NULL); // (SIGTERM 15) normal kill command
}

void signal_handler(int signal_num) {
	//printf("%s - received signal %d\n", __func__, signal_num);

	bool isIgnored = false;

	switch (signal_num) {
		case SIGHUP:
			// ignore, continue running
			isIgnored = true;
			break;
		case SIGINT:
			// accept, allow process to exit
			break;
		case SIGTERM:
			// accept, allow process to exit
			break;
		default:
			break;
	}

	// cleanup here - freeing memory, closing files, etc

	if (!isIgnored) { exit(signal_num); }
}
