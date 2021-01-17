#ifndef __TRANSMITTER_H__
#define __TRANSMITTER_H__

#include <opencv2/opencv.hpp>
#include <vector>
#include <sys/socket.h>
#include "../csapp/csapp.h"

class transmitter {
    /**Think of the transmitter base class like an envelope:
     * Transmitter objects are like mailing envelopes: they have a port and a
     * fileDescriptor like an envelope has an address and a stamp, but the
     * contents of the envelope (cv::Mat image) are separate from the
     * envelope itself
     */
protected:
    int port = 111;
    // int fileDescriptor; //must be dynamically created or made in queue
    transmitter() {};
    virtual ~transmitter() {};
    struct transmitSize {
        int width;
        int height;
        uint8_t channels;
        uint8_t operations;
        //10 bytes total
    };
    cv::Mat* receiveImage(transmitSize *sizeStruct, int fileDescriptor);
    cv::Mat* receiveImage(int width, int height, int channels, int fileDescriptor);
    int sendImage(cv::Mat* image, int fileDescriptor);
    transmitSize* receiveSize(int fileDescriptor);
    bool sendSize(cv::Mat* image, int fileDescriptor);
public:
    cv::Mat* receive(int fileDescriptor);
    int send(cv::Mat* image, int fileDescriptor);
    char* matToBytes(cv::Mat* image);
    cv::Mat* bytesToMat(char *bytes,int width,int height,bool grey);
    int getPort() { return port; }
    // int getFileDescriptor() { return fileDescriptor; }
};
#endif
