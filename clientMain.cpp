#include "classes/transmitter.h"
#include "classes/client.h"
#include <opencv2/opencv.hpp>

int main(int argc, char **argv) 
{
    cv::Mat image = cv::imread("./lena.jpg", 1);
	client clientTransmitter = client();
	
    if( !image.data ){
        printf( " No image data \n " );
        if( argc != 2) {
            printf(" ./lena.jpg does not exist \n");
            // printf(" Supply an argument if you want to use something else \n");
        }
		return -1;
	}

	int imgWidth = 512;
	int imgHeight = imgWidth;
	int bytesWritten;

	if((bytesWritten = clientTransmitter.send(clientTransmitter.getFileDescriptor(), image, imgWidth, imgHeight, 3)) >= imgHeight*imgWidth*3) {
		printf("%d bytes written\n", bytesWritten);
	}
    
	cv::Mat * receivedImage = clientTransmitter.receive(clientTransmitter.getFileDescriptor(), imgWidth, imgHeight, 1); 
	if(receivedImage != nullptr && receivedImage->data) { //possibly redundant
		clientTransmitter.show(&image,receivedImage);
		delete receivedImage;
	}
	else { printf("Error: failed to receive image back from server"); }

    Close(clientTransmitter.getFileDescriptor()); //line:netp:echoclient:close
    exit(0);
}