//
// Created by JaimeVan on 2024/6/10.
//
#include "utils.h"
#include "iostream"
#include <iomanip>
#include <ctime>


std::string getCurrentTimestamp() {
    std::time_t t = std::time(nullptr);
    std::tm* tmPtr = std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(tmPtr, "%y%d%m_%H%M");

    return oss.str();
}