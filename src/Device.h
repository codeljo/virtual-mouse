#pragma once
#ifndef DEVICE_H
#define DEVICE_H

#include <iostream>

class Device {
private:
    const std::string name_;
    const std::string path_;
public:
    Device(const std::string& name, const std::string& path) : name_(name), path_(path) { }
    Device(const char * name, const char * path) : name_(name), path_(path) { }

    std::string getName() const {
        return name_;
    }

    std::string getPath() const {
        return path_;
    }

    bool equals(const Device& other) const {
        return path_ == other.path_;
    }

    bool isValid() const {
        return (name_.length() != 0 && path_.length() != 0);
    }

    friend std::ostream& operator<<(std::ostream& os, const Device& obj) {
        os << "Device name:" << obj.name_ << " path:" << obj.path_ << "\n";
        return os;
    }

};

#endif  /* DEVICE_H */