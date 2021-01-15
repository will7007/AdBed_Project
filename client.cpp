#include <opencv2/opencv.hpp>
#include "csapp/csapp.h"
#include <vector>

cv::Mat bytesToMat(char * bytes,int width,int height);
std::vector<char> matToBytes(cv::Mat image);

int main(int argc, char **argv) 
{
    int clientfd, port = 111;
    char host[] = "127.0.0.1\0";
	// char buf[MAXLINE]; //unsigned vs signed: makes no difference
    rio_t rio;
    char defaultArg[] = "./lena.jpg\0";
    char * imageName = nullptr;
    cv::Mat image;

    //checking/assigning args
    if( argc != 2 )	{
	    //hardcode Lena as default
        imageName = defaultArg;
	} 
    else {
        imageName = argv[1];
    }
    image = cv::imread( imageName, 1 );
	if( !image.data )
	{
        printf( " No image data \n " );
        if( argc != 2) {
            printf(" ./lena.jpg does not exist \n");
            printf(" Supply an argument if you want to use something else \n");
        }
		return -1;
	}
    //hardcoded arguments up top
    // host = argv[1];
    // port = atoi(argv[2]);

    // cv::namedWindow( imageName, cv::WINDOW_AUTOSIZE );
    // cv::imshow( imageName, image );

    //resizing image for transfer
    cv::Size sz = image.size();
    //FIXME assuming default Lena 512^2 * 3 picture
	int imgWidth = sz.width;
	int imgHeight = sz.height;
    std::vector<char> imgBytes = matToBytes(image);
    
    char * buf = &imgBytes[0];
    clientfd = Open_clientfd(host, port);
    printf("%d bytes written\n",(int)write(clientfd,buf,(imgWidth*imgHeight*3)));
    
    //done sending--time to receive the grayscale version back
    size_t n;
    char recBuf[512*512*3];
    char picture[512*512*3];
    int current_offset = 0;

    while((n = recv(clientfd, recBuf, 512*512*3,MSG_WAITALL)) != 0) {
        //recv is like read but for sockets--we can specify helpful flags
        //like wait all, which waits for everything to come in before reading
        //(although it could still stop early due to an error)
        memcpy((void *)(picture + current_offset), recBuf,n);
        current_offset += n;
        printf("Source %p dest %p bytes %d\n", (void *)(picture + current_offset), (void *)(recBuf), (int)n);

        if(current_offset >= (512*512*3)) {
            printf("server received %d bytes\n", (int)current_offset);

            cv::Mat image = bytesToMat(recBuf,512,512);
            cv::Mat grayImage;
            cv::cvtColor( image, grayImage, cv::COLOR_BGR2GRAY );

            cv::namedWindow( "Sent Image", cv::WINDOW_AUTOSIZE );
            cv::imshow( "Sent Image", image );
            cv::namedWindow( "Received Image", cv::WINDOW_AUTOSIZE );
            cv::imshow( "Received Image", grayImage );
            cv::waitKey(0);
            break;
        }
    }

    Close(clientfd); //line:netp:echoclient:close
    exit(0);
}
/* $end echoclientmain */

std::vector<char> matToBytes(cv::Mat image)
{
    int size = image.total() * image.elemSize();
    std::vector<char> img_bytes(size);
    img_bytes.assign(image.datastart, image.dataend);
    return img_bytes;
}

cv::Mat bytesToMat(char * bytes,int width,int height)
{
	//https://stackoverflow.com/questions/33027942/opencv-convert-image-to-bytes-and-back
	cv::Mat image = cv::Mat(height,width,CV_8UC3,bytes).clone(); // make a copy
	//if we don't make a deep copy then when we leave the function scope our
	//data can get overwritten
	//cv::Mat image(height,width,CV_8UC3,bytes.data()); //wrong, shows nothing
    return image;
}