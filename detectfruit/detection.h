//file khai bao cac ham con
#pragma once
#include<opencv2/opencv.hpp>
#include<vector>
#include<math.h>
#include<iostream>
using namespace std;
using namespace cv;

void contoursofdst(Mat src, vector<Vec3b>cl, vector<vector<Point>>&ct, vector<RotatedRect>&s);//gan cac bien vao 1 vector ct
void findaxis(Mat src, vector<RotatedRect>s);//ve truc
void detectcolor(Mat &src); //loc mau
void filteredges(Mat src, Mat &imgResult);//loc laplacian
void segmentation(Mat src, Mat &markers, Mat imgReusult, vector<vector<Point> > &contours);//distance tranform and watershed
void addcolor(Mat &dst, vector<vector<Point>>contour, Mat marker, vector<Vec3b>&colors);// them mau cho qua
void bold(Mat &src);//lam dam la
void convex(Mat src, vector<vector<Point>>ct, size_t size,vector<Vec3b>);
Mat read_image(string link);//doc anh
Mat erodil(Mat src, int i, int sizematrix);//i=1 dung erode;i=2 dung dilate
Mat imwrite_and_imread(string link,Mat);//ghi anh va doc anh

