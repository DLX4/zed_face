/*
 * =====================================================================================
 *
 *       Filename:  face_detection.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *
 *       Revision:  none
 *       Compiler:  arm-xilinx-linux-gnueabi-g++
 *
 *         Author:  DLX
 * =====================================================================================
 */

#include "opencv2/opencv.hpp"
#include "opencv/cxcore.h"

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <vector>
#include <opencv2/contrib/contrib.hpp>

#include <opencv2/core/core.hpp>
//#include "opencv2/high"
//#include "opencv/cv.h"
//#include "opencv/highgui.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>
#include <sys/stat.h>
#include <iostream>
#include <algorithm>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
using namespace cv;

using std::cout;
using std::endl;


static CvMemStorage* storage = cvCreateMemStorage();
static CvHaarClassifierCascade* cascade = 0;

void detect_and_draw( IplImage* image ,IplImage* image_old,int minX,int minY);
bool less_by_x(const cv::Point& lhs, const cv::Point& rhs)
{
  return lhs.x < rhs.x;
}

bool less_by_y(const cv::Point& lhs, const cv::Point& rhs)
{
  return lhs.y < rhs.y;
}

const char* cascade_name =
	"haarcascade_frontalface_alt.xml";
/*    "haarcascade_profileface.xml";*/
//static int init_flag = 0;
int sockfd, portno, n;
struct sockaddr_in serv_addr;
char bufferImage[1024] = "hello dlx----------\n";
char buffer[32];
	 
char *serverWelcome = "welcome";
char *serverBye = "bye";

