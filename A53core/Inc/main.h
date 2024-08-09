//
// Created by JaimeVan on 2023/7/23.
//

#ifndef HELMET_MAIN_H
#define HELMET_MAIN_H
#include "RpmsgClient.h"
#include "mutex"
#include <iostream>
#include "RpmsgClient.h"
#include "SerialPort.hpp"
#include "thread"
#include "atomic"
#include "ThreadSafeString.h"

extern std::atomic<bool> send_message_flag;
extern ThreadSafeString Acore_status_now;

#endif //HELMET_MAIN_H
