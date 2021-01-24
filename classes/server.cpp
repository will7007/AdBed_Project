#include "server.h"

//yellow = fd queue is full
//blue = thead is hungry but queue is empty, so time to sleep
//red = thread is eating a fd/thread created to eat fd
//cyan = fd added to queue by server
//green = messages from the client

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
        //std::cout << "Running single-threaded server\n";
        while (1) {
            int fileDescriptor = Accept(fileDescriptorListen, (SA *)&clientaddr, &clientlen);
            displayConnectionInfo(&clientaddr);
            transaction(this, &fileDescriptor);
        }
    }
    
    else if(codeVersion == 2) {
        threadID = new pthread_t;
        printf("Running multi-threaded server\n");
        //std::cout << "Running multi-threaded server\n";
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

    else if(codeVersion == 3 || codeVersion == 4) {
        printf("Running producer/consumer pre-threaded server\n");
        //std::cout << "Running producer/consumer pre-threaded server\n";
        threadID = new pthread_t[maxThreads];
        pthread_attr_t threadAttr; //making this a pointer to nullptr caused a segfault
        pthread_mutex_init(&queueMutex, NULL);
        pthread_cond_init (&hungry, NULL);
        pthread_cond_init (&justAte, NULL);

        if(codeVersion == 4) {
            struct sched_param my_param;
            my_param.sched_priority = sched_get_priority_max(SCHED_RR);
            // printf("Thead priorities will also be set to %d\n",my_param.sched_priority);
            pthread_setschedparam(pthread_self(), SCHED_RR, &my_param);
            printf("Main thread priority assigned to %d\n",getThreadPriority());
            //std::cout << "Main thread priority assigned to " << getThreadPriority() << std::endl;

            //explicitly set policy to round robbin and a lower priority than main
            //(if we are able to make it lower) for worker threads
            pthread_attr_init(&threadAttr);
            pthread_attr_setinheritsched(&threadAttr, PTHREAD_EXPLICIT_SCHED);
            pthread_attr_setschedpolicy(&threadAttr, SCHED_RR);
            if(my_param.sched_priority > 1) { my_param.sched_priority -= 1;}
            pthread_attr_setschedparam(&threadAttr, &my_param);

            for(int threads = 0; threads < maxThreads; threads++) {
                pthread_create(&threadID[threads], &threadAttr, &transactionConsumer, static_cast<void *>(this));
                printf("Thead %d created\n", threads);
                //std::cout << "Thead " << threads << " created\n";
            }
        } else {
            for(int threads = 0; threads < maxThreads; threads++) {
                pthread_create(&threadID[threads], NULL, &transactionConsumer, static_cast<void *>(this));
                printf("Thead %d created\n", threads);
                //std::cout << "Thead " << threads << " created\n";
            }
        }
   
        while(1) {
            int *fileDescriptor = new int;
            *fileDescriptor = Accept(fileDescriptorListen, (SA *)&clientaddr, &clientlen);
            displayConnectionInfo(&clientaddr);
            pthread_mutex_lock(&queueMutex);
            if(connectionQueue.size() >= queueLimit) {
                printf(FYEL("The queue is full (with %lu file descriptors)\n"), connectionQueue.size());
                //std::cout << "The queue is full (with " << connectionQueue.size() << " file descriptors)\n";
                printf(FYEL("Connection %d will be added once there is room: main thread is waiting for room...\n"), *fileDescriptor);
                //std::cout << "Connection " << *fileDescriptor << " will be added once there is room: main thread is waiting for room...\n";
                pthread_cond_wait(&justAte,&queueMutex);
            }
            connectionQueue.push(fileDescriptor);
            printf(FCYN("Connection %d was added to the queue: size is now %lu\n"), *fileDescriptor, connectionQueue.size());
            //std::cout << "Connection " << *fileDescriptor << " was added to the queue: size is now " << connectionQueue.size() << std::endl;
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
            printf("Thread %d is now active with priority of %d\n", IDNum, caller->getThreadPriority());
            //std::cout << "Thread " << IDNum << " is now active with priority of " << caller->getThreadPriority() << std::endl;
            threadNumber = IDNum;
            break;
        }
    }
    if(threadNumber < -1) { printf("Something is wrong with the threads... (thread ID is -1\n"); }
    // { std::cout << "Something is wrong with the threads... (thread ID is -1)\n"; }
    int processedImages = 0;
    while(1) { //TODO: break out of this loop to rejoin main when a running variable inside of caller is set to false
        pthread_mutex_lock(&(caller->queueMutex));
        if(caller->connectionQueue.empty()) {
            printf(FBLU("Connection queue is empty, so thread %d is sleeping\n"), threadNumber);
            //std::cout << "Connection queue is empty, so thread " << threadNumber << " is sleeping\n";
            pthread_cond_wait(&(caller->hungry), &(caller->queueMutex));
        }
        int fileDescriptor = *(caller->connectionQueue.front());
        delete caller->connectionQueue.front();
        caller->connectionQueue.pop();
        pthread_mutex_unlock(&(caller->queueMutex));
        pthread_cond_signal(&(caller->justAte));
        printf(FRED("Thread %d just ate connection %d\n"), threadNumber, fileDescriptor);
        //std::cout << "Thread " << threadNumber << " just ate connection " << fileDescriptor << std::endl;
        caller->transaction(caller, &fileDescriptor);
        printf(FBLU("Thread %d has processed %d image(s) so far\n"), threadNumber, ++processedImages);
    }
    printf("Thread %d is shutting down\n", threadNumber);
    //std::cout << "Thread " << threadNumber << " is shutting down\n";
    return nullptr;
}

void* server::transactionThreaded(void *args) { //points to array of void *
    threadArgs* argsStruct = ((threadArgs*)args);
    printf(FRED("Socket fd %d is being handled in a new thread\n"), (*(argsStruct->fd)));
    //std::cout << "Socket fd " << (*(argsStruct->fd)) << " is being handled in a new thread\n";
    argsStruct->caller->transaction(argsStruct->caller,argsStruct->fd);
    delete argsStruct->fd;
    delete argsStruct;
    return nullptr;
}

void server::transaction(server *caller, int *fileDescriptor) {
    printf("Receiving image from client\n");
    //std::cout << "Receiving image from client\n";
    cv::Mat *receivedImage = caller->receive(*fileDescriptor);
    printf("Received image over socket fd %d, sleeping for %d seconds\n", *fileDescriptor, caller->sleepTime);
    //std::cout << "Image received over socket fd " << *fileDescriptor << ", sleeping for " << caller->sleepTime << " seconds\n";
    sleep(caller->sleepTime);
    printf("Performing OpenCV operations on image\n");
    //std::cout << "Performing OpenCV operations on image\n";
    cv::Mat *newImage = caller->operate(receivedImage);
    delete receivedImage->datastart;
    delete receivedImage;
    printf("Server thread with fd %d sent %d bytes\n", *fileDescriptor, caller->send(newImage, *fileDescriptor));
    //std::cout << "Server thread with fd " << *fileDescriptor << " sent " << caller->send(newImage, *fileDescriptor) << " bytes\n";
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
    //std::cout << "server connected to " << hp->h_name << " (" << haddrp << ")\n";
}

int server::getThreadPriority() {
    pthread_t thread_id = pthread_self();
	struct sched_param param;
	int policy, ret;
	ret = pthread_getschedparam (thread_id, &policy, &param);
	return param.sched_priority;
}