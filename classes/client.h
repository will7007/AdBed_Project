#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "transmitter.h"
#include <string>
#include "../csapp/csapp.h"

using std::string;

class client : public transmitter {
private:
    int clientFileDescriptor;
    string host = "127.0.0.1";
    int clientNumber;
    int setup();
public:
    client(string& hostArg, int portArg, int clientNumberArg);
    client(int clientNumberArg = 0);
    ~client();
    void show(cv::Mat *imgOriginal, cv::Mat *imgModified);
    cv::Mat* receive();
    int send(cv::Mat* image);
    int getFileDescriptor() { return clientFileDescriptor; }
};
#endif
