#include "classes/transmitter.h"
#include "classes/server.h"
#include <opencv2/opencv.hpp>

int main(int argc, char **argv) 
{
    int version = 2;
    if(argc == 2) { version = atoi(argv[1]); }
    server serverTransmitter = server(version);
    serverTransmitter.listen();
    exit(0);
}