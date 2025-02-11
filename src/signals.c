#include "signals.h"

struct sigaction act;

void signal_handler_init() {
	memset(&act, 0, sizeof(act));
	act.sa_sigaction = signal_handler;
	act.sa_flags = SA_SIGINFO;
	sigaction(SIGHUP, &act, NULL);  // (SIGHUP  1)  controlling parent/terminal exits
	sigaction(SIGINT, &act, NULL);  // (SIGINT  2)  ctrl-c
	sigaction(SIGTERM, &act, NULL); // (SIGTERM 15) normal kill command
	//printf("%s\n", __func__);
}

void signal_handler(int signum, __attribute__((unused)) siginfo_t *info, __attribute__((unused)) void *ptr) {
	//printf("%s - received signal %d\n", __func__, signum);
	//printf("%s - from process %lu\n", __func__, (unsigned long)info->si_pid);

	bool isIgnored = false;

	switch (signum) {
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

	// TODO: do cleanup here - freeing memory, closing files, etc

	if (!isIgnored) { exit(signum); }
}