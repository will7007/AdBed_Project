#include "client.h"

client::client(string& hostArg, int portArg, int clientNumberArg) {
    port = portArg;
    host = hostArg;
    clientNumber = clientNumberArg;
    this->setup();
}

client::client(int clientNumberArg) {
    clientNumber = clientNumberArg;
    this->setup();
}

client::~client() { close(fileDescriptor); }

void client::show(cv::Mat *imgOriginal, cv::Mat *imgModified) {
    cv::namedWindow( "Original Image", cv::WINDOW_AUTOSIZE );
    cv::imshow( "Original Image", *imgOriginal );
    cv::namedWindow( "Remotely Modified Image", cv::WINDOW_AUTOSIZE );
    cv::imshow( "Remotely Modified Image", *imgModified );
    cv::waitKey(0);
}

int client::setup() {
    return fileDescriptor = Open_clientfd(const_cast<char *>(host.c_str()), port);
}