int face_main()
{
	CvCapture* capture = 0;
	IplImage *image =0;
	IplImage *frame, *frame_copy = 0;

  Mat image_bmp;
  IplImage tmp;
  IplImage tmp2;
  //dlx modify
  Mat image_bmp_back;
  IplImage tmp_back;
  IplImage *image_back =0;
  IplImage *image_dst =0;
  IplImage *image_dst_gray =0;
  IplImage *image_dst_gray_canny =0;
  IplImage *image_skin =0;
  //	int optlen = strlen("--cascade=");


  //if(init_flag == 0)
  //{

    //portno = atoi(argv[2]);
    portno = 9001;


    //bzero((char *) &serv_addr, sizeof(serv_addr));
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("192.168.1.40");
    serv_addr.sin_port = htons(portno);



	cascade_name = "./haarcascade_frontalface_alt2.xml";
		//opencv装好后haarcascade_frontalface_alt2.xml的路径,
		//也可以把这个文件拷到你的工程文件夹下然后不用写路径名cascade_name= "haarcascade_frontalface_alt2.xml";
	cascade = (CvHaarClassifierCascade*) cvLoad(cascade_name,0,0,0);//Problem happens

	if( !cascade )
	{
		fprintf( stderr, "ERROR: Could not load classifier cascade\n" );
		fprintf( stderr,
			"Usage: facedetect --cascade=\"<cascade_path>\" [filename|camera_index]\n" );
		return -1;
	}

	//capture a pictrue from camera
	//capture = cvCaptureFromCAM(-1);//CV_CAP_ANY );
	//if captured successfully, save the image as capture.jpg , detect and draw

	//cvSetCaptureProperty (capture,CV_CAP_PROP_FRAME_WIDTH,320);
	//cvSetCaptureProperty (capture,CV_CAP_PROP_FRAME_HEIGHT,240);
	if( 0 )
  {
    for(;;)
    {
      if( !cvGrabFrame( capture ))
          break;
      frame = cvRetrieveFrame( capture );
      if( !frame )
          break;
      if( !frame_copy )
         // frame_copy = cvCreateImage( cvSize(frame->width,frame->height),IPL_DEPTH_8U, frame->nChannels );

         frame_copy = cvCreateImage( cvSize(320,240),IPL_DEPTH_8U, frame->nChannels );
//copy frame to frame_copy and ensure frame_copy is not upside down
      if( frame->origin == IPL_ORIGIN_TL )
	       cvResize(frame,frame_copy,CV_INTER_LINEAR);
          //cvCopy( frame, frame_copy, 0 );
      else
      {
         cvResize(frame,frame_copy,CV_INTER_LINEAR);
         cvFlip( frame, frame_copy, 0 );
      }
  		//save captured image
  		const char* filename =  (char*)"./capture.bmp";// local path
  		cvSaveImage(filename,frame_copy);
  		//detect and draw
            //detect_and_draw( frame_copy );

 	  }

    cvReleaseImage( &frame_copy );
    cvReleaseCapture( &capture );
  }

	else
	{
		//const char* filename =  (char*)"./lena.bmp";// local path
//const char* filename =  (char*)"./tmp.bmp";// local path
      const char* filename =  "/temp.bmp";
      image_bmp = imread(filename,1);
      tmp = IplImage(image_bmp);
      image = &tmp;
      if(!image)
      {
      			printf("load image fault!\n");
      }

      Mat output_mask;
      Mat output_image;
      Mat mask;

    	Mat skinCrCbHist = Mat::zeros(Size(256, 256), CV_8UC1);
    	ellipse(skinCrCbHist, Point(113, 155.6), Size(23.4, 15.2), 43.0, 0.0, 360.0, Scalar(255, 255, 255), -1);

    	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1) );
    	Mat ycrcb_image;
    	output_mask = Mat::zeros(image_bmp.size(), CV_8UC1);
    	cvtColor(image_bmp, ycrcb_image, CV_BGR2YCrCb); //首先转换成到YCrCb空间

    	for(int i = 0; i < image_bmp.rows; i++) //利用椭圆皮肤模型进行皮肤检测
    	{
    	    uchar* p = (uchar*)output_mask.ptr<uchar>(i);
    	    Vec3b* ycrcb = (Vec3b*)ycrcb_image.ptr<Vec3b>(i);
    	    for(int j = 0; j < image_bmp.cols; j++)
    		  {
    			if(skinCrCbHist.at<uchar>(ycrcb[j][1], ycrcb[j][2]) > 0)
    				p[j] = 255;
    		  }
    	}

    	morphologyEx(output_mask,output_mask,MORPH_CLOSE,element);


    	vector< vector<Point> > contours;   // 轮廓
    	vector< vector<Point> > filterContours; // 筛选后的轮廓
    	vector< Vec4i > hierarchy;    // 轮廓的结构信息
    	contours.clear();
    	hierarchy.clear();
    	filterContours.clear();

    	findContours(output_mask, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
            	typedef std::vector<cv::Point>::const_iterator PointIt;
            	int minX = 640;
            	int maxX = 0;
            	int minY = 480;
            	int maxY = 0;
    	// 去除伪轮廓
    	for (size_t i = 0; i < contours.size(); i++)
    	{
      		if (fabs(contourArea(Mat(contours[i]))) > 3000/*&&fabs(arcLength(Mat(contours[i]),true))<2000*/)  //判断手进入区域的阈值
      		{
        		filterContours.push_back(contours[i]);  printf("++ %d\n",fabs(contourArea(Mat(contours[i]))));
        		//dlx add ----get the max,min of x,y   ;http://stackoverflow.com/questions/18438706/finding-min-and-max-of-a-point-vector
        		//std::pair<PointIt, PointIt> mmx
        		PointIt mmx = std::min_element(contours[i].begin(),
        				      contours[i].end(),
        				      less_by_x);
      			printf("min x:%d  \n",mmx->x);
      			if(minX > mmx->x)
      			{
      				minX = mmx->x;
      			}

      			mmx = std::max_element(contours[i].begin(),
      				      contours[i].end(),
      				      less_by_x);
      			printf("max x:%d  \n",mmx->x);
      			if(maxX < mmx->x)
      			{
      				maxX = mmx->x;
      			}

      			mmx = std::min_element(contours[i].begin(),
      				      contours[i].end(),
      				      less_by_y);
        		printf("min y:%d  \n",mmx->y);
        		if(minY > mmx->y)
      			{
      				minY = mmx->y;
      			}

        		mmx = std::max_element(contours[i].begin(),
        				      contours[i].end(),
        				      less_by_y);
        		printf("min y:%d  \n",mmx->y);
      			if(maxY < mmx->y)
      			{
      				maxY = mmx->y;
      			}

    		}

    	}


      printf("minX=%d minY=%d maxX=%d maxY=%d \n\n",minX,minY,maxX,maxY);

      if(minX > maxX || minY > maxY)
      {
          minX = 0;minY = 0;
          maxX = 640 - 1;maxY = 480 -1;
      //return 0;

      }

      output_mask.setTo(0);
      drawContours(output_mask, filterContours, -1, Scalar(255,0,0), CV_FILLED); //8, hierarchy);

      image_bmp.copyTo(output_image, output_mask);
              //cut the ROI part
      cvSetImageROI(image,cvRect(minX,minY,maxX-minX,maxY-minY));
      //tmp2 = IplImage(image);
      cvSaveImage("/saveROI.bmp",image);
      tmp2 = IplImage(output_image);
      cvSaveImage("/save_skin_tmp2.bmp",&tmp2);



      //dlx mod 5.3
      //detect_and_draw( &tmp2,image);
      //cvReleaseImage( &image );
      IplImage* ROI_img = cvCreateImage(cvGetSize(image),image->depth,image->nChannels);
      cvCopy(image,ROI_img,NULL);
      cvResetImageROI(image);
      detect_and_draw( ROI_img,image,minX,minY);
      cvReleaseImage( &ROI_img );
	}

	cvReleaseHaarClassifierCascade( &cascade );

	return 0;
}

