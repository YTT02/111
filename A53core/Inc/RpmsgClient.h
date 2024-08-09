//
// Created by JaimeVan on 2023/7/7.
//

#ifndef HELMET_RPMSGCLIENT_H
#define HELMET_RPMSGCLIENT_H
#include "SerialPort.hpp"

typedef void(*RpmsgCallBack)(const std::vector<u_char>& message);

class RpmsgClient{
public:
    RpmsgClient();
    virtual ~RpmsgClient();

    void ClientInit(const std::string& tty_name, mn::CppLinuxSerial::BaudRate baud ,RpmsgCallBack func);
    void ClientReceiveMessage();
    void ClientSendString(const std::string& message);

    void Release();
private:
    bool init_flag_;
    bool release_flag_;
    bool alive_flag_;
    std::vector<u_char> receive_buffer_;
    mn::CppLinuxSerial::SerialPort my_rpmsg_serial_;
    RpmsgCallBack RpmsgGetcharCallback_;
};

#endif //HELMET_RPMSGCLIENT_H
