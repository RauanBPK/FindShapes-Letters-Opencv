 
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
        namedWindow("subimg", WINDOW_KEEPRATIO);
        namedWindow("GraphY", WINDOW_KEEPRATIO);
        namedWindow("GraphX", WINDOW_KEEPRATIO);
        
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
                dilate(edge, edge, Mat(), Point(-1, -1), 2, 1, 1);
           
               // findContours(edge, conts, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
                findContours( edge, conts, RETR_TREE, CHAIN_APPROX_TC89_L1 );

                unsigned int numberOfObjects=0;
                for(int i=0; i<conts.size(); i++)
                {
                    if(conts.at(i).size() > 200)
                    {
                        drawContours(output, conts, i, Scalar(0,0,255), 2, 8);
                        numberOfObjects++;
                        
                        //geometrical center of mass
                        float xacum, yacum;
                        xacum=yacum=0;
                        
                        float minx, miny, maxx, maxy;
                        maxx=maxy=-999999;
                        minx=miny=999999;
                
                        for(int k=0; k<conts.at(i).size(); k++)
                        {
                            float xx = conts.at(i).at(k).x;
                            float yy = conts.at(i).at(k).y;
                            xacum+=xx;
                            yacum+=yy;
                            
                            if(xx < minx) minx=xx;
                            if(yy < miny) miny=yy;
                            if(xx > maxx) maxx=xx;
                            if(yy > maxy) maxy=yy;                  
                        }
                        xacum/=conts.at(i).size();
                        yacum/=conts.at(i).size();

                        circle(output, Point(xacum,yacum), 10, Scalar(255,0,0), 10);
                        
                        Mat subImg = frame(Rect(minx,miny,maxx-minx,maxy-miny));
                        Mat graphY = Mat::zeros(subImg.size(),subImg.type());
                        Mat graphX = Mat::zeros(subImg.size(),subImg.type());
                        //dy
                        for(int j=0; j<subImg.cols; j++)
                        {
                            float acum=0;
                            for(int i=0; i<subImg.rows; i++)
                                if(subImg.at<Vec3b>(i,j)[0]==255) acum++;
                            line(graphY, Point(j,0), Point(j,acum), Scalar(255,255,255), 1);
                        }
                        //dx
                        for(int i=0; i<subImg.rows; i++)
                        {
                            float acum=0;
                            for(int j=0; j<subImg.cols; j++)
                                if(subImg.at<Vec3b>(i,j)[0]==255) acum++;
                            line(graphX, Point(0,i), Point(acum,i), Scalar(255,255,255), 1);
                        }
                        imshow("subimg",subImg);
                        imshow("GraphY",graphY);
                        imshow("GraphX",graphX);
                        
                        waitKey(10);
                    }                    
                    //draw individually each contourn
    
                }
                
                cout << "number of objects " << numberOfObjects << endl;
                                
                imshow("frame", frame);
                imshow("canny", edge);
                imshow("output", output);
                if(waitKey(30) >= 0) break;
        }
}
