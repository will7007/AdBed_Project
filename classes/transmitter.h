#ifndef __TRANSMITTER_H__
#define __TRANSMITTER_H__

#include <opencv2/opencv.hpp>
#include <vector>
#include <sys/socket.h>
#include "../csapp/csapp.h"

class transmitter {
protected:
    int port;
    int fileDescriptor;
    transmitter(int portArg = 111) { port = portArg; };
    ~transmitter() {};
public:
    cv::Mat *receive(int socket, int width, int height, int channels);
    int send(int socket, cv::Mat image, int imgWidth, int imgHeight, int channels); //FIXME automatically use fileDescriptor as socket fd
    std::vector<char> matToBytes(cv::Mat image);
    cv::Mat bytesToMat(char *bytes,int width,int height,bool grey);
    cv::Mat *receiveImage(int socket,int width,int height,int channels);
    int getPort() { return port; }
    int getFileDescriptor() { return fileDescriptor; }
};
#endif
