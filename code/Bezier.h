#pragma once
#ifndef Bezier_h_
#define Bezier_h_
#include <iostream>
#include <stdio.h>
#include <time.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;
class Bezier
{
  public:
        vector<Point2f> total_points;
  public:
        Bezier();
        void addPoint(Point p);
        vector<Point2f> getSample(int n);//获取n个样点
        vector<Point2f> fit(int _iterNum,Mat _image,int Threshold) ;
        vector<Point2f> fitSpline(vector<Point2f> ps);
        float computeScore(vector<Point2f> spline, Mat _image,int Threshold); 
        void getT(Mat& _T, float t);
        void getM(Mat& _M);
        void getP(Mat& _P, vector<Point2f> ps);
        Point getPoint(float t, vector<Point2f> spline) ;
        vector<Point2f> order(vector<Point2f> spline);//按照坐标位置进行排序
};

#endif