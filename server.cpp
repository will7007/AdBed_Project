#include "csapp/csapp.h"
#include <opencv2/opencv.hpp>
#include <vector>

void display(int connfd);
std::vector<char> matToBytes(cv::Mat image);
cv::Mat bytesToMat(char * bytes,int width,int height);

int main(int argc, char **argv) 
{
    int listenfd, connfd, port;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    struct hostent *hp;
    char *haddrp;
    if (argc != 2) { port = 111; }
    else { port = atoi(argv[1]); }

    listenfd = Open_listenfd(port);
    while (1) {
	clientlen = sizeof(clientaddr);
	connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);

	/* Determine the domain name and IP address of the client */
	hp = Gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, 
			   sizeof(clientaddr.sin_addr.s_addr), AF_INET);
	haddrp = inet_ntoa(clientaddr.sin_addr);
	printf("server connected to %s (%s)\n", hp->h_name, haddrp);

	display(connfd);
	Close(connfd);
    }
    exit(0);
}

void display(int connfd) 
{
    size_t n; 
    char buf[512*512*3]; 
    rio_t rio;
    char picture[512*512*3];
    int current_offset = 0;

    // Rio_readinitb(&rio, connfd);
    // while((n = read(connfd, buf, 512*512*3)) != 0) { //line:netp:echo:eof
    while((n = recv(connfd, buf, 512*512*3,MSG_WAITALL)) != 0) {
        //recv is like read but for sockets--we can specify helpful flags
        //like wait all, which waits for everything to come in before reading
        //(although it could still stop early due to an error)
        memcpy((void *)(picture + current_offset), buf,n);
        current_offset += n;
        printf("Source %p dest %p bytes %d\n", (void *)(picture + current_offset), (void *)(buf), (int)n);

        if(current_offset >= (512*512*3)) {
            printf("server received %d bytes\n", (int)current_offset);

            cv::Mat image = bytesToMat(buf,512,512);
            cv::Mat grayImage;
            cv::cvtColor( image, grayImage, cv::COLOR_BGR2GRAY );
            
            std::vector<char> imgBytes = matToBytes(grayImage);
            char * sendBuf = &imgBytes[0];
            printf("%d bytes written\n",(int)write(connfd,sendBuf,(512*512)));
        }
    }
}

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
