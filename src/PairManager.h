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
	std::vector<std::unique_ptr<PairHolder>> holders;
public:
	PairManager() { }

	void add(PairHolder * holder) {
		holders.push_back(std::unique_ptr<PairHolder>(holder));
	}

	bool exists(const Device& remote) {
		for (const auto& holder : holders) {
			if (remote.equals(holder->getRemote())) { return true; }
		}
		return false;
	}
	
	/* remove all PairHolders that are no longer running */
	void cleanup() {
		for (const auto& holder : holders) {
			if (holder->getStatus() == PairHolder::Status::STOPPING) { holder->stop(); }
		}
		std::erase_if(holders, [](std::unique_ptr<PairHolder>& ph){ return ph->getStatus() == PairHolder::Status::STOPPED; });
	}

	void loop() {

		PollerINotify poll(PollerINotify::create_fd(Devices::PATH_DEV_INPUT), Poller::PollType::READ);

		while (true) {

			// show all devices
			//Devices::showDevices(Devices::DetailType::NAME);

			// mouse
			std::vector<Device> mouses = Devices::getWithKeysRelsSupported({ BTN_MOUSE }, { REL_X, REL_Y, REL_WHEEL });

			// remote
			std::vector<int> remote_keys { KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_ENTER, KEY_KPENTER, KEY_BACK, KEY_HOMEPAGE, KEY_MENU, KEY_REWIND, KEY_PLAYPAUSE, KEY_FASTFORWARD };
			std::vector<Device> remotes = Devices::getWithKeysRelsSupported(remote_keys, {});

			// verify mouse, remote are both found
			if (mouses.empty() || remotes.empty()) {
				poll.wait_for(30000);
				continue;
			}
			
			// mouse
			Device mouse = mouses.front();

			// cleanup
			cleanup();

			// add new pair
			for (auto& remote : remotes)
			{
				if (mouse.getPath() == remote.getPath()) { continue; }
				if (!exists(remote)) {
					add(new PairHolder(remote, mouse));
				}
			}

			// print details for current pairs
			print();
			
			poll.wait_for(30000);
		}

	}

	void print() {
		for (const auto& holder : holders) {
			std::cout << *holder;
		}
	}

};

#endif  /* PAIR_MANAGER_H */
