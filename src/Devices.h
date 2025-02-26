#pragma once
#ifndef DEVICES_H
#define DEVICES_H

#include <iostream>
#include <cstring>
#include <filesystem>
#include <vector>
#include <fcntl.h>
#include <linux/input.h>
#include <sys/stat.h>
#include <unistd.h>
#include "Device.h"
#include "Files.h"

class Devices {
public:
    Devices() = delete;
    ~Devices() = delete;
    enum class KeyEventType { DOWN, UP, DOWNUP };
    enum class SearchType { EXACT, CONTAINS };
    enum class DetailType { ALL, NAME };
    static Device getByName(const char* search, SearchType searchType);
    static std::vector<Device> getByName2(const char* search, SearchType searchType);
    static std::vector<Device> getWithKeysRelsSupported(const std::vector<int>& keys, const std::vector<int>& rels);
    static bool showDevice(const char* device, DetailType detailType);
    static bool showDevices(DetailType detailType);
    static bool isKeySupported(int fd, int key);
    static bool isRelSupported(int fd, int rel);
    static const inline char* PATH_DEV_INPUT { "/dev/input/" };
};

#endif  /* DEVICES_H */