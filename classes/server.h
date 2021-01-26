#ifndef __SERVER_H__
#define __SERVER_H__

#include "transmitter.h"
#include "../csapp/csapp.h"
#include <pthread.h>
#include <queue>
#include "../colors.h"

class server : public transmitter {
protected:
    int fileDescriptorListen;
    int sleepTime = 10;
    bool running = true;
    bool transaction(server *caller, int *fileDescriptor);
    void displayConnectionInfo(sockaddr_in *clientaddr);
public:
    void listen();
    cv::Mat* operate(cv::Mat *input, uint8_t operations);
};

class serverThreaded : public server {
protected:
    static void* transactionThreaded(void *connectionFileDescriptor);
    pthread_t* threadID;
    struct threadArgs {
        serverThreaded* caller;
        int* fd;
    };
public:
    void listen();
};

class serverPreThreaded : public serverThreaded {
protected:
    bool prioritizeMainThread;
    std::queue<int *> connectionQueue;
    pthread_mutex_t queueMutex;
    static void* transactionConsumer(void *callerArg);
    int maxThreads = 7;
    pthread_cond_t hungry, justAte;
    int queueLimit = 14; //should be 2*maxThreads
    int getThreadPriority();
public:
    serverPreThreaded(bool prioritizeMainThreadArg = false);
    void listen();
};
#endif
