#include "classes/transmitter.h"
#include "classes/server.h"
#include <opencv2/opencv.hpp>

int main(int argc, char **argv) 
{
    int version = 2;
    if(argc == 2) { version = atoi(argv[1]); }
    if(version == 1) {
        server serverTransmitter = server();
        serverTransmitter.listen();
    } else if(version == 2) {
        serverThreaded serverTransmitter = serverThreaded();
        serverTransmitter.listen();
    } else {
        serverPreThreaded serverTransmitter = serverPreThreaded((version == 4));
        serverTransmitter.listen();
    }
    exit(0);
}