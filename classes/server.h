#ifndef __SERVER_H__
#define __SERVER_H__

#include "transmitter.h"
#include "../csapp/csapp.h"
#include <pthread.h>
#include <queue>
#include "../colors.h"

class server : public transmitter { //maybe split into successive child classes for different parts?
private:
    int fileDescriptorListen;
    int codeVersion = 2;
    //single-threaded
    void transaction(server *caller, int *fileDescriptor);
    void displayConnectionInfo(sockaddr_in *clientaddr);
    //threaded
    static void* transactionThreaded(void *connectionFileDescriptor);
    struct threadArgs {
        server* caller;
        int* fd;
    };
    //pre-threaded
    std::queue<int *> connectionQueue;
    pthread_mutex_t queueMutex;
    static void* transactionConsumer(void *callerArg);
    int maxThreads = 7;
    pthread_t* threadID;
    pthread_cond_t hungry, justAte;
    int queueLimit = 14; //should be 2*maxThreads
    int getThreadPriority();
    int sleepTime = 10;
public:
    server(int codeVersionArg = 2);
    ~server() {};
    void listen();
    cv::Mat* operate(cv::Mat *input);
};
#endif
