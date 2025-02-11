#include <iostream>
#include "PairManager.h"
#include "signals.h"

int main () {

	signal_handler_init();

	PairManager manager;
	manager.loop();

	//(new PairManager())->loop();

	return 0;
}

/*
g++ -std=c++20 -Wall -Wextra -Wshadow *.c *.cpp -o main
sudo ./main

note: -std= (c99, c11, c17, c2x, etc)
note: -std= (c++11, c++14, c++17, c++20, etc)
*/

/*
"AR", "AR Keyboard", "Amazon Remote", "Amazon Remote Keyboard" "Logitech M510", "MX Keys Mini Keyboard"
note: "AR", "Amazon Remote" have no supported keys

{ BTN_MOUSE }, { REL_X, REL_Y, REL_WHEEL, REL_WHEEL_HI_RES }
{ KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_ENTER, KEY_KPENTER, KEY_BACK, KEY_HOMEPAGE, KEY_MENU, KEY_REWIND, KEY_PLAYPAUSE, KEY_FASTFORWARD }

Fire TV Stick 4K Max 2nd-Gen ("kcmouse", "Amazon Fire TV Remote") (original remote paired with Fire TV 4K Max 2nd-Gen)
*/