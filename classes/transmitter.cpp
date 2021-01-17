#include "transmitter.h"

cv::Mat* transmitter::receive(int channels) {
    // transmitSize *size = receiveSize();
    cv::Mat* returnValue = nullptr;
    // if(size != nullptr) {
        // returnValue = receiveImage(size);
    if(true) {
        returnValue = receiveImage(512,512,channels);
        // delete size;
    }
    return returnValue;
}

int transmitter::send(cv::Mat* image) {
    // return sendSize(image) ? sendImage(image) : 0;
    return true ? sendImage(image) : 0;
}

transmitter::transmitSize* transmitter::receiveSize() {
    transmitSize *recBuf = new transmitSize;
    if(recv(fileDescriptor, &recBuf, 10, MSG_WAITALL) == 10) {
        return recBuf;
    }
    else {
        delete recBuf;
        return nullptr;
    }
}

bool transmitter::sendSize(cv::Mat* image) {
    cv::Size size = image->size();
    transmitSize sendBuf;
    sendBuf.width = size.width;
    sendBuf.height = size.height;
    sendBuf.channels = image->channels();
    return (write(fileDescriptor,&sendBuf,10) == 10);
}

cv::Mat* transmitter::receiveImage(int width, int height, int channels) {
    int size = width*height*channels;
    char *recBuf = new char[size];
    //don't forget to deallocate before erasing image

	if(recv(fileDescriptor, recBuf, size, MSG_WAITALL) == size) {
		return bytesToMat(recBuf,width,height,(channels == 1));
	}
	else { return nullptr; }
}

cv::Mat* transmitter::receiveImage(transmitter::transmitSize *sizeStruct) {
    int width = sizeStruct->width;
    int height = sizeStruct->height;
    int channels = sizeStruct->channels;
    return receiveImage(width, height, channels);
}

int transmitter::sendImage(cv::Mat* image) {
    cv::Size size = image->size();
    printf("Sent width: %d\n", size.width);
    return write(fileDescriptor,matToBytes(image),
                    (size.width*size.height*image->channels()));
}

//https://stackoverflow.com/questions/33027942/opencv-convert-image-to-bytes-and-back

char* transmitter::matToBytes(cv::Mat* image) {
    return (char *)(image->datastart);
}

cv::Mat* transmitter::bytesToMat(char * bytes,int width,int height,bool grey) {
	cv::Mat* image;
    if(grey) { image = new cv::Mat(height,width,CV_8UC1,bytes); }
    else { image = new cv::Mat(height,width,CV_8UC3,bytes); }
    return image;
}
