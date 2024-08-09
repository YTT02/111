#include <iostream>
#include "RpmsgClient.h"
#include "SerialPort.hpp"
#include "thread"
#include "main.h"
#include "multicore_comm.h"
#include "RpmsgClient.h"
#include "atomic"
#include "ThreadSafeString.h"

using namespace std;

using namespace std::chrono_literals;

std::atomic<bool> send_message_flag(false);
ThreadSafeString Acore_status_now;

int get_sleep(){
    std::string message_send2rpmsg = "OFFLINE";
    Acore_status_now.setString(message_send2rpmsg);
    send_message_flag = true;
    std::this_thread::sleep_for(5s);
    system("/home/root/sleep.sh");
    return 1;
}

int main() {
    static int running_cnt;
    unsigned int n = std::thread::hardware_concurrency();
    Acore_status_now.setString("DEFAULT");
    std::cout << n << " concurrent threads are supported." << std::endl;

    static int no_other_thread_count;
    static bool sleep_flag = false;
//    std::thread rpmsg_thread(rpmsg_client);
//    rpmsg_thread.detach();
    std::cout << "main thread is running rpmsg client!" << std::endl;

    n = std::thread::hardware_concurrency();
    std::cout << n << " concurrent threads are supported." << std::endl;

    RpmsgClient my_rpmsg_client;
    my_rpmsg_client.ClientInit("/dev/ttyRPMSG30", mn::CppLinuxSerial::BaudRate::B_115200, MessageFromM4Process);
    std::cout << "Rpmsg client keep running..." << std::endl;
    while (1){
        my_rpmsg_client.ClientReceiveMessage();
        if (running_cnt == 100){
//            std::string message2send = Acore_status_now.getString();
//            my_rpmsg_client.ClientSendString(message2send);
//            message2send.clear();
            running_cnt = 0;
        } else{
            if (send_message_flag){
                std::string message2send = Acore_status_now.getString();
                my_rpmsg_client.ClientSendString(message2send);
                message2send.clear();
                send_message_flag = false;
            }
            running_cnt ++;
        }

        std::this_thread::sleep_for(10ms);
    }
}
