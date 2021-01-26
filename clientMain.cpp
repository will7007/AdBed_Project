#include "classes/transmitter.h"
#include "classes/client.h"
#include <opencv2/opencv.hpp>
#include "./colors.h"

int main(int argc, char **argv) 
{
    cv::Mat image;
	client clientTransmitter = client();
	int bytesWritten = 0;
	bool showImage = true;
	uint8_t operations = 0x1; //grayscale by default
	
    switch(argc) {
		case 1:
			image = cv::imread("./lena.jpg", 1);
			if(!image.data) {
				fprintf(stderr, "Default picture ./lena.jpg does not exist\n");
				fprintf(stderr, "Provide path to a picture as an argument\n");
				exit(0);
			}
			break;
		case 4:
			showImage = false;
			// printf("4th argument present: supressing image\n");
			//any 4th arg disables showing the image
			//flowthrough
		case 3:
			// printf("3th argument present: selecting operations\n");
			operations = atoi(argv[2]);
			if(!operations) {
				printf(FGRN("Sorry, shutting down the server from the client isn't supported yet. "));
				printf(FGRN("How about I ask for grayscale instead?\n"));
				operations++;
			}
			//flowthrough
		case 2:
			// printf("2nd argument present: custom image\n");
			image = cv::imread(argv[1], 1);
			if(!image.data) {
				fprintf(stderr, "No image data in provided file\n");
				exit(0);
			}
			break;
		default:
			fprintf(stderr, "Too many arguments\n");
			exit(0);
			break;
	}

	int imgWidth = image.size().width;
	int imgHeight = image.size().height;
	int channels = image.channels();

	if(operations && (bytesWritten = clientTransmitter.send(&image, operations)) >=
			imgHeight*imgWidth*channels) {
		printf(FGRN("Client sent %d bytes\n"), bytesWritten);
	} else if(bytesWritten == 0 && operations) {
		fprintf(stderr, FGRN("Error: No bytes sent to server\n"));
	} else { printf(FGRN("Server asked to shutdown again as a workaround\n")); }
    
	if(operations != 0x0) {
		cv::Mat * receivedImage = clientTransmitter.receive(); 
		if(receivedImage != nullptr && receivedImage->data) {
			printf(FGRN("Image received\n"));
			if(showImage) {	clientTransmitter.show(&image,receivedImage); }
			delete receivedImage->datastart;
			delete receivedImage;
		}
		else {
			printf(FGRN("Error: failed to receive image back from server\n"));
			}
	} else {
		printf(FGRN("Not expecting an image back since no operations were requested\n"));
	}

    Close(clientTransmitter.getFileDescriptor()); //just an extra precaution
    exit(0);
}
