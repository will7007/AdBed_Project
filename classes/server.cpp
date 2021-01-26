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
    while (running) {
        int fileDescriptor = Accept(fileDescriptorListen, (SA *)&clientaddr, &clientlen);
        displayConnectionInfo(&clientaddr);
        running = transaction(this, &fileDescriptor);
    }
    close(fileDescriptorListen);
}

cv::Mat* server::operate(cv::Mat *input, uint8_t operations) {
    cv::Mat *newImage = new cv::Mat();
    //input->copyTo(&newImage);
    if(operations == 0x0) {
        delete newImage;
        return nullptr;
    } else {
        input->copyTo(*newImage);
        if(operations & 0x1) {
            cv::cvtColor(*newImage, *newImage, cv::COLOR_BGR2GRAY);
        }
        if(operations & 0x2) {
            cv::flip(*newImage, *newImage, 1);
        }
        if(operations & 0x4) {
            cv::resize(*newImage, *newImage, cv::Size(), 0.5, 0.5);
        }
    return newImage;
    }
}

bool server::transaction(server *caller, int *fileDescriptor) {
    printf("Receiving image from client\n");
    std::pair<transmitter::transmitSize*, cv::Mat*> receivedData = caller->receive(*fileDescriptor);
    cv::Mat *receivedImage = receivedData.second;
    uint8_t operations = receivedData.first->operations;
    printf("Received image over socket fd %d, performing requested OpenCV operations\n", *fileDescriptor);
    cv::Mat *newImage = caller->operate(receivedImage, operations);
    delete receivedImage->datastart;
    delete receivedImage;
    if(newImage != nullptr && newImage->data) {
        printf("Sleeping for %d seconds\n", caller->sleepTime);
        sleep(caller->sleepTime);
        printf("Server thread with fd %d sent %d bytes\n", *fileDescriptor, caller->send(newImage, *fileDescriptor));
        delete newImage;
        Close(*fileDescriptor);
        return true;
    } else {
        printf("No operations were requested by the client: shutting down\n");
        Close(*fileDescriptor);
        return false;
    }
   
}

void server::displayConnectionInfo(sockaddr_in *clientaddr) {
    struct hostent *hp;
    char *haddrp;
    hp = Gethostbyaddr((const char *)&(*clientaddr).sin_addr.s_addr, 
                        sizeof(clientaddr->sin_addr.s_addr), AF_INET);
    haddrp = inet_ntoa(clientaddr->sin_addr);
    printf("server connected to %s (%s)\n", hp->h_name, haddrp);
}
