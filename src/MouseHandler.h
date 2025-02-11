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

class MouseHandler {
public:
	MouseHandler(Device& mouse, SyncQueue<Button>& queue);
	~MouseHandler();
	void start();
	void stop();
	bool isRunning() const;
private:
	enum class KeyEventType { DOWN, UP, DOWNUP };
	int open_fd();
	void close_fd();
	int doLoop();
	bool doMouseMove(int fd, int x, int y);
	bool doMouseWheel(int fd, int value);
	bool doKeyEvent(int fd, uint16_t keyCode, KeyEventType keyEventType);
private:
	Device mouse_;
	SyncQueue<Button>& queue_;
	std::future<int> future_;
	std::atomic<bool> isStopRequested_ = false;
	int fd_ {-1};
private:
	static constexpr double SPEED_MAX = 5.0f;
	static constexpr double SPEED_ACCEL = 1.01f;
	static constexpr int MOVE_COUNT_MAX = 512;
	static constexpr int SCROLL_COUNT_MAX = 32;
};

#endif  /* MOUSE_HANDLER_H */