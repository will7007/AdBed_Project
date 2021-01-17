#include "server.h"

server::server(int codeVersionArg) {
    codeVersion = codeVersionArg;
    fileDescriptorListen = Open_listenfd(port);
}

void server::listen() {
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    struct hostent *hp;
    char *haddrp;
    clientlen = sizeof(clientaddr);
    if(codeVersion == 1) {
        //normal version
        printf("Running single-threaded server\n");
        while (1) {
            int fileDescriptor = Accept(fileDescriptorListen, (SA *)&clientaddr, &clientlen);

            /* Determine the domain name and IP address of the client */
            hp = Gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, 
                    sizeof(clientaddr.sin_addr.s_addr), AF_INET);
            haddrp = inet_ntoa(clientaddr.sin_addr);
            printf("server connected to %s (%s)\n", hp->h_name, haddrp);

            cv::Mat *receivedImage = this->receive(fileDescriptor);
            printf("Image received\n");
            cv::Mat *newImage = this->operate(receivedImage);
            delete receivedImage->datastart;
            delete receivedImage;
            printf("Server sent %d bytes\n", this->send(newImage, fileDescriptor));
            delete newImage;
            
            Close(fileDescriptor);
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

            pthread_create(&tid, NULL, &transactionThreaded, (void *)args);
        }
    }
    exit(0);
}

cv::Mat * server::operate(cv::Mat *input) {
    cv::Mat *grayImage = new cv::Mat();
    cv::cvtColor(*input, *grayImage, cv::COLOR_BGR2GRAY);
    return grayImage;
}

void* server::transactionThreaded(void *args) { //points to array of void *
    threadArgs* argsStruct = ((threadArgs*)args);
    int fileDescriptor = *(argsStruct->fd);
    printf("Socket fd %d is being handled in a new thread\n",fileDescriptor);
    cv::Mat *receivedImage = argsStruct->caller->receive(fileDescriptor); //the object pointer works, but receive() seems to be getting the wrong file descriptor
    printf("Image received, time to sleep for 30 seconds\n");
    sleep(30);
    cv::Mat *newImage = argsStruct->caller->operate(receivedImage);
    delete receivedImage->datastart;
    delete receivedImage;
    printf("Server thread with fd %d sent %d bytes\n", fileDescriptor, argsStruct->caller->send(newImage, fileDescriptor));
    delete newImage;
    Close(fileDescriptor);
    delete argsStruct->fd;
    delete argsStruct;
    return NULL;
}
