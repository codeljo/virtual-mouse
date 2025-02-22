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
#include "ThreadedObject.h"

class RemoteHandler : public ThreadedObject<int> {
public:
    RemoteHandler(Device& remote, SyncQueue<Button>& queue);
    ~RemoteHandler();
private:
    int run() override;
    int fd_open();
    void fd_close();
private:
    Device remote_;
    SyncQueue<Button>& queue_;
    int fd_ {-1};
    static constexpr int READ_EVENTS_MAX = 64;
};

#endif  /* REMOTE_HANDLER_H */