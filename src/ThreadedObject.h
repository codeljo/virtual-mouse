#pragma once
#ifndef THREADED_OBJECT_H
#define THREADED_OBJECT_H

#include <future>

template <class T>
class ThreadedObject {
public:
	virtual T run() = 0;
	virtual void start() {
		future_ = std::async(std::launch::async, &ThreadedObject::run, this);
	}
	virtual void stop() {
		isStopRequested_ = true;
	}
	virtual T join() {
		return future_.get();
	}
	virtual bool isRunning() const {
		return future_.wait_for(std::chrono::seconds(0)) == std::future_status::timeout;
	}
	virtual ~ThreadedObject() {
		stop();
	}
protected:
	std::future<T> future_;
	std::atomic<bool> isStopRequested_ {false};
};

#endif  /* THREADED_OBJECT_H */