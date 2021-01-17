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
    int port = 51238;
    int fileDescriptor;
    transmitter() {};
    virtual ~transmitter() {};
    cv::Mat* receiveImage(int width, int height, int channels);
    int sendImage(cv::Mat* image);

    struct transmitSize {
        int width;
        int height;
        uint8_t channels;
        uint8_t operations;
        //10 bytes total
    };
    transmitSize* receiveSize();
    bool sendSize(cv::Mat* image);
    cv::Mat* receiveImage(transmitter::transmitSize *sizeStruct);
public:
    cv::Mat* receive(int channels); //should have no arguments in the future
    int send(cv::Mat* image);
    char* matToBytes(cv::Mat* image);
    cv::Mat* bytesToMat(char *bytes,int width,int height,bool grey);
    int getPort() { return port; }
    int getFileDescriptor() { return fileDescriptor; }
};
#endif