void detect_and_draw( IplImage* img ,IplImage* img_old,int minX,int minY)
{
	static CvScalar colors = CV_RGB( 255, 0, 0);
	printf("detect=++++++++++++++++++++++++++++++==\n");
	double scale = 1.3;
	IplImage* gray = cvCreateImage( cvSize(img->width,img->height), 8, 1 );
	IplImage* small_img = cvCreateImage( cvSize( cvRound (img->width/scale),cvRound (img->height/scale)),8, 1 );


	cvCvtColor( img, gray, CV_BGR2GRAY );
	//dlx-----
	//cvSaveImage("/save_gray.bmp",gray);
	double t = (double)cvGetTickCount();
	cvResize( gray, small_img, CV_INTER_LINEAR );
	t = (double)cvGetTickCount() - t;
	printf( "cvResize time = %gms\n", t/((double)cvGetTickFrequency()*1000.) );
	t = (double)cvGetTickCount();
	cvEqualizeHist( small_img, small_img );

	t = (double)cvGetTickCount() - t;
	printf( "cvEqualizeHist time = %gms\n", t/((double)cvGetTickFrequency()*1000.) );
	printf("detect=++++++++++++++++++++++++++++++==0.1\n");
	cvClearMemStorage( storage );
	if( cascade )
	{
		int i;
		t = (double)cvGetTickCount();
		CvSeq* faces = cvHaarDetectObjects( small_img, cascade, storage,1.2, 2, CV_HAAR_DO_CANNY_PRUNING,cvSize(30, 30) );
		t = (double)cvGetTickCount() - t;
		printf( "detection time = %gms\n", t/((double)cvGetTickFrequency()*1000.) );

		for( i = 0; i < (faces ? faces->total : 0); i++ )
		{
			CvRect* r = (CvRect*)cvGetSeqElem( faces, i );
			CvPoint center;
			int radius;
			center.x = cvRound((r->x + r->width*0.5)*scale + minX);
			center.y = cvRound((r->y + r->height*0.5)*scale + minY);
			radius = cvRound((r->width + r->height)*0.25*scale);
			cvCircle( img_old, center, radius, colors, 3, 8, 0 );
			printf("heloo------------------------------------people\n");


			// send the image to server
			Mat ROI_mat = Mat(img);
			Mat faceImg = ROI_mat(*r).clone();

			sprintf(bufferImage,"rows=%d , cols=%d , image.total()=%d \n",faceImg.rows,faceImg.cols,faceImg.total());
	      faceImg = (faceImg.reshape(0,1));
	      
	      portno = 9001;
	      sockfd = socket(AF_INET, SOCK_STREAM, 0);
	      if (sockfd < 0)
				printf("ERROR opening socket\n");

	      //bzero((char *) &serv_addr, sizeof(serv_addr));
	      memset(&serv_addr, 0, sizeof(serv_addr));
	      serv_addr.sin_family = AF_INET;
	      serv_addr.sin_addr.s_addr = inet_addr("192.168.1.40");
	      serv_addr.sin_port = htons(portno);
	      if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
		  		perror("ERROR connecting1\n");
		  		
		  		
	      printf("socket fd=%d\n",sockfd);
	
			bzero(buffer,32);
			n = recv(sockfd,buffer,8,0);//server welcome
			printf("recv n =%d\n",n);
			if(0 == strcmp(serverWelcome,buffer))
			{
				printf("recv server hello\n");
				printf(buffer);
				bzero(buffer,32);
			}else{
				close(sockfd);
				break;
			}
	
			printf("rows =%d , cols = %d ,image.total() = %d \n",faceImg.rows,faceImg.cols,faceImg.total());
		    //n = send(sockfd,image.data,image.total()*image.channels(),0);
		    n = send(sockfd,bufferImage,strlen(bufferImage),0);
		    	printf("send n =%d\n",n);
		    if (n < 0) 
			 perror("ERROR writing to socket");
			 
		    n = send(sockfd,faceImg.data,faceImg.total()*faceImg.elemSize(),0);
		    printf("send n =%d\n",n);
			if (n < 0) 
				perror("ERROR writing to socket");
			//char byeBuffer[5]	= "bye";
			 n = recv(sockfd,buffer,4,0);
			 if(0 == strcmp(serverBye,buffer))
			 {
			 	printf("recv server bye\n");
				printf(buffer);
			 }else{
				close(sockfd);
				break;
			}
		    close(sockfd);

		}
		printf("detect=++++++++++++++++++++++++++++++1\n");
	}
	printf("==============================================\n");
			//const char* filename =  (char*)"./detection.bmp";
	cvSaveImage("/tmp_detection.bmp",img_old);//save the result as "detection.bmp"
	cvReleaseImage( &gray );
	cvReleaseImage( &small_img );


}
