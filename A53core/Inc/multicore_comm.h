//
// Created by JaimeVan on 2023/7/19.
//

#ifndef HELMET_MULTICORE_COMM_H
#define HELMET_MULTICORE_COMM_H
#include "RpmsgClient.h"
#include "atomic"

#define DEFAULT_STATUS 0x00
#define PUSHING_STREAM 0x01
#define VIDEO_RECORDING 0x02


void MessageFromM4Process(const std::vector<u_char>& message);

extern std::atomic<bool> is_keep_pushing;
extern std::atomic<bool> is_keep_recording;

#endif //HELMET_MULTICORE_COMM_H
