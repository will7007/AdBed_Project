#include "server.h"

server::server(int codeVersionArg) {
    codeVersion = codeVersionArg;
    fileDescriptorListen = Open_listenfd(port);
}

void server::listen() {
    struct sockaddr_in clientaddr;
    socklen_t clientlen = sizeof(clientaddr);
    if(codeVersion == 1) {
        //normal version
        printf("Running single-threaded server\n");
        while (1) {
            int fileDescriptor = Accept(fileDescriptorListen, (SA *)&clientaddr, &clientlen);
            displayConnectionInfo(&clientaddr);
            transaction(this, &fileDescriptor);
        }
    }
    else if(codeVersion == 2) {
        pthread_t tid;
        printf("Running multi-threaded server\n");
        while(1) {
            int *fileDescriptor = new int;
            *fileDescriptor = Accept(fileDescriptorListen, (SA *)&clientaddr, &clientlen);
            threadArgs* args = new threadArgs;
            args->caller = this;
            args->fd = fileDescriptor;
            displayConnectionInfo(&clientaddr);
            pthread_create(&tid, NULL, &transactionThreaded, (void *)args);
        }
    }
    exit(0);
}

cv::Mat* server::operate(cv::Mat *input) {
    cv::Mat *grayImage = new cv::Mat();
    cv::cvtColor(*input, *grayImage, cv::COLOR_BGR2GRAY);
    return grayImage;
}

void* server::transactionThreaded(void *args) { //points to array of void *
    threadArgs* argsStruct = ((threadArgs*)args);
    printf("Socket fd %d is being handled in a new thread\n",(*(argsStruct->fd)));
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
