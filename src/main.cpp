#include <iostream>
#include "MouseVirtual.h"
#include "PairManager.h"
#include "SignalHandler.h"

int main () {

	SignalHandler handler({SIGHUP}, +[](int signum) {
		std::cout << "signal received:" << signum << "\n";
	});
	
	MouseVirtual mouse;

	PairManager manager;
	manager.loop();

	return 0;
}

/*
g++ -std=c++20 -Wall -Wextra -Wshadow *.cpp -o main
sudo ./main

//(new PairManager())->loop();
*/