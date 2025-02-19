#include <chrono>
#include <csignal>
#include <cstring>
#include <iostream>
#include <thread>
#include <vector>

class SignalHandler {
public:
	SignalHandler(std::vector<int> signums, void(*handler)(int)) {
		struct sigaction sa;
		std::memset(&sa, 0, sizeof(sa));
		sa.sa_handler = handler;
		sa.sa_flags = 0;
		sigemptyset(&sa.sa_mask);
		for (auto signum : signums) {
			sigaction(signum, &sa, nullptr);
		}
	}
};

/*
SignalHandler handler({SIGINT}, +[](int signum) {
	std::cout << "signal received: " << signum << std::endl;
	if (signum == SIGINT) { exit(signum); }
});

(SIGHUP  1)  controlling parent/terminal exits
(SIGINT  2)  ctrl-c
(SIGTERM 15) normal kill command
*/