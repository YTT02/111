//
// Created by LENOVO on 2024/6/18.
//

#ifndef HELMET_THREADSAFESTRING_H
#define HELMET_THREADSAFESTRING_H
#include <iostream>
#include <string>
#include <mutex>
#include <thread>

class ThreadSafeString {
public:
    void setString(const std::string& str) {
        std::lock_guard<std::mutex> lock(mutex_);
        string_ = str;
    }

    std::string getString() {
        std::lock_guard<std::mutex> lock(mutex_);
        return string_;
    }

private:
    std::string string_;
    std::mutex mutex_;
};


#endif //HELMET_THREADSAFESTRING_H
