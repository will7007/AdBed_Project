#include "server.h"

//yellow = fd queue is full
//blue = thead is hungry but queue is empty, so time to sleep
//red = thread is eating a fd/thread created to eat fd
//cyan = fd added to queue by server
//green = messages from the client

void server::listen() {
    struct sockaddr_in clientaddr;
    socklen_t clientlen = sizeof(clientaddr);
    
    printf("Running single-threaded server\n");
    fileDescriptorListen = Open_listenfd(port);
    while (1) {
        int fileDescriptor = Accept(fileDescriptorListen, (SA *)&clientaddr, &clientlen);
        displayConnectionInfo(&clientaddr);
        transaction(this, &fileDescriptor);
    }
}

cv::Mat* server::operate(cv::Mat *input) {
    cv::Mat *grayImage = new cv::Mat();
    cv::cvtColor(*input, *grayImage, cv::COLOR_BGR2GRAY);
    return grayImage;
}

void server::transaction(server *caller, int *fileDescriptor) {
    printf("Receiving image from client\n");
    cv::Mat *receivedImage = caller->receive(*fileDescriptor);
    printf("Received image over socket fd %d, sleeping for %d seconds\n", *fileDescriptor, caller->sleepTime);
    sleep(caller->sleepTime);
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
