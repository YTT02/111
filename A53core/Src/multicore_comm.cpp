//
// Created by JaimeVan on 2023/7/19.
//
#include "RpmsgClient.h"
#include "multicore_comm.h"
#include "iostream"
#include "opencv_pushstream.h"
#include "thread"
#include <ctime>
#include <iomanip>
#include <utility>
#include "atomic"
#include "main.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <csignal>

#define SIGNAL_SOS 0x1111
#define SIGNAL_PUSHING 0x0102
#define SIGNAL_CHEESE 0x0101
#define SIGNAL_RECORD 0x0100
#define SIGNAL_ADD 0x0200
#define SIGNAL_MINUS 0x0201

std::atomic<bool> is_keep_pushing(false);
std::atomic<bool> is_keep_recording(false);
pid_t child_pid = -1;
std::string command2execute = "gst-launch-1.0 alsasrc ! volume volume=4.0 ! audioconvert ! queue ! avenc_aac bitrate=32000 ! queue ! rtspclientsink location=rtsp://118.195.217.196:8854/helmet_audio_stream protocols=tcp";

void executeShellCommand(const std::string& command) {
    child_pid = fork();
    if (child_pid == 0) {
        // 子进程
        execl("/bin/sh", "sh", "-c", command.c_str(), (char *)0);
        // 如果 exec 失败
        std::cerr << "Exec failed\n";
        exit(1);
    } else if (child_pid > 0) {
        // 父进程
        std::cout << "Started process with PID: " << child_pid << std::endl;
        // 等待子进程完成（如果不需要等待，可以移除这行）
        waitpid(child_pid, nullptr, 0);
    } else {
        // fork 失败
        std::cerr << "Fork failed\n";
    }
}

// 终止子进程的函数
void terminateChildProcess() {
    if (child_pid > 0) {
        if (kill(child_pid, SIGTERM) == 0) {
            std::cout << "Terminated process with PID: " << child_pid << std::endl;
        } else {
            std::cerr << "Failed to terminate process with PID: " << child_pid << std::endl;
        }
    } else {
        std::cerr << "No child process to terminate\n";
    }
}


void MessageFromM4Process(const std::vector<u_char>& message){
    static std::thread execThread;
    static thread pushing_stream_thread, recording_thread;
    static std::uint8_t running_status = DEFAULT_STATUS;
    if ((message[0] == 0x11) && (message[1] == 0x45) && (message[4] == 0x14)){
        uint16_t ctl_signal = (message[2] << 8) + message[3];
        std::cout << "get ctl_signal " << ctl_signal << std::endl;
        switch (ctl_signal) {
            case SIGNAL_SOS:
                std::cout << "SOS signal get!" << std::endl;
                break;
            case SIGNAL_RECORD:
                std::cout << "launch video recording!" << std::endl;
                is_keep_recording = true;
                recording_thread = thread(OpencvRecordVideo);
                running_status = VIDEO_RECORDING;

                send_message_flag = true;
                Acore_status_now.setString("RECORDING");

                break;
            case SIGNAL_CHEESE:
                if (running_status == PUSHING_STREAM){
                    is_keep_pushing = false;
                    std::cout << "preparing end stream" << std::endl;
                    terminateChildProcess();
                    if (execThread.joinable()){
                        execThread.join();
                    }
                    if (pushing_stream_thread.joinable()){
                        pushing_stream_thread.join();
                    }
                    std::cout << "push stream end" << std::endl;

                    send_message_flag = true;
                    Acore_status_now.setString("DEFAULT");

                    running_status = DEFAULT_STATUS;
                } else if (running_status == VIDEO_RECORDING){
                    is_keep_recording = false;
                    if (recording_thread.joinable()){
                        recording_thread.join();
                    }
                    std::cout << "video saved" << std::endl;

                    send_message_flag = true;
                    Acore_status_now.setString("DEFAULT");

                    running_status = DEFAULT_STATUS;
                } else{
                    std::cout << "cheesing" << std::endl;
                    std::cout << "waiting for cheese end" << std::endl;

                    thread photo_taking_thread = std::thread(OpencvTakePhoto);
                    if (photo_taking_thread.joinable()){
                        photo_taking_thread.join();
                    }
                    std::cout << "photo saved!" << std::endl;
                }
                break;
            case SIGNAL_PUSHING:
                is_keep_pushing = true;
                std::cout << "launch video call" << std::endl;
                std::cout << "preparing to push stream" << std::endl;
                execThread = std::thread(executeShellCommand, command2execute);
                pushing_stream_thread = std::thread(OpencvPushStream);
                running_status = PUSHING_STREAM;

                send_message_flag = true;
                Acore_status_now.setString("PUSHING");

                break;
            case SIGNAL_ADD:
                std::cout << "volume add" << std::endl;
                break;
            case SIGNAL_MINUS:
                std::cout << "volume minus" << std::endl;
                break;
            default:
                std::cout << "unknown signal" << std::endl;
                break;
        }
    }
}