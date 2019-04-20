
#include <iostream>
#include <stdio.h>
#include <time.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "Bezier.h"
using namespace std;
using namespace cv;
#define Randmod(x) rand()%x

//直接指定4个点进行3次贝塞尔曲线拟合
// int main()
// {
//     Bezier B;
//     vector <Point2f> bestSpline;
//     Mat ground(500,500,CV_8UC1,Scalar::all(0));
//     B.addPoint(Point(300,300));
//     circle(ground,Point(300,300),10,Scalar(255),-1,8);
//     B.addPoint(Point(400,50));
//     circle(ground,Point(400,50),10,Scalar(255),-1,8);
//     B.addPoint(Point(100,100));
//     circle(ground,Point(100,100),10,Scalar(255),-1,8);
//     B.addPoint(Point(50,200));
//     circle(ground,Point(50,200),10,Scalar(255),-1,8);
//     bestSpline = B.total_points;
//     cout<<bestSpline.at(0)<<bestSpline.at(1)<<bestSpline.at(2)<<bestSpline.at(3)<<endl;

//     Point  p1 , pOld;
//     if( bestSpline.size() > 0) 
//     {
//         pOld = bestSpline.at(0);//这是计算第一个点，为了让变量pOld有初值

//         for (float t = 0.0; t <= 1; t += 0.001)//画出1000个点
//         {
//            Point p1 = B.getPoint(t, bestSpline);
//            line(ground, pOld, p1, Scalar(255));
//            pOld = p1;
//         }
//     }
//     else 
//     {
//         cout<<"没有拟合线条"<<endl;
//     }
    
//     imshow("1.jpg",ground);
//     imwrite("1.jpg",ground);
//     waitKey(0);
// }

// 带有RANSIC的筛选拟合方式
int main()
{
    Bezier B;
    int Threshold =30;
	Mat ground(500,500,CV_8UC1,Scalar::all(0));
	vector <Point2f> sample;
    vector <Point2f> bestSpline;
    srand(60);
    
  
    //在指定范围内随机晒10个点,每次不一样
	for (int i = 0; i <10; ++i)
	{
        int num_x = 100+Randmod(30);
        int num_y = 100+Randmod(200);
        Point p = Point(num_x,num_y);
        B.addPoint(p);
        circle(ground,p,2,Scalar(255),-1,8);
    }
	// 	
    //     
    //从50个点中选取4个点,这4个点的选取规则是让其他的所有点离这4个点最近.
    bestSpline = B.fit(2,ground,Threshold);
    // bestSpline = B.total_points;
    cout<<bestSpline.at(0)<<bestSpline.at(1)<<bestSpline.at(2)<<bestSpline.at(3)<<endl;

    //进行Bezier 3次曲线拟合,画出曲线,如果用在其他应用上,最好对这4个点按自己想要的坐标排序
    Point  p1 , pOld;
    if( bestSpline.size() > 0) 
    {
        pOld = bestSpline.at(0);//这是计算第一个点，为了让曲线有起点

        for (float t = 0.0; t <= 1; t += 0.001)//画出1000个点,连成线
        {
           Point p1 = B.getPoint(t, bestSpline);
           line(ground, pOld, p1, Scalar(255));
           pOld = p1;
        }
    }
    else 
    {
        cout<<"没有拟合线条"<<endl;
    }
	
    imshow("1.jpg",ground);
    waitKey(0);
    imwrite("2.jpg",ground);
}
