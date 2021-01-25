#include "server.h"

void serverThreaded::listen() {
    struct sockaddr_in clientaddr;
    socklen_t clientlen = sizeof(clientaddr);
    fileDescriptorListen = Open_listenfd(port);
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

void* serverThreaded::transactionThreaded(void *args) {
    threadArgs* argsStruct = ((threadArgs*)args);
    printf(FRED("Socket fd %d is being handled in a new thread\n"), (*(argsStruct->fd)));
    argsStruct->caller->transaction(argsStruct->caller,argsStruct->fd);
    delete argsStruct->fd;
    delete argsStruct;
    return nullptr;
}