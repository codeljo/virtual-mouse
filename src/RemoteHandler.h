#pragma once
#ifndef REMOTE_HANDLER_H
#define REMOTE_HANDLER_H

#include <iostream>
#include <cstring>
#include <future>
#include <thread>
#include <queue>
#include <fcntl.h>
#include <linux/input.h>
#include <sys/stat.h>
#include <unistd.h>
#include "Button.h"
#include "Device.h"
#include "SyncQueue.h"

class RemoteHandler {
public:
	RemoteHandler(Device& remote, SyncQueue<Button>& queue);
	~RemoteHandler();
	void start();
	void stop();
	bool isRunning() const;
private:
	int open_fd();
	void close_fd();
	int doLoop();
private:
	Device remote_;
	SyncQueue<Button>& queue_;
	std::future<int> future_;
	std::atomic<bool> isStopRequested_ = false;
	int fd_ {-1};
private:
	static constexpr int READ_EVENTS_MAX = 64;
};

#endif  /* REMOTE_HANDLER_H */