 
#include <iostream>
#include <stdlib.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main()
{
        VideoCapture capture(0);
        namedWindow("frame", WINDOW_KEEPRATIO);
        namedWindow("canny", WINDOW_KEEPRATIO);
        namedWindow("output", WINDOW_KEEPRATIO);
        if(!capture.isOpened()) return -1;
    
        for(;;)
        {
                Mat frame, edge;
                capture >> frame;
                Mat output = frame.clone();
                

                for(int i=0; i<frame.rows; i++)
                    for(int j=0; j<frame.cols; j++)
                    {
                            //poor thresholding
                            Vec3b px = frame.at<Vec3b>(i,j);
                            //cout << " " << px[0] << " " << px[1] << " " << px[2] << endl; 
                            int thres = 80;
                            if(sqrt(px[0]*px[0]+px[1]*px[1]+px[2]*px[2]) < thres)
                            {
                                frame.at<Vec3b>(i,j)[0] = 255;
                                frame.at<Vec3b>(i,j)[1] = 255;
                                frame.at<Vec3b>(i,j)[2] = 255;
                            }
                            else
                            {
                                frame.at<Vec3b>(i,j)[0] = 0;
                                frame.at<Vec3b>(i,j)[1] = 0;
                                frame.at<Vec3b>(i,j)[2] = 0;
                            }
                    }
                
                vector< vector < Point > > conts;
                Canny(frame, edge, 50, 300, 3);                
                findContours(edge, conts, RETR_TREE, CHAIN_APPROX_SIMPLE);

                unsigned int numberOfObjects=0;
                for(int i=0; i<conts.size(); i++)
                {
                    if(conts.at(i).size() > 50)
                    {
                        drawContours(output, conts, i, Scalar(0,0,255), 2, 8);
                        numberOfObjects++;
                        
                        //geometrical center of mass
                        float xacum, yacum;
                        xacum=yacum=0;
                        for(int k=0; k<conts.at(i).size(); k++)
                        {
                            xacum+=conts.at(i).at(k).x;
                            yacum+=conts.at(i).at(k).y;
                        }
                        xacum/=conts.at(i).size();
                        yacum/=conts.at(i).size();

                        circle(output, Point(xacum,yacum), 10, Scalar(255,0,0), 10);
                    }
                }
                
                cout << "number of objects " << numberOfObjects << endl;
                                
                imshow("frame", frame);
                imshow("canny", edge);
                imshow("output", output);
                if(waitKey(30) >= 0) break;
        }
}
