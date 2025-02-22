#include <chrono>
#include <future>
#include <thread>
#include "MouseHandler.h"

//#define DEBUG printf
#define DEBUG(format, args...) ((void)0)

MouseHandler::MouseHandler(Device& mouse, SyncQueue<Button>& queue) : mouse_(mouse), queue_(queue) { 
    DEBUG("MouseHandler() (constructor)\n");
    start();
}

MouseHandler::~MouseHandler() {
    DEBUG("~MouseHandler() (deconstructor)\n");
    fd_close();
}

int MouseHandler::fd_open() {
    fd_ = open(mouse_.getPath().c_str(), O_RDWR);
    if (fd_ == -1) {
        DEBUG("MouseHandler %s - failed to open device: %s (%s)\n", __func__, mouse_.getPath().c_str(), strerror(errno));
    }
    return fd_;
}

void MouseHandler::fd_close() {
    if (fd_ != -1) {
        if (close(fd_) == 0) { fd_ = -1; }
    }
}

int MouseHandler::run() {

    if (fd_open() == -1) { return -1; }

    DEBUG("MouseHandler %s - reading from: %s (%s)\n", __func__, mouse_.getPath().c_str(), mouse_.getName().c_str());

    while (isStopRequested_ == false) {

        // check if file descriptor is still valid
        if (fcntl(fd_, F_GETFL) == -1) {
            DEBUG("MouseHandler %s - fd returned -1 so exiting - %s\n", __func__, strerror(errno));
            break;
        }

        if (!queue_.wait_for(5000)) {
            continue;
        }
        Button button = queue_.pop();

        auto key = button.getKey();
        auto state = button.getState();
        int move_count = 0;
        double speed = 1.0f;

        // mouse move
        if (state == Button::State::DOWN && (key == KEY_UP || key == KEY_DOWN)) {
            while(move_count < MOVE_COUNT_MAX && !queue_.isAvailable()) {
                doMouseMove(fd_, 0, (key == KEY_UP ? -speed : speed));
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
                if (speed < SPEED_MAX) {
                    speed *= SPEED_ACCEL;
                    if (speed > SPEED_MAX) { speed = SPEED_MAX; }
                }
                move_count++;
            }
        } else if (state == Button::State::DOWN && (key == KEY_LEFT || key == KEY_RIGHT)) {
            while(move_count < MOVE_COUNT_MAX && !queue_.isAvailable()) {
                doMouseMove(fd_, (key == KEY_LEFT ? -speed : speed), 0);
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
                if (speed < SPEED_MAX) {
                    speed *= SPEED_ACCEL;
                    if (speed > SPEED_MAX) { speed = SPEED_MAX; }
                }
                move_count++;
            }
        }

        // mouse click
        else if (state == Button::State::UP && (key == KEY_ENTER || key == KEY_KPENTER)) {
            doKeyEvent(fd_, BTN_MOUSE, KeyEventType::DOWNUP);
        }

        // mouse scroll
        else if (state == Button::State::DOWN && (key == KEY_REWIND || key == KEY_FASTFORWARD)) {
            while(move_count < SCROLL_COUNT_MAX && !queue_.isAvailable()) {
                doMouseWheel(fd_, (key == KEY_REWIND ? 1 : -1));
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
                move_count++;
            }
        }

        // mouse show on activate (for user visual indicator only)
        else if (key == 255) {
            doMouseMove(fd_, 1, 0);
        }

        // sleep
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    fd_close();
    return 0;
}

bool MouseHandler::doMouseMove(int fd, int x, int y) {
    input_event event;

    gettimeofday(&event.time, NULL);
    event.type  = EV_REL;
    event.code  = x != 0 ? REL_X : REL_Y;
    event.value = x != 0 ? x : y;

    // move mouse
     ssize_t rw1 = write(fd, &event, sizeof(input_event));

    // sync
    event.type  = EV_SYN;
    event.code  = SYN_REPORT;
    event.value = 0;
    ssize_t rw2 = write(fd, &event, sizeof(input_event));

    return (rw1 != -1 && rw2 != -1);
}

bool MouseHandler::doMouseWheel(int fd, int value) {
    input_event event1, event2, event_sync;

    gettimeofday(&event1.time, NULL);
    event1.type  = EV_REL;
    event1.code  = REL_WHEEL;
    event1.value = value;

    event2.time = event1.time;
    event2.type  = EV_REL;
    event2.code  = REL_WHEEL_HI_RES;
    event2.value = (value == -1) ? -120 : 120;

    event_sync.time = event1.time;
    event_sync.type  = EV_SYN;
    event_sync.code  = SYN_REPORT;
    event_sync.value = 0;

    // move mouse wheel, sync
    ssize_t rw1 = write(fd, &event1, sizeof(input_event));
    ssize_t rw2 = write(fd, &event2, sizeof(input_event));
    ssize_t rw3 = write(fd, &event_sync, sizeof(input_event));
    
    return (rw1 != -1 && rw2 != -1 && rw3 != -1);
}

bool MouseHandler::doKeyEvent(int fd, uint16_t keyCode, KeyEventType keyEventType) {
    input_event event, event_sync;
    ssize_t rw[] { 0, 0, 0, 0 };

    gettimeofday(&event.time, NULL);
    event.type = EV_KEY;
    event.code = keyCode;
    event.value = 0;

    event_sync.time = event.time;
    event_sync.type = EV_SYN;
    event_sync.code = SYN_REPORT;
    event_sync.value = 0;

    //send key down event, sync
    if (keyEventType == KeyEventType::DOWN || keyEventType == KeyEventType::DOWNUP) {
        event.value = 1;
        rw[0] = write(fd, &event, sizeof(input_event));
        rw[1] = write(fd, &event_sync, sizeof(input_event));
        //DEBUG("MouseHandler sending - event.type:%i .code:%i .value:%i time.sec:%li .usec:%li\n", event.type, event.code, event.value, event.time.tv_sec, event.time.tv_usec);
        //DEBUG("MouseHandler sending - event.type:%i .code:%i .value:%i time.sec:%li .usec:%li\n", event_sync.type, event_sync.code, event_sync.value, event_sync.time.tv_sec, event_sync.time.tv_usec);
    }

    // update event, event_sync time if multiple events are being sent
    if (keyEventType == KeyEventType::DOWNUP) {
        gettimeofday(&event.time, NULL);
        event_sync.time = event.time;
    }

    //send key up event, sync
    if (keyEventType == KeyEventType::UP || keyEventType == KeyEventType::DOWNUP) {
        event.value = 0;
        rw[2] = write(fd, &event, sizeof(input_event));
        rw[3] = write(fd, &event_sync, sizeof(input_event));
        //DEBUG("MouseHandler sending - event.type:%i .code:%i .value:%i time.sec:%li .usec:%li\n", event.type, event.code, event.value, event.time.tv_sec, event.time.tv_usec);
        //DEBUG("MouseHandler sending - event.type:%i .code:%i .value:%i time.sec:%li .usec:%li\n", event_sync.type, event_sync.code, event_sync.value, event_sync.time.tv_sec, event_sync.time.tv_usec);
    }

    return (rw[0] != -1 && rw[1] != -1 && rw[2] != -1 && rw[3] != -1);
}