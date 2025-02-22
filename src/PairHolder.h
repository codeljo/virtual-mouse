#pragma once
#ifndef PAIR_HOLDER_H
#define PAIR_HOLDER_H

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
#include "MouseHandler.h"
#include "RemoteHandler.h"
#include "SyncQueue.h"

class PairHolder {
public:
    enum class Status { RUN, STOPPING, STOPPED };
public:
    PairHolder(Device& remote, Device& mouse);
    ~PairHolder();
    void stop();
    Status getStatus() const;
    Device getRemote();
    Device getMouse();
    friend std::ostream& operator<<(std::ostream& os, const PairHolder& obj);
private:
    Device remote_;
    Device mouse_;
    RemoteHandler remoteHandler_;
    MouseHandler mouseHandler_;
    SyncQueue<Button> queue_;
};

#endif  /* PAIR_HOLDER_H */