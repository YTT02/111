//
// Created by JaimeVan on 2023/7/4.
//
#include "SerialPort.hpp"
#include "RpmsgClient.h"

using namespace mn::CppLinuxSerial;

//void RpmsgIsrCallback()
RpmsgClient::RpmsgClient() {
    init_flag_ = false;
    release_flag_ = false;
    alive_flag_ = false;
    RpmsgGetcharCallback_ = nullptr;
}

RpmsgClient::~RpmsgClient() {
    if (!release_flag_){
        Release();
    }
}

void RpmsgClient::ClientInit(const std::string &tty_name,
                             mn::CppLinuxSerial::BaudRate baud,
                             RpmsgCallBack func) {
    my_rpmsg_serial_.SetDevice(tty_name);
    my_rpmsg_serial_.SetBaudRate(baud);
    my_rpmsg_serial_.SetTimeout(100);
    my_rpmsg_serial_.Open();
    if (func != nullptr){
        RpmsgGetcharCallback_ = func;
        std::cout << "callback func get init" << std::endl;
    }
    std::cout << "Linux Client On Line" << std::endl;

    init_flag_ = true;
}

void RpmsgClient::Release() {
    if (!release_flag_){
        my_rpmsg_serial_.Close();
        std::destroy(receive_buffer_.begin(), receive_buffer_.end());

        release_flag_ = true;
        std::cout << "Linux Rpmsg Client Off Line" << std::endl;
    }
}

void RpmsgClient::ClientReceiveMessage() {
    my_rpmsg_serial_.ReadBinary(receive_buffer_);
    if (!receive_buffer_.empty()){
        if (RpmsgGetcharCallback_){
            RpmsgGetcharCallback_(receive_buffer_);
        }
        for (auto data:receive_buffer_) {
            std::cout << std::hex << int(data) ;
        }
        std::cout << std::endl;
        receive_buffer_.clear();
    }
}

void RpmsgClient::ClientSendString(const std::string& message){
    my_rpmsg_serial_.Write(message);
}
