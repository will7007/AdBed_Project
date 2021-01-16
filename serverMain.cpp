#include "classes/transmitter.h"
#include "classes/server.h"
#include <opencv2/opencv.hpp>

int main(int argc, char **argv) 
{
    server serverTransmitter = server();
    serverTransmitter.listen();
    exit(0);
}