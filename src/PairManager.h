#pragma once
#ifndef PAIR_MANAGER_H
#define PAIR_MANAGER_H

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>
#include "Device.h"
#include "Devices.h"
#include "PairHolder.h"
#include "Poller.h"

class PairManager {
private:
    std::vector<std::unique_ptr<PairHolder>> holders_;
    const std::vector<int> remote_keys_ { KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_ENTER, KEY_KPENTER, KEY_BACK, KEY_HOMEPAGE, KEY_MENU, KEY_REWIND, KEY_PLAYPAUSE, KEY_FASTFORWARD };
    const std::vector<int> mouse_keys_ { BTN_MOUSE };
    const std::vector<int> mouse_rels_ { REL_X, REL_Y, REL_WHEEL };
    const int POLL_TIMEOUT_MS {30000};
public:
    PairManager() { }

    void add(PairHolder * holder) {
        holders_.push_back(std::unique_ptr<PairHolder>(holder));
    }

    bool exists(const Device& remote) {
        for (const auto& holder : holders_) {
            if (remote.equals(holder->getRemote())) { return true; }
        }
        return false;
    }

    void cleanup() {
        for (const auto& holder : holders_) {
            if (holder->getStatus() == PairHolder::Status::STOPPING) { holder->stop(); }
        }
        std::erase_if(holders_, [](std::unique_ptr<PairHolder>& ph){ return ph->getStatus() == PairHolder::Status::STOPPED; });
    }

    void loop() {

        PollerINotify poll(PollerINotify::create_fd(Devices::PATH_DEV_INPUT), Poller::PollType::READ);

        while (true) {

            std::vector<Device> remotes = Devices::getWithKeysRelsSupported(remote_keys_, {});
            std::vector<Device> mouses = Devices::getWithKeysRelsSupported(mouse_keys_, mouse_rels_);
            if (remotes.empty() || mouses.empty()) {
                poll.wait_for(POLL_TIMEOUT_MS);
                continue;
            }

            cleanup();

            Device mouse = mouses.front();
            for (auto& remote : remotes) {
                if (remote.equals(mouse)) { continue; }
                if (!exists(remote)) {
                    add(new PairHolder(remote, mouse));
                }
            }

            print();
            poll.wait_for(POLL_TIMEOUT_MS);
        }

    }

    void print() {
        for (const auto& holder : holders_) {
            std::cout << *holder;
        }
    }

};

#endif  /* PAIR_MANAGER_H */