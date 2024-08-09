//
// Created by JaimeVan on 2023/7/23.
//
#include <iostream>
#include <opencv2/videoio.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <chrono>
#include <ctime>
#include "sys/stat.h"
#include "utils.h"
#include "opencv_pushstream.h"
#include "multicore_comm.h"
#include <csignal>

using namespace cv;
using namespace std;

int gst_audio_pid = 0;

void OpencvRecordVideo(){
    VideoCapture cap(0,cv::CAP_V4L2);
    if (!cap.isOpened()) {
        std::cerr << "Error: Unable to open the camera" << std::endl;
        return;
    }
    string save_filename;
    string saveFilePath = "/mnt/mmc/result/video/";

    struct stat info{};
    if (stat(saveFilePath.c_str(), &info) == 0 && (info.st_mode & S_IFDIR)) {
        std::cout << "Path exists.\n";
        string time_now = getCurrentTimestamp();
        save_filename = saveFilePath + time_now + "_output.avi";
    } else {
        std::cout << "Path does not exist.\n";
        string time_now = getCurrentTimestamp();
        save_filename = "/home/root/temp/result/video/" + time_now + "_output.avi";
    }


    cv::Mat frame;
    cv::VideoWriter writer;
    int frame_width = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    int frame_height = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    writer.open(save_filename, cv::VideoWriter::fourcc('Y', 'U', 'Y', 'V'), 30,
                cv::Size(frame_width, frame_height), true);

    if (!writer.isOpened()) {
        std::cerr << "Error: Unable to open the video file for writing" << std::endl;
        return;
    }

    while (is_keep_recording) {
        cap >> frame;
        if (frame.empty()) {
            std::cerr << "Error: Blank frame grabbed" << std::endl;
            break;
        }
        writer.write(frame);
    }

    cap.release();
    writer.release();
}

void OpencvTakePhoto(){
    VideoCapture cap(0,cv::CAP_V4L2);
    string filename;
    string saveFilePath = "/mnt/mmc/result/image/";

    struct stat info{};
    if (stat(saveFilePath.c_str(), &info) == 0 && (info.st_mode & S_IFDIR)) {
        std::cout << "mmc path exists.\n";
        string time_now = getCurrentTimestamp();
        filename = saveFilePath + time_now + ".jpg";
    } else {
        std::cout << "mmc path does not exist. saved to temp file!\n";
        string time_now = getCurrentTimestamp();
        filename = "/home/root/temp/result/image/" + time_now + "_output.jpg";
    }

//    cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('Y', 'U', 'Y', 'V'));
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    int frameWidth = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    int frameHeight = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    int fourcc = static_cast<int>(cap.get(cv::CAP_PROP_FOURCC));

    std::cout << "Current resolution: " << frameWidth << "x" << frameHeight << std::endl;
    std::cout << "Current format: " << cv::format("%c%c%c%c",
                                                  fourcc & 0XFF,
                                                  (fourcc >> 8) & 0XFF,
                                                  (fourcc >> 16) & 0XFF,
                                                  (fourcc >> 24) & 0XFF) << std::endl;
    if(!cap.isOpened()){
        cerr << "open fail !\n" << endl;
    }

    Mat cheeseImg;
    cap >> cheeseImg;
    if (cheeseImg.empty()) {
        std::cerr << "Error: Could not capture image" << std::endl;
    }
    cv::imwrite(filename, cheeseImg);
    cap.release();
}

void OpencvPushStream(){
    VideoCapture cap(0,cv::CAP_V4L2);

    cap.set(CV_CAP_PROP_FOURCC, CV_FOURCC('Y','U','Y','V'));
    cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
    cap.set(CV_CAP_PROP_FPS, 30.0);
    if(!cap.isOpened()){
        cerr << "open fail !\n" << endl;
    }
    cout << "cap opened, FPS: ";
    cout << cap.get(CV_CAP_PROP_FPS) << endl;

    auto capture_forcc = cap.get(CV_CAP_PROP_FOURCC);
    cout << "capture opened" << endl;

    VideoWriter writer, audio_writer;

    stringstream  gst_pipeline;
    stringstream  gst_audio_pipeline;

    string rtsp_location = "rtsp://118.195.217.196:8854/helmet_stream";

    gst_pipeline << "appsrc ! autovideoconvert ! vpuenc_h264 bitrate=600 ! queue ! rtspclientsink location=" << rtsp_location <<  " protocols=tcp  ";

    // Write this string to one line to be sure!!

    writer.open(gst_pipeline.str(), cv::CAP_GSTREAMER ,
                0 , 30, Size(640, 480), true);
    if(!writer.isOpened()){
        cerr << "writer open fail" << endl;
    }
    cout << "Pipeline opened, play location: ";
    cout << rtsp_location << endl;
    Mat img;

    cout << is_keep_pushing << endl;

    while (is_keep_pushing)
    {
        cap.read(img);
        writer << img;
    }

    std::cout << "end push!" << std::endl;
    cap.release();
    writer.release();
}

