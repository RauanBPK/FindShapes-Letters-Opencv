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
	namedWindow("otsu", WINDOW_KEEPRATIO);
	// namedWindow("diff", WINDOW_KEEPRATIO);
	if(!capture.isOpened()) //Check if success
		return -1;

	Mat gray;
	time_t timer;
	time_t timerant;

	//-- IMAGENS PARA ACHAR --//
	Mat A = imread("./A.jpg", IMREAD_GRAYSCALE);
	Mat B = imread("./B.jpg", IMREAD_GRAYSCALE);
	Mat SPHERE = imread("./SPHERE.jpg", IMREAD_GRAYSCALE);
	Mat SQUARE = imread("./SQUARE.jpg", IMREAD_GRAYSCALE);
	Mat T1 = imread("./T1.jpg", IMREAD_GRAYSCALE);
	Mat T2 = imread("./T2.jpg", IMREAD_GRAYSCALE);
	Mat T3 = imread("./T3.jpg", IMREAD_GRAYSCALE);
	Mat T4 = imread("./T4.jpg", IMREAD_GRAYSCALE);

	int numImgsToLook = 8; // Melhorar isso -  strongtyped fica feio
	//------------------------//

	int fontSize = 2;
	int fontThickness = 2;
	int cropSize = 100; //escala da figura pra comparar
	int erroMax = cropSize*cropSize*255; // erro maximo quando comparando imgs (absdiff)
	char str[200];

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

		
		threshold(gray, gray, 100, 255, cv::THRESH_BINARY | THRESH_OTSU);
		imshow("otsu",gray);

		vector<vector< Point > >conts;
		// vector<Mat> cropped;
		vector<Vec4i> hierarchy;
		findContours(gray, conts,hierarchy, RETR_TREE  , CHAIN_APPROX_NONE);
		unsigned int numberOfObjects=0;

		vector<RotatedRect> minRect( conts.size() );
		for(int i=0; i<conts.size(); i++)
		{
			//drawContours(frame, conts, i, Scalar(0,0,255), 2, 8); //desenha contorno
			if(conts.at(i).size() > 200 && conts.at(i).size() < 2000)
			{
				
				//numberOfObjects++;


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
				//warpAffine(frame, rotated, M, frame.size(), INTER_CUBIC);
				getRectSubPix(rotated, rect_size, minRect[i].center, cropped);

				Point2f rect_points[4]; minRect[i].points( rect_points );

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
					minShape = 5;
				}
				absdiff(cropped, T3, diff);
				s = cv::sum( diff )[0];
				if(s < min){
					min = s;
					minShape = 5;
				}
				absdiff(cropped, T4, diff);
				s = cv::sum( diff )[0];
				if(s < min){
					min = s;
					minShape = 5;
				}  

				 // Se identificou como triangulo e tem um contorno filho, Deve ser A, pois triangulo nao tem filho..(esteril ?)
				if(minShape == 5 && hierarchy[i][2] != -1){
					minShape = 1;
					//recalcula o fitness pq foi sobreescrito pelo do triangulo, que apesar de melhor, nao estava correto
					absdiff(cropped, A, diff);
					min = cv::sum( diff )[0];
				}

				float fitness = 0;
				fitness = 1 - ((1*min)/(erroMax));

				Rect boundRect = boundingRect(Mat(conts[i]));
				// Se o fitness nao for bom, figura nao identificada
				// Poderia estimar um fitness bom pra cada figura. A pode ser identificado com menos por exemplo
				if(fitness < 0.65){ 
					minShape = 0;
				}else{

					//Desenha bounding rectangle normal
					rectangle( frame, boundRect.tl(), boundRect.br(), Scalar(0,191,255), 2, 8, 0 );
					
					// //Desenha bounding rectangle minimo
					// for( int j = 0; j < 4; j++ )
					// 	line( frame, rect_points[j], rect_points[(j+1)%4], Scalar(255,0,0), 4, 8 );
				}
				//Ponto pra escrever o texto
				
				Point pText = boundRect.tl(); 

				switch(minShape){
					case 1:
					sprintf(str,"A (%.4f)", fitness );
					putText(frame, str, pText, FONT_HERSHEY_PLAIN,fontSize, Scalar(0,255,100),fontThickness);
					break;
					case 2:
					sprintf(str,"B (%.4f)", fitness );
					putText(frame, str, pText, FONT_HERSHEY_PLAIN,fontSize, Scalar(0,255,100),fontThickness);

					break;
					case 3:
					sprintf(str,"CIRCULO (%.4f)", fitness );
					putText(frame, str,pText, FONT_HERSHEY_PLAIN,fontSize, Scalar(0,255,100),fontThickness);

					break;
					case 4:
					sprintf(str,"QUADRADO (%.4f)", fitness );
					putText(frame, str, pText, FONT_HERSHEY_PLAIN,fontSize, Scalar(0,255,100),fontThickness);

					break;
					case 5:
					sprintf(str,"TRIANGULO (%.4f)", fitness );
					putText(frame, str, pText, FONT_HERSHEY_PLAIN,fontSize, Scalar(0,255,100),fontThickness);
					break;

					default:
					// sprintf(str,"?");
					// putText(frame, str, minRect[i].center, FONT_HERSHEY_PLAIN,fontSize, Scalar(0,255,100),fontThickness);
					break;
				}

			}
			
		}
		sprintf(str,"Rauan Pires - 14103318");
        putText(frame, str, Point(10,40), FONT_HERSHEY_PLAIN,3, Scalar(0,255,255),3);

		imshow("frame",frame);
		
		if(waitKey(30) >= 0) break;
	}

	return 0;
}
