#include "client.h"

client::client(string& hostArg, int portArg, int clientNumberArg) {
    port = portArg;
    host = hostArg;
    clientNumber = clientNumberArg;
    setup();
}

client::client(int clientNumberArg) {
    clientNumber = clientNumberArg;
    setup();
}

int client::setup() {
    return clientFileDescriptor = Open_clientfd(const_cast<char *>(host.c_str()), port);
}

client::~client() { close(clientFileDescriptor); }

void client::show(cv::Mat *imgOriginal, cv::Mat *imgModified) {
    cv::namedWindow( "Original Image", cv::WINDOW_AUTOSIZE );
    cv::imshow( "Original Image", *imgOriginal );
    cv::namedWindow( "Remotely Modified Image", cv::WINDOW_AUTOSIZE );
    cv::imshow( "Remotely Modified Image", *imgModified );
    cv::waitKey(0);
}

cv::Mat* client::receive() { return transmitter::receive(clientFileDescriptor); }

int client::send(cv::Mat* image) { return transmitter::send(image, clientFileDescriptor); }
