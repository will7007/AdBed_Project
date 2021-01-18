#include "server.h"

server::server(int codeVersionArg) {
    codeVersion = codeVersionArg;
    fileDescriptorListen = Open_listenfd(port);
    // queueMutex = new pthread_mutex_t; //mutex MUST have allocated memory for initalization
}

void server::listen() {
    struct sockaddr_in clientaddr;
    socklen_t clientlen = sizeof(clientaddr);
    
    if(codeVersion == 1) {
        printf("Running single-threaded server\n");
        while (1) {
            int fileDescriptor = Accept(fileDescriptorListen, (SA *)&clientaddr, &clientlen);
            displayConnectionInfo(&clientaddr);
            transaction(this, &fileDescriptor);
        }
    }
    
    else if(codeVersion == 2) {
        threadID = new pthread_t;
        printf("Running multi-threaded server\n");
        while(1) {
            int *fileDescriptor = new int;
            *fileDescriptor = Accept(fileDescriptorListen, (SA *)&clientaddr, &clientlen);
            threadArgs* args = new threadArgs;
            args->caller = this;
            args->fd = fileDescriptor;
            displayConnectionInfo(&clientaddr);
            pthread_create(threadID, NULL, &transactionThreaded, static_cast<void *>(args));
        }
        delete threadID;
    }

    else if(codeVersion == 3) {
        threadID = new pthread_t[maxThreads];
        printf("Running producer/consumer pre-threaded server\n");
        pthread_mutex_init(&queueMutex, NULL);
        printf("Mutex initalized\n");
        pthread_cond_init (&hungry, NULL);
        pthread_cond_init (&justAte, NULL);
        printf("CVs initalized\n");
        for(int threads = 0; threads < maxThreads; threads++) {
            pthread_create(&threadID[threads], NULL, &transactionConsumer, static_cast<void *>(this));
            printf("Thead %d created\n", threads);
        }
        while(1) {
            int *fileDescriptor = new int;
            *fileDescriptor = Accept(fileDescriptorListen, (SA *)&clientaddr, &clientlen);
            displayConnectionInfo(&clientaddr);
            pthread_mutex_lock(&queueMutex);
            if(connectionQueue.size() >= queueLimit) {
                printf("The queue is full (with %lu file descriptors)\n", connectionQueue.size());
                printf("Connection %d will be added once there is room: main thread is waiting for room...\n", *fileDescriptor);
                pthread_cond_wait(&justAte,&queueMutex);
            }
            connectionQueue.push(fileDescriptor);
            printf("Connection %d was added to the queue: size is now %lu\n", *fileDescriptor, connectionQueue.size());
            pthread_mutex_unlock(&queueMutex);
            pthread_cond_signal(&hungry);
        }
        delete threadID;
        pthread_mutex_destroy(&queueMutex);
        pthread_cond_destroy(&hungry);
        pthread_cond_destroy(&justAte);
    }
    exit(0);
}

cv::Mat* server::operate(cv::Mat *input) {
    cv::Mat *grayImage = new cv::Mat();
    cv::cvtColor(*input, *grayImage, cv::COLOR_BGR2GRAY);
    return grayImage;
}

void* server::transactionConsumer(void *callerArg) {
    server* caller = static_cast<server *>(callerArg);
    pthread_t id = pthread_self();
    int threadNumber = -1;
    for(int IDNum = 0; IDNum < caller->maxThreads; IDNum++) {
        if(pthread_equal(id,caller->threadID[IDNum])) {
            printf("Thread %d is now active\n", IDNum);
            threadNumber = IDNum;
            break;
        }
    }
    if(threadNumber < -1) { printf("Something is wrong with the threads... (thread ID is -1\n"); }
    while(1) {
        pthread_mutex_lock(&(caller->queueMutex));
        if(caller->connectionQueue.empty()) {
            printf("Connection queue is empty, so thread %d is sleeping\n", threadNumber);
            pthread_cond_wait(&(caller->hungry), &(caller->queueMutex));
        }
        int fileDescriptor = *(caller->connectionQueue.front());
        delete caller->connectionQueue.front();
        caller->connectionQueue.pop();
        pthread_mutex_unlock(&(caller->queueMutex));
        pthread_cond_signal(&(caller->justAte));
        printf("Thread %d just ate connection %d\n", threadNumber, fileDescriptor);
        caller->transaction(caller, &fileDescriptor);
    }
    printf("Thread %d is shutting down\n", threadNumber);
    return nullptr;
}

void* server::transactionThreaded(void *args) { //points to array of void *
    threadArgs* argsStruct = ((threadArgs*)args);
    printf("Socket fd %d is being handled in a new thread\n", (*(argsStruct->fd)));
    argsStruct->caller->transaction(argsStruct->caller,argsStruct->fd);
    delete argsStruct->fd;
    delete argsStruct;
    return nullptr;
}

void server::transaction(server *caller, int *fileDescriptor) {
    printf("Receiving image from client\n");
    cv::Mat *receivedImage = caller->receive(*fileDescriptor);
    printf("Image received over socket fd %d, time to sleep for 10 seconds\n", *fileDescriptor);
    sleep(10);
    printf("Performing OpenCV operations on image\n");
    cv::Mat *newImage = caller->operate(receivedImage);
    delete receivedImage->datastart;
    delete receivedImage;
    printf("Server thread with fd %d sent %d bytes\n", *fileDescriptor, caller->send(newImage, *fileDescriptor));
    delete newImage;
    Close(*fileDescriptor);
}

void server::displayConnectionInfo(sockaddr_in *clientaddr) {
    struct hostent *hp;
    char *haddrp;
    hp = Gethostbyaddr((const char *)&(*clientaddr).sin_addr.s_addr, 
                        sizeof(clientaddr->sin_addr.s_addr), AF_INET);
    haddrp = inet_ntoa(clientaddr->sin_addr);
    printf("server connected to %s (%s)\n", hp->h_name, haddrp);
}
