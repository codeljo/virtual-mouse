#pragma once
#ifndef SYNC_QUEUE_H
#define SYNC_QUEUE_H

#include <cassert>
#include <future>
#include <thread>
#include <queue>

template <class T>
class SyncQueue {
private:
    std::queue<T> queue;
    std::mutex mutex;
    std::condition_variable cv;
public:
    SyncQueue() { };

    void push(const T& item) {
        {
            std::lock_guard<std::mutex> guard(mutex);
            queue.push(item);
        }
        cv.notify_one();
    }
    
    T pop() {
        std::lock_guard<std::mutex> guard(mutex);
        assert(!queue.empty());
        auto val = queue.front();
        queue.pop();
        return val;
    }

    bool isAvailable() {
        std::lock_guard<std::mutex> guard(mutex);
        return !queue.empty();
    }
    
    /* returns true if the queue has been notified of a push, or if the queue is not empty */
    bool wait_for(int64_t ms) {
        std::unique_lock<std::mutex> ulock(mutex);
        if (!queue.empty()) { return true; }
        auto status = cv.wait_for(ulock, std::chrono::duration(std::chrono::milliseconds(ms)));
        return status == std::cv_status::no_timeout;
    }

};

#endif  /* SYNC_QUEUE_H */