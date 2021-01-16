#include "client.h"

// client::client(string& hostArg,
//             // string& pictureNameArg,
//             string& originalNameArg,
//             string& receivedNameArg,
//             int portArg = 111,
//             int clientNumberArg = 0) {
//     host = hostArg;
//     originalName = originalNameArg;
//     receivedName = receivedNameArg;
//     port = portArg;
//     clientNumber = clientNumberArg;
//     client(pictureNameArg);
// }

// client::client(string& pictureNameArg) { pictureName = pictureNameArg; }

client::client() { setup(); } //FIXME add flexibility to class

client::~client() { close(fileDescriptor); }

void client::show(cv::Mat *imgOriginal, cv::Mat *imgModified) {
    cv::namedWindow( "Original Image", cv::WINDOW_AUTOSIZE );
    cv::imshow( "Original Image", *imgOriginal );
    cv::namedWindow( "Remotely Modified Image", cv::WINDOW_AUTOSIZE );
    cv::imshow( "Remotely Modified Image", *imgModified );
    cv::waitKey(0);
}

int client::setup(string& hostArg, int portArg) {
    port = portArg;
    host = hostArg;
    return setup();
}

int client::setup() {
    return fileDescriptor = Open_clientfd(const_cast<char *>(host.c_str()), port);
}
