#pragma once
#ifndef POLLER_H
#define POLLER_H

#include <iostream>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <string.h>
#include <sys/inotify.h>
#include <unistd.h>

class Poller {
protected:
    const int fd_;
    struct pollfd pfd_;
public:
    enum class PollType { READ = POLLIN, WRITE = POLLOUT, READWRITE = (POLLIN | POLLOUT)};
    enum class Result { ERROR, TIMEOUT, READ, WRITE, READWRITE };
public:
    Poller(int fd, PollType ptype) : fd_(fd), pfd_({fd, (static_cast<uint8_t>(ptype)), 0}) { }
    virtual ~Poller() { }

    /*
        Monitor for events on a file descriptor.
        @param timeout_ms milliseconds to wait for event. -1 for infinite timeout.
        @return Poller::Result
    */
    Result monitor(int timeout_ms) {

        int rpoll = poll(&pfd_, 1, timeout_ms);
        //printf("Poller fd:%i  events: 0x%04x, revents: 0x%04x\n", fd_, pfd_.events, pfd_.revents);

        switch (rpoll)
        {
        case -1:
            //printf("Poller fd:%i monitor - errno:%i (%s)\n", fd_, errno, strerror(errno));
            return Result::ERROR;
        case 0:
            return Result::TIMEOUT;
        default:
            if (pfd_.revents & POLLIN && pfd_.revents & POLLOUT) {
                return Result::READWRITE;
            } else if (pfd_.revents & POLLIN) {
                return Result::READ;
            } else if (pfd_.revents & POLLOUT) {
                return Result::WRITE;
            } else if (pfd_.revents & POLLERR || pfd_.revents & POLLHUP || pfd_.revents & POLLNVAL) {
                return Result::ERROR;
            }
            return Result::ERROR;
        }
    }

};

class PollerINotify : public Poller {
public:
    PollerINotify(int fd, PollType ptype) : Poller(fd, ptype) {  }
    ~PollerINotify() {
        close(fd_);
    }

    static int create_fd(const char* dir_path) {
        int nfd = inotify_init1(IN_NONBLOCK);
        if (nfd != -1) { inotify_add_watch(nfd, dir_path, (IN_CREATE | IN_DELETE)); }
        return nfd;
    }

    /*
        Monitor for events on a file path.
        @param dir_path   - path to monitor for change events.
        @param timeout_ms - milliseconds to wait for event. -1 for infinite timeout.
        @return returns nothing
    */
    static Result wait_for(const char* dir_path, int timeout_ms) {
        auto res = Poller::Result::ERROR;
        int fd = PollerINotify::create_fd(dir_path);
        if (fd != -1) {
            PollerINotify poll { fd, Poller::PollType::READ };
            res = poll.monitor(timeout_ms);
            if (res == Poller::Result::READ) { poll.read_drain(); }
        }
        return res;
    }

    /*
        Monitor for events on a file path.
        @param timeout_ms - milliseconds to wait for event. -1 for infinite timeout.
        @return Poller::Result
    */
    Result wait_for(int timeout_ms) {
        auto res = monitor(timeout_ms);
        if (res == Poller::Result::READ) { read_drain(); }
        return res;
    }
    
    void read_drain() {
        char buffer[4*1024];
        while (read(fd_, buffer, sizeof(buffer)) > 0) { }
    }

};

#endif  /* POLLER_H */