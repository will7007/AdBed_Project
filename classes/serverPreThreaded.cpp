#include "server.h"

serverPreThreaded::serverPreThreaded(bool prioritizeMainThreadArg) {
    prioritizeMainThread = prioritizeMainThreadArg;
}

void serverPreThreaded::listen() {
    struct sockaddr_in clientaddr;
    socklen_t clientlen = sizeof(clientaddr);        
    printf("Running producer/consumer pre-threaded server\n");
    fileDescriptorListen = Open_listenfd(port);
    threadID = new pthread_t[maxThreads];
    pthread_attr_t threadAttr;
    pthread_mutex_init(&queueMutex, NULL);
    pthread_cond_init (&hungry, NULL);
    pthread_cond_init (&justAte, NULL);

    if(prioritizeMainThread) {
        struct sched_param my_param;
        my_param.sched_priority = sched_get_priority_max(SCHED_RR);
        pthread_setschedparam(pthread_self(), SCHED_RR, &my_param);
        printf("Main thread priority assigned to %d\n",getThreadPriority());

        pthread_attr_init(&threadAttr);
        pthread_attr_setinheritsched(&threadAttr, PTHREAD_EXPLICIT_SCHED);
        pthread_attr_setschedpolicy(&threadAttr, SCHED_RR);
        if(my_param.sched_priority > 1) { my_param.sched_priority -= 1;}
        pthread_attr_setschedparam(&threadAttr, &my_param);

        for(int threads = 0; threads < maxThreads; threads++) {
            pthread_create(&threadID[threads], &threadAttr, &transactionConsumer, static_cast<void *>(this));
            printf("Thead %d created\n", threads);
        }
    } else {
        for(int threads = 0; threads < maxThreads; threads++) {
            pthread_create(&threadID[threads], NULL, &transactionConsumer, static_cast<void *>(this));
            printf("Thead %d created\n", threads);
        }
    }

    while(1) {
        int *fileDescriptor = new int;
        *fileDescriptor = Accept(fileDescriptorListen, (SA *)&clientaddr, &clientlen);
        displayConnectionInfo(&clientaddr);
        pthread_mutex_lock(&queueMutex);
        if(connectionQueue.size() >= queueLimit) {
            printf(FYEL("The queue is full (with %lu file descriptors)\n"), connectionQueue.size());
            printf(FYEL("Connection %d will be added once there is room: main thread is waiting for room...\n"), *fileDescriptor);
            pthread_cond_wait(&justAte,&queueMutex);
        }
        connectionQueue.push(fileDescriptor);
        printf(FCYN("Connection %d was added to the queue: size is now %lu\n"), *fileDescriptor, connectionQueue.size());
        pthread_mutex_unlock(&queueMutex);
        pthread_cond_signal(&hungry);
    }
    delete threadID;
    pthread_mutex_destroy(&queueMutex);
    pthread_cond_destroy(&hungry);
    pthread_cond_destroy(&justAte);
}

void* serverPreThreaded::transactionConsumer(void *callerArg) {
    serverPreThreaded* caller = static_cast<serverPreThreaded *>(callerArg);
    pthread_t id = pthread_self();
    int threadNumber = -1;
    for(int IDNum = 0; IDNum < caller->maxThreads; IDNum++) {
        if(pthread_equal(id,caller->threadID[IDNum])) {
            printf("Thread %d is now active with priority of %d\n", IDNum, caller->getThreadPriority());
            threadNumber = IDNum;
            break;
        }
    }
    if(threadNumber < -1) { printf("Something is wrong with the threads... (thread ID is -1\n"); }
    int processedImages = 0;
    while(1) { //TODO: break out of this loop to rejoin main when a running variable inside of caller is set to false
        pthread_mutex_lock(&(caller->queueMutex));
        if(caller->connectionQueue.empty()) {
            printf(FBLU("Connection queue is empty, so thread %d is sleeping\n"), threadNumber);
            pthread_cond_wait(&(caller->hungry), &(caller->queueMutex));
        }
        int fileDescriptor = *(caller->connectionQueue.front());
        delete caller->connectionQueue.front();
        caller->connectionQueue.pop();
        pthread_mutex_unlock(&(caller->queueMutex));
        pthread_cond_signal(&(caller->justAte));
        printf(FRED("Thread %d just ate connection %d\n"), threadNumber, fileDescriptor);
        caller->transaction(caller, &fileDescriptor);
        printf(FBLU("Thread %d has processed %d image(s) so far\n"), threadNumber, ++processedImages);
    }
    printf("Thread %d is shutting down\n", threadNumber);
    return nullptr;
}

int serverPreThreaded::getThreadPriority() {
    pthread_t thread_id = pthread_self();
	struct sched_param param;
	int policy, ret;
	ret = pthread_getschedparam (thread_id, &policy, &param);
	return param.sched_priority;
}