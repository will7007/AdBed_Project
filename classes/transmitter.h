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
    int port;
    int fileDescriptor;
    transmitter(int portArg = 111) { port = portArg; };
    virtual ~transmitter() {};
public:
    cv::Mat* receive(int width, int height, int channels);
    int send(cv::Mat* image, int imgWidth, int imgHeight, int channels);
    char* matToBytes(cv::Mat* image);
    cv::Mat* bytesToMat(char *bytes,int width,int height,bool grey);
    // cv::Mat* receiveImage(int socket,int width,int height,int channels);
    int getPort() { return port; }
    int getFileDescriptor() { return fileDescriptor; }
};
#endif
