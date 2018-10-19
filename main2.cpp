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
	// namedWindow("otsu", WINDOW_KEEPRATIO);
	// namedWindow("diff", WINDOW_KEEPRATIO);
	if(!capture.isOpened()) //Check if success
		return -1;

	Mat gray;

	//-- IMAGENS PARA ACHAR --//
	Mat A = imread("./A.jpg", IMREAD_GRAYSCALE);
	Mat B = imread("./B.jpg", IMREAD_GRAYSCALE);
	Mat SPHERE = imread("./SPHERE.jpg", IMREAD_GRAYSCALE);
	Mat SQUARE = imread("./SQUARE.jpg", IMREAD_GRAYSCALE);
	Mat T1 = imread("./T1.jpg", IMREAD_GRAYSCALE);
	Mat T2 = imread("./T2.jpg", IMREAD_GRAYSCALE);
	Mat T3 = imread("./T3.jpg", IMREAD_GRAYSCALE);

	int numImgsToLook = 7; // Melhorar isso -  strongtyped fica feio
	//------------------------//

	int fontSize = 2;
	int fontThickness = 2;

	for(;;){
		
		Mat frame, otsu, gray;
		capture >> frame;
		cvtColor(frame, gray, COLOR_BGR2GRAY);
		medianBlur(gray, gray, 5);
		medianBlur(gray, gray, 5);		
		
		for (int r = 1; r < 5; r++)
		{
			Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(2*r + 1, 2*r +1));

			morphologyEx(gray, gray, MORPH_CLOSE, kernel);
			morphologyEx(gray, gray, MORPH_OPEN, kernel);
		}

		
		threshold(gray, gray, 30, 255, cv::THRESH_BINARY | THRESH_OTSU);
		// imshow("otsu",gray);

		vector<vector< Point > >conts;
		// vector<Mat> cropped;

		findContours(gray, conts, RETR_LIST  , CHAIN_APPROX_NONE);
		unsigned int numberOfObjects=0;

		vector<RotatedRect> minRect( conts.size() );
		for(int i=0; i<conts.size(); i++)
		{
			if(conts.at(i).size() > 150 && conts.at(i).size() < 2000)
			{
				//drawContours(frame, conts, i, Scalar(0,0,255), 2, 8); //desenha contorno
				numberOfObjects++;


				// Bounding box pra fazer o footprint - Gira junto
				
				Mat M, rotated;
				Mat cropped;
				

				minRect[i] = minAreaRect( Mat(conts[i]) );
				
				float angle = minRect[i].angle;
        		Size rect_size = minRect[i].size;
        		if (minRect[i].angle < -45.) {
            		angle += 90.0;
            		swap(rect_size.width, rect_size.height);
        		} 
        		M = getRotationMatrix2D(minRect[i].center, angle, 1.0);
				
				warpAffine(gray, rotated, M, gray.size(), INTER_CUBIC);
				//warpAffine(frame, rotated, M, frame.size(), INTER_LINEAR);
				getRectSubPix(rotated, rect_size, minRect[i].center, cropped);

				Point2f rect_points[4]; minRect[i].points( rect_points );

				// desenha bunding boxes
				for( int j = 0; j < 4; j++ ){
					line( frame, rect_points[j], rect_points[(j+1)%4], Scalar(255,0,0), 4, 8 );
					
				}

				//-- Pra pegar os itens da tela - meio gambi
				// resize(cropped, cropped, Size(100, 100), 0, 0, INTER_CUBIC); 
				// string result;
				// result = "./cropped" + std::to_string(numberOfObjects) + ".jpg";
				// imwrite(result,cropped);
				

				int cropSize = 100;
				resize(cropped, cropped, Size(cropSize, cropSize), 0, 0, INTER_CUBIC);
				Mat diff;
				double min = 999999999999;
				int minShape = 0;
				double s;
				char str[200];

				absdiff(cropped, A, diff);
				s = cv::sum( diff )[0];
				if(s < min){
					min = s;
					minShape = 1;
				}
				absdiff(cropped, B, diff);
				s = cv::sum( diff )[0];
				if(s < min){
					min = s;
					minShape = 2;
				}
				absdiff(cropped, SPHERE, diff);
				s = cv::sum( diff )[0];
				if(s < min){
					min = s;
					minShape = 3;
				}
				absdiff(cropped, SQUARE, diff);
				s = cv::sum( diff )[0];
				if(s < min){
					min = s;
					minShape = 4;
				}
				absdiff(cropped, T1, diff);
				s = cv::sum( diff )[0];
				if(s < min){
					min = s;
					minShape = 5;
				}
				absdiff(cropped, T2, diff);
				s = cv::sum( diff )[0];
				if(s < min){
					min = s;
					minShape = 6;
				}
				absdiff(cropped, T3, diff);
				s = cv::sum( diff )[0];
				if(s < min){
					min = s;
					minShape = 7;
				} 


				float fitness = 0;
				fitness = 1 - ((1*min)/(cropSize*cropSize*255));

				if(fitness < 0.65){ // Se o fitness nao for bom, figura nao identificada
					minShape = 0;
				}
				switch(minShape){
					case 1:
					sprintf(str,"A (%.4f)", fitness );
					putText(frame, str, minRect[i].center, FONT_HERSHEY_PLAIN,fontSize, Scalar(0,255,100),fontThickness);
					break;
					case 2:
					sprintf(str,"B (%.4f)", fitness );
					putText(frame, str, minRect[i].center, FONT_HERSHEY_PLAIN,fontSize, Scalar(0,255,100),fontThickness);

					break;
					case 3:
					sprintf(str,"SPHERE (%.4f)", fitness );
					putText(frame, str, minRect[i].center, FONT_HERSHEY_PLAIN,fontSize, Scalar(0,255,100),fontThickness);

					break;
					case 4:
					sprintf(str,"SQUARE (%.4f)", fitness );
					putText(frame, str, minRect[i].center, FONT_HERSHEY_PLAIN,fontSize, Scalar(0,255,100),fontThickness);

					break;
					case 5:
					sprintf(str,"TRIANGLE (%.4f)", fitness );
					putText(frame, str, minRect[i].center, FONT_HERSHEY_PLAIN,fontSize, Scalar(0,255,100),fontThickness);
					break;
					case 6:
					sprintf(str,"TRIANGLE (%.4f)", fitness );
					putText(frame, str, minRect[i].center, FONT_HERSHEY_PLAIN,fontSize, Scalar(0,255,100),fontThickness);
					break;
					case 7:
					sprintf(str,"TRIANGLE (%.4f)", fitness );
					putText(frame, str, minRect[i].center, FONT_HERSHEY_PLAIN,fontSize, Scalar(0,255,100),fontThickness);
					break;
					default:
					sprintf(str,"?");
					putText(frame, str, minRect[i].center, FONT_HERSHEY_PLAIN,fontSize, Scalar(0,255,100),fontThickness);
					break;
				}

			}
				
		}

		imshow("frame",frame);




		
		if(waitKey(30) >= 0) break;
	}

	return 0;
}
