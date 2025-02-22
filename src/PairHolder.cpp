#include <chrono>
#include <future>
#include <thread>
#include "PairHolder.h"

//#define DEBUG printf
#define DEBUG(format, args...) ((void)0)

PairHolder::PairHolder(const Device& remote, const Device& mouse) : remote_(remote), mouse_(mouse), remoteHandler_(remote, queue_), mouseHandler_(mouse, queue_) { 
    DEBUG("PairHandler() (constructor)\n");
}

PairHolder::~PairHolder() {
    DEBUG("~PairHandler() (deconstructor)\n");
    stop();
}

void PairHolder::stop() {
    remoteHandler_.stop();
    mouseHandler_.stop();
}

PairHolder::Status PairHolder::getStatus() const {
    auto rh = remoteHandler_.isRunning();
    auto mh = mouseHandler_.isRunning();
    if (rh && mh) { return Status::RUN; }
    if (!rh && !mh) { return Status::STOPPED; }
    return Status::STOPPING;
}

Device PairHolder::getRemote() {
    return remote_;
}

Device PairHolder::getMouse() {
    return mouse_;
}

std::ostream& operator<<(std::ostream& os, const PairHolder& obj) {
    os << "PairHolder (remote name:" << obj.remote_.getName() << " path:" << obj.remote_.getPath() << ") (mouse name:" << obj.mouse_.getName() << " path:" << obj.mouse_.getPath() << ")" << "\n";
    return os;
}