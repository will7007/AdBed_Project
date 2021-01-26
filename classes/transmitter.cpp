#include "transmitter.h"

std::pair<transmitter::transmitSize*, cv::Mat*> transmitter::receive(int fileDescriptor) {
    transmitSize *size = receiveSize(fileDescriptor);
    cv::Mat* image = nullptr;
    if(size != nullptr) {
        image = receiveImage(size, fileDescriptor);
        // delete size;
    }
    return std::make_pair(size,image);
}

int transmitter::send(cv::Mat* image, int fileDescriptor, uint8_t operations) {
    return sendSize(image, fileDescriptor, operations) ? sendImage(image, fileDescriptor) : 0;
}

transmitter::transmitSize* transmitter::receiveSize(int fileDescriptor) {
    transmitSize *recBuf = new transmitSize;
    if(recv(fileDescriptor, recBuf, 10, MSG_WAITALL) == 10) {
        return recBuf;
    }
    else {
        delete recBuf;
        return nullptr;
    }
}

bool transmitter::sendSize(cv::Mat* image, int fileDescriptor, uint8_t operations) {
    cv::Size size = image->size();
    transmitSize sendBuf;
    sendBuf.width = size.width;
    sendBuf.height = size.height;
    sendBuf.channels = image->channels();
    sendBuf.operations = operations;
    return (write(fileDescriptor,&sendBuf,10) == 10);
}

cv::Mat* transmitter::receiveImage(int width, int height, int channels, int fileDescriptor) {
    int size = width*height*channels;
    char *recBuf = new char[size];
    //don't forget to deallocate before erasing image

	if(recv(fileDescriptor, recBuf, size, MSG_WAITALL) == size) {
		return bytesToMat(recBuf,width,height,(channels == 1));
	}
	else { return nullptr; }
}

cv::Mat* transmitter::receiveImage(transmitter::transmitSize *sizeStruct, int fileDescriptor) {
    int width = sizeStruct->width;
    int height = sizeStruct->height;
    int channels = sizeStruct->channels;
    return receiveImage(width, height, channels, fileDescriptor);
}

int transmitter::sendImage(cv::Mat* image, int fileDescriptor) {
    cv::Size size = image->size();
    return write(fileDescriptor,matToBytes(image),
                    (size.width*size.height*image->channels()));
}

char* transmitter::matToBytes(cv::Mat* image) {
    return (char *)(image->datastart);
}

cv::Mat* transmitter::bytesToMat(char * bytes,int width,int height,bool grey) {
	cv::Mat* image;
    if(grey) { image = new cv::Mat(height,width,CV_8UC1,bytes); }
    else { image = new cv::Mat(height,width,CV_8UC3,bytes); }
    return image;
}
