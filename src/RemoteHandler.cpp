#include <chrono>
#include <future>
#include <thread>
#include "EventDesc.h"
#include "Poller.h"
#include "RemoteHandler.h"

//#define DEBUG printf
#define DEBUG(format, args...) ((void)0)

RemoteHandler::RemoteHandler(Device& remote, SyncQueue<Button>& queue) : remote_(remote), queue_(queue) {
	DEBUG("RemoteHandler() (constructor)\n");
	start();
}

RemoteHandler::~RemoteHandler() {
	DEBUG("~RemoteHandler() (deconstructor)\n");
	close_fd();
}

void RemoteHandler::start() {
	future_ = std::async(std::launch::async, &RemoteHandler::doLoop, this);
}

void RemoteHandler::stop() {
	isStopRequested_ = true;
}

bool RemoteHandler::isRunning() const {
	return (this->future_.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready);
}

int RemoteHandler::open_fd() {
	fd_ = open(remote_.getPathPtr(), O_RDONLY);
	if (fd_ == -1) {
		DEBUG("RemoteHandler %s - failed to open device: %s (%s)\n", __func__, remote.getPathPtr(), strerror(errno));
	}
	return fd_;
}

void RemoteHandler::close_fd() {
	if (fd_ != -1) {
		ioctl(fd_, EVIOCGRAB, 0);
		if (close(fd_) == 0) { fd_ = -1; }
	}
}

int RemoteHandler::doLoop() {

	if (open_fd() == -1) { return -1; }

	DEBUG("RemoteHandler %s - reading from: %s (%s)\n", __func__, remote.getPathPtr(), remote.getNamePtr());

	bool isMouse = false;

	input_event events[READ_EVENTS_MAX];
	Button button { KEY_PLAYPAUSE };
	Poller poller { fd_, Poller::PollType::READ };

	while (isStopRequested_ == false) {

		Poller::Result rpoll = poller.monitor(5000);
		if (rpoll == Poller::Result::ERROR) { break; }
		if (rpoll == Poller::Result::TIMEOUT) { continue; }
		if (rpoll == Poller::Result::READ) { }

		ssize_t rread = read(fd_, events, sizeof(input_event) * READ_EVENTS_MAX);
		if (rread == -1 || rread == 0 || (rread < (ssize_t)sizeof(input_event))) { break; }

		auto event_count = (rread / (ssize_t)sizeof(input_event));

		for (ssize_t i=0; i < event_count; i++) {
			auto type  = events[i].type;
			auto code  = events[i].code;
			auto value = events[i].value;

			//const char* typedesc = getEventTypeDescription(type);
			//const char* codedesc = getEventCodeDescription(type, code);
			//DEBUG("RemoteHandler %s - event[%i] type=%i code=%i value=%i\n", __func__, i, type, code, value);
			//DEBUG("RemoteHandler %s - event[%i] type:0x%02x(%s) code:0x%04x(%s) value:0x%08x %i\n", __func__, i, type, typedesc, code, codedesc, value, value);
			//DEBUG("RemoteHandler %s - event[%i] type:0x%02x(%s) code:0x%04x(%s) value:0x%08x %i time.sec:%li .usec:%li\n", __func__, i, type, typedesc, code, codedesc, value, value, events[i].time.tv_sec, events[i].time.tv_usec);

			// handle KEY_PLAYPAUSE(activate, deactivate), KEY_HOMEPAGE(deactivate)
			if (type == EV_KEY) {
				switch (code) {
					case KEY_PLAYPAUSE:
						if (value == 0) {
							button.setState(Button::State::UP);
							auto diff = button.getTimeDiffMs(Button::State::UP);
							if (diff > 0 && diff < 500) {
								if (ioctl(fd_, EVIOCGRAB, !isMouse) != -1) { isMouse = !isMouse; }
								if (isMouse) { queue_.push(Button(255)); }
							}
							DEBUG("RemoteHandler %s - KEY_PLAYPAUSE diff:%lf isMouse:%s\n", __func__, diff, (isMouse ? "true" : "false"));
						} else if (value == 1) {
							button.setState(Button::State::DOWN);
						}
						break;
					case KEY_ESC:
					case KEY_HOMEPAGE:
						if (ioctl(fd_, EVIOCGRAB, 0) != -1) { isMouse = false; }
						DEBUG("RemoteHandler %s - isMouse:%s\n", __func__, (isMouse ? "true" : "false"));
						break;
					case KEY_UP:
					case KEY_DOWN:
					case KEY_LEFT:
					case KEY_RIGHT:
					case KEY_ENTER:
					case KEY_KPENTER:
					case KEY_BACK:
					case KEY_MENU:
					case KEY_REWIND:
					case KEY_FASTFORWARD:
						if (isMouse && (value == 0 || value == 1)) {
							queue_.push(Button(code, value == 0 ? Button::State::UP : Button::State::DOWN));
						}
						break;
					default:
						break;
				}
			}
		}
	}

	close_fd();
	return 0;
}
