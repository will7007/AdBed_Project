#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "transmitter.h"
#include <string>
#include "../csapp/csapp.h"

using std::string;

class client : public transmitter {
private:
    //port
    //fileDescriptor
    string host = "127.0.0.1";
    // string pictureName = "./lena.jpg"; //transmitter should not worry about remembering image/Mat
    string originalName = "Original Image";
    string receivedName = "Remotely Modified Image";
    int clientNumber = 0;
public:
    // client(string& hostArg,
    //         // string& pictureNameArg,
    //         string& originalNameArg,
    //         string& receivedNameArg,
    //         int portArg = 111,
    //         int clientNumberArg = 0);
    // client(string& pictureName);
    client();
    ~client();
    void show(cv::Mat *imgOriginal, cv::Mat *imgModified);
    int setup();
    int setup(string& hostArg, int portArg);
};
#endif
