#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;


//trackbar variables
int sliderPos;
const int sliderMax = 300;	
double alpha;
double beta;
int otTh = 20;

Mat frame;
void on_trackbar( int, void* )
{

	if(sliderPos < 1){
		otTh = 1;
	}else{
		otTh = sliderPos;
	}
  imshow( "frame", frame );
}

int main(int argc, char* argv[])
{
	VideoCapture capture(0);
	namedWindow("frame", WINDOW_KEEPRATIO);
	namedWindow("otsu", WINDOW_KEEPRATIO);
	if(!capture.isOpened()) //Check if success
		return -1;
	

	char TrackbarName[50];
  	sprintf( TrackbarName, "Posição x %d", sliderPos );
	createTrackbar( TrackbarName, "frame", &sliderPos, sliderMax, on_trackbar );

	for(;;){
		Mat otsu, gray;
		capture >> frame;
		cvtColor(frame, gray, COLOR_BGR2GRAY);
		medianBlur(gray, gray, 5);
		medianBlur(gray, gray, 5);
		

		for (int r = 1; r < 8; r++)
		{
    		Mat kernel = getStructuringElement(MORPH_RECT, Size(3*r+1, 3*r+1));
    		morphologyEx(gray, gray, MORPH_CLOSE, kernel);
    		morphologyEx(gray, gray, MORPH_OPEN, kernel);
		}


    
		cv::threshold(gray, otsu, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

		imshow("otsu",otsu);

		vector<Vec3f> circles;
		vector<int> centros;
    char str[200];
 		HoughCircles(otsu, circles, HOUGH_GRADIENT,1 , 50 , 120 , otTh); // 19
  
  		for( size_t i = 0; i < circles.size(); i++ )
  		{

      		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
      		int radius = cvRound(circles[i][2]);
      		centros.push_back(cvRound(circles[i][0]));
      		centros.push_back(cvRound(circles[i][1]));
     		 // circle center
      		circle( frame, center, 3, Scalar(0,255,0), -1, 4, 0 );
      		// circle outline
      		circle( frame, center, radius, Scalar(0,0,255), 2, LINE_AA, 0 );

  		}
  		if(circles.size() == 3){
      		
      		float dist,a,b,c;
			
      		line(frame,Point(centros[0],centros[1]),Point(centros[2],centros[3]),Scalar(255,0,0), 4, LINE_AA);
      		dist = sqrt(pow(centros[0]-centros[2],2) + pow(centros[1]-centros[3],2));
      		a = dist;
      		sprintf(str,"%.2f", dist);
      		putText(frame, str, Point(centros[0],centros[1]), FONT_HERSHEY_PLAIN,3, Scalar(0,255,0),2);

       		line(frame,Point(centros[2],centros[3]),Point(centros[4],centros[5]),Scalar(255,0,0), 4, LINE_AA);
       		dist = sqrt(pow(centros[2]-centros[4],2) + pow(centros[3]-centros[5],2));
       		b = dist;
      		sprintf(str,"%.2f", dist);
      		putText(frame, str, Point(centros[2],centros[3]), FONT_HERSHEY_PLAIN,3, Scalar(0,255,0),2);

       		line(frame,Point(centros[4],centros[5]),Point(centros[0],centros[1]),Scalar(255,0,0), 4, LINE_AA);
       		dist = sqrt(pow(centros[4]-centros[0],2) + pow(centros[5]-centros[1],2));
       		c = dist;
      		sprintf(str,"%.2f", dist);
      		putText(frame, str, Point(centros[4],centros[5]), FONT_HERSHEY_PLAIN,3, Scalar(0,255,0),2);

      		//Herons formula
      		float p = (a+b+c)/2;
      		float Area = sqrt(p*((p-a)*(p-b)*(p-c)));

      		sprintf(str,"Area: %.2f", Area);
      		putText(frame, str, Point(10,80), FONT_HERSHEY_PLAIN,3, Scalar(80,255,100),3);
  
       	}
      	centros.clear();
        sprintf(str,"Rauan Pires - 14103318");
        putText(frame, str, Point(10,40), FONT_HERSHEY_PLAIN,3, Scalar(0,255,255),3); 
  
  	printf("%d\n", circles.size());
		imshow("frame",frame);
		if(waitKey(30) >= 0) break;
	}

	return 0;
}