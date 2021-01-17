#ifndef __SERVER_H__
#define __SERVER_H__

#include "transmitter.h"
#include "../csapp/csapp.h"
#include <pthread.h>

class server : public transmitter {
private:
    //port
    //fileDescriptor
    int fileDescriptorListen;
    int codeVersion = 2;
    static void* transactionThreaded(void *connectionFileDescriptor);
    void transaction(server *caller, int *fileDescriptor);
    void displayConnectionInfo(sockaddr_in *clientaddr);
    struct threadArgs {
        server* caller;
        int* fd;
    };
    void printConnectionInfo(sockaddr_in* clientaddr);
public:
    server(int codeVersionArg = 2);
    ~server() {};
    void listen();
    cv::Mat* operate(cv::Mat *input);
};
#endif
