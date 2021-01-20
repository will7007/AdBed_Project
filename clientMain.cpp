#include "classes/transmitter.h"
#include "classes/client.h"
#include <opencv2/opencv.hpp>

int main(int argc, char **argv) 
{
    cv::Mat image;
	client clientTransmitter = client();
	int bytesWritten = 0;
	bool showImage = true;
	
    switch(argc) {
		case 1:
			image = cv::imread("./lena.jpg", 1);
			if(!image.data) {
				fprintf(stderr, "Default picture ./lena.jpg does not exist\n");
				fprintf(stderr, "Provide path to a picture as an argument\n");
				exit(0);
			}
			break;
		case 3:
			showImage = false;
			//any third arg disables showing the image
			//flowthrough
		case 2:
			image = cv::imread(argv[1], 1);
			if(!image.data) {
				fprintf(stderr, "No image data in provided file\n");
				exit(0);
			}
			break;
		default:
			fprintf(stderr, "Too many arguments (coming soon)\n");
			exit(0);
			break;
	}

	int imgWidth = image.size().width;
	int imgHeight = image.size().height;
	int channels = image.channels();

	if((bytesWritten = clientTransmitter.send(&image)) >=
			imgHeight*imgWidth*channels) {
		printf("Client sent %d bytes\n", bytesWritten);
	}
    
	cv::Mat * receivedImage = clientTransmitter.receive(); 
	if(receivedImage != nullptr && receivedImage->data) {
		printf("Image received\n");
		if(showImage) {	clientTransmitter.show(&image,receivedImage); }
		delete receivedImage->datastart;
		delete receivedImage;
	}
	else { printf("Error: failed to receive image back from server\n"); }

    Close(clientTransmitter.getFileDescriptor()); //just an extra precaution
    exit(0);
}
