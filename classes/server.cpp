#include "server.h"

server::server() {
    // port = portArg;
    fileDescriptorListen = Open_listenfd(port);
}

void server::listen() {
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    struct hostent *hp;
    char *haddrp;
    while (1) {
        clientlen = sizeof(clientaddr);
        fileDescriptor = Accept(fileDescriptorListen, (SA *)&clientaddr, &clientlen);

        /* Determine the domain name and IP address of the client */
        hp = Gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, 
                sizeof(clientaddr.sin_addr.s_addr), AF_INET);
        haddrp = inet_ntoa(clientaddr.sin_addr);
        printf("server connected to %s (%s)\n", hp->h_name, haddrp);

        cv::Mat *receivedImage = this->receive(fileDescriptor,512,512,3);
        cv::Mat *newImage = this->operate(receivedImage);
        delete receivedImage;
        printf("Server sent %d bytes\n", this->send(fileDescriptor,*newImage,512,512,1));
        delete newImage;
        
        Close(fileDescriptor);
    }
    exit(0);
}
cv::Mat * server::operate(cv::Mat *input) {
    cv::Mat *grayImage = new cv::Mat();
    cv::cvtColor(*input, *grayImage, cv::COLOR_BGR2GRAY);
    return grayImage;
}
