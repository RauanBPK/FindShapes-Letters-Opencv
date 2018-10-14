#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;


int main(int argc, char* argv[])
{
	VideoCapture capture(0);
	namedWindow("frame", WINDOW_KEEPRATIO);
	// namedWindow("canny", WINDOW_KEEPRATIO);
	// namedWindow("gray", WINDOW_KEEPRATIO);
	if(!capture.isOpened()) //Check if success
		return -1;

	Mat gray;

	for(;;){
		Mat frame, edge, gray;
		capture >> frame;
		namedWindow("frame", WINDOW_KEEPRATIO);

		imshow("frame",frame);
		if(waitKey(30) >= 0) break;
	}

	return 0;
}