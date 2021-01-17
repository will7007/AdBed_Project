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

	if((bytesWritten = clientTransmitter.send(&image)) >= imgHeight*imgWidth*3) {
		printf("Client sent %d bytes\n", bytesWritten);
	}
    
	cv::Mat * receivedImage = clientTransmitter.receive(1); 
	if(receivedImage->data) {
		printf("Image received\n");
		clientTransmitter.show(&image,receivedImage);
		delete receivedImage->datastart;
		delete receivedImage;
	}
	else { printf("Error: failed to receive image back from server"); }

    Close(clientTransmitter.getFileDescriptor()); //just an extra precaution
    exit(0);
}