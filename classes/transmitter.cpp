#include "transmitter.h"

cv::Mat* transmitter::receive(int width, int height, int channels) {
    int size = width*height*channels;
    char *recBuf = new char[size]; //don't forget to deallocate before erasing image
    //allocated since otherwise the buffer would leave the scope and die

	if(recv(fileDescriptor, recBuf, size, MSG_WAITALL) == size) {
		// cv::Mat *image = new cv::Mat;
		// bytesToMat(recBuf,width,height,(channels == 1))->copyTo(*image); //too inneficient to copy the buffer
		return bytesToMat(recBuf,width,height,(channels == 1));
	}
	else { return nullptr; } //not all bytes received
}

int transmitter::send(cv::Mat* image, int imgWidth, int imgHeight, int channels) {
    return write(fileDescriptor,matToBytes(image),(imgWidth*imgHeight*channels));
}

//https://stackoverflow.com/questions/33027942/opencv-convert-image-to-bytes-and-back

char* transmitter::matToBytes(cv::Mat* image) {
    char *imgBytes = (char *)(const_cast<uchar *>(image->datastart));
    return imgBytes;
}

cv::Mat* transmitter::bytesToMat(char * bytes,int width,int height,bool grey) {
	cv::Mat* image;
    if(grey) { image = new cv::Mat(height,width,CV_8UC1,bytes); }
    else { image = new cv::Mat(height,width,CV_8UC3,bytes); }
    return image;
}
