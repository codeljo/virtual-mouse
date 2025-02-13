#include <iostream>
#include "MouseVirtual.h"
#include "PairManager.h"
#include "signals.h"

int main () {

	signal_handler_init();
	
	MouseVirtual mouse;

	PairManager manager;
	manager.loop();

	return 0;
}

/*
g++ -std=c++20 -Wall -Wextra -Wshadow *.c *.cpp -o main
sudo ./main

note: -std= (c99, c11, c17, c2x, etc)
note: -std= (c++11, c++14, c++17, c++20, etc)

//(new PairManager())->loop();
*/