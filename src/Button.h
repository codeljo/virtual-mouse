#pragma once
#ifndef BUTTON_H
#define BUTTON_H

#include <linux/input.h>
#include <time.h>

class Button {
public:
    enum class State { UP, DOWN };
private:
    const uint16_t key_;
    enum State state_;
    struct timespec times_down[2]{};
    struct timespec times_up[2]{};
public:
    Button(uint16_t key, State state = State::UP) : key_(key), state_(state) { }

    uint16_t getKey() const {
        return key_;
    }

    State getState() const {
        return state_;
    }

    void setState(State state) {
        state_ = state;
        switch (state) {
            case State::UP:
                times_up[0] = times_up[1];
                clock_gettime(CLOCK_REALTIME, &times_up[1]);
                break;
            case State::DOWN:
                times_down[0] = times_down[1];
                clock_gettime(CLOCK_REALTIME, &times_down[1]);
                break;
        }
        //std::cout << *this;
    }

    /* return ms between last 2 button states */
    int64_t getTimeDiffMs(State state) const {
        int64_t ms = 0;
        switch (state) {
            case State::UP:
                ms = time_diff_ms(times_up[0], times_up[1]);
                break;
            case State::DOWN:
                ms = time_diff_ms(times_down[0], times_down[1]);
                break;
        }
        return ms;
    }

    int64_t time_diff_ms(const struct timespec& start, const struct timespec& end) const {
        int64_t seconds = end.tv_sec - start.tv_sec;
        int64_t nanoseconds = end.tv_nsec - start.tv_nsec;
        if (nanoseconds < 0) {
            seconds--;
            nanoseconds += 1000000000;
        }
        return (seconds * 1000) + (nanoseconds / 1000000);
    }

    friend std::ostream& operator<<(std::ostream& os, const Button& obj) {
        os << "Button key:" << obj.key_ << " state:" << int(obj.state_) << " ms:" << obj.getTimeDiffMs(Button::State::UP) << "\n";
        return os;
    }

};

#endif  /* BUTTON_H */