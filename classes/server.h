#ifndef __SERVER_H__
#define __SERVER_H__

#include "transmitter.h"
#include "../csapp/csapp.h"

class server : public transmitter {
private:
    //port
    //fileDescriptor
    int fileDescriptorListen;
public:
    server(int portArg = 111);
    ~server() {};
    void listen();
    cv::Mat* operate(cv::Mat *input);
};
#endif
