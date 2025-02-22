#pragma once
#ifndef MOUSE_VIRTUAL_H
#define MOUSE_VIRTUAL_H

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <linux/uinput.h>
#include <cstring>
#include <errno.h>

class MouseVirtual {
private:
    const char* name_;
    int fd_;
public:
    MouseVirtual(const char* name="Virtual Mouse") : name_(name) {
        init();
    }

    ~MouseVirtual() {
        if (fd_ >= 0) {
            ioctl(fd_, UI_DEV_DESTROY);
            close(fd_);
        }
    }

    void init() {

        // open uinput device
        fd_ = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
        if (fd_ < 0) {
            perror("Failed to open /dev/uinput");
        }

        // enable mouse events
        if (ioctl(fd_, UI_SET_EVBIT, EV_KEY) < 0) { perror("Failed to set EV_KEY"); }
        if (ioctl(fd_, UI_SET_KEYBIT, BTN_LEFT) < 0) { perror("Failed to set BTN_LEFT"); }
        if (ioctl(fd_, UI_SET_KEYBIT, BTN_RIGHT) < 0) { perror("Failed to set BTN_RIGHT"); }
        if (ioctl(fd_, UI_SET_EVBIT, EV_REL) < 0) { perror("Failed to set EV_REL"); }
        if (ioctl(fd_, UI_SET_RELBIT, REL_X) < 0) { perror("Failed to set REL_X"); }
        if (ioctl(fd_, UI_SET_RELBIT, REL_Y) < 0) { perror("Failed to set REL_Y"); }
        if (ioctl(fd_, UI_SET_RELBIT, REL_WHEEL) < 0) { perror("Failed to set REL_WHEEL"); }

        // create the device
        struct uinput_user_dev uidev;
        memset(&uidev, 0, sizeof(uidev));
        strncpy(uidev.name, name_, UINPUT_MAX_NAME_SIZE-1);
        uidev.id.bustype = BUS_USB;
        uidev.id.vendor = 0x1234;
        uidev.id.product = 0x5678;
        uidev.id.version = 1;

        if (write(fd_, &uidev, sizeof(uidev)) < 0) {
            perror("Failed to write uinput_user_dev");
        }

        if (ioctl(fd_, UI_DEV_CREATE) < 0) {
            perror("Failed to create uinput device");
        }
    }

    void move(int x, int y) {
        struct input_event ev;

        memset(&ev, 0, sizeof(ev));
        ev.type = EV_REL;
        ev.code = REL_X;
        ev.value = x;
        write(fd_, &ev, sizeof(ev));

        memset(&ev, 0, sizeof(ev));
        ev.type = EV_REL;
        ev.code = REL_Y;
        ev.value = y;
        write(fd_, &ev, sizeof(ev));

        memset(&ev, 0, sizeof(ev));
        ev.type = EV_SYN;
        ev.code = SYN_REPORT;
        ev.value = 0;
        write(fd_, &ev, sizeof(ev));
    }

    void click(int button) {
        struct input_event ev;

        memset(&ev, 0, sizeof(ev));
        ev.type = EV_KEY;
        ev.code = button;
        ev.value = 1;
        write(fd_, &ev, sizeof(ev));

        memset(&ev, 0, sizeof(ev));
        ev.type = EV_KEY;
        ev.code = button;
        ev.value = 0;
        write(fd_, &ev, sizeof(ev));

        memset(&ev, 0, sizeof(ev));
        ev.type = EV_SYN;
        ev.code = SYN_REPORT;
        ev.value = 0;
        write(fd_, &ev, sizeof(ev));
    }

};

#endif  /* MOUSE_VIRTUAL_H */