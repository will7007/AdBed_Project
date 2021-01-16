#include "transmitter.h"
cv::Mat * transmitter::receive(int socket, int width, int height, int channels) {
    int size = width*height*channels;
    char recBuf[size];

	if(recv(socket, recBuf, size, MSG_WAITALL) == size) {
		cv::Mat *image = new cv::Mat;
		bytesToMat(recBuf,width,height,(channels == 1)).copyTo(*image);
		return image;
	}
	else { return nullptr; } //not all bytes received
}

int transmitter::send(int socket, cv::Mat image, int imgWidth, int imgHeight, int channels) {
    std::vector<char> imgBytes = matToBytes(image);
    char *bufImg = &imgBytes[0]; //this cast may not be needed
    //socket = Open_clientfd(host, port); //must already be opened beforehand
    return write(socket,bufImg,(imgWidth*imgHeight*channels));
} //FIXME stop passing by value, pass by reference

std::vector<char> transmitter::matToBytes(cv::Mat image) {
    int size = image.total() * image.elemSize();
    std::vector<char> img_bytes(size);
    img_bytes.assign(image.datastart, image.dataend);
    return img_bytes;
}

cv::Mat transmitter::bytesToMat(char * bytes,int width,int height,bool grey) {
	cv::Mat image;
    if(grey) { image = cv::Mat(height,width,CV_8UC1,bytes).clone(); }
    else { image = cv::Mat(height,width,CV_8UC3,bytes).clone(); }
    return image;
}
