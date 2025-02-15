#pragma once
#ifndef MOUSE_HANDLER_H
#define MOUSE_HANDLER_H

#include <iostream>
#include <cstring>
#include <future>
#include <thread>
#include <queue>
#include <fcntl.h>
#include <linux/input.h>
#include <sys/stat.h>
#include <unistd.h>
#include "Device.h"
#include "RemoteHandler.h"
#include "SyncQueue.h"
#include "ThreadedObject.h"

class MouseHandler : public ThreadedObject {
public:
	MouseHandler(Device& mouse, SyncQueue<Button>& queue);
	~MouseHandler();
	int run() override;
private:
	enum class KeyEventType { DOWN, UP, DOWNUP };
	int fd_open();
	void fd_close();
	bool doMouseMove(int fd, int x, int y);
	bool doMouseWheel(int fd, int value);
	bool doKeyEvent(int fd, uint16_t keyCode, KeyEventType keyEventType);
private:
	Device mouse_;
	SyncQueue<Button>& queue_;
	int fd_ {-1};
private:
	static constexpr double SPEED_MAX = 5.0f;
	static constexpr double SPEED_ACCEL = 1.01f;
	static constexpr int MOVE_COUNT_MAX = 512;
	static constexpr int SCROLL_COUNT_MAX = 32;
};

#endif  /* MOUSE_HANDLER_H */