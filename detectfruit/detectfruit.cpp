//file chua ham main

#include "stdafx.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>
//#include "convexhull.h"
#include "detection.h"
using namespace cv;
using namespace std;

string link = "0.jpg";
string link_write = "dst.jpg";
Mat src;//anh goc dau vao tao marker
Mat src1;// anh dau vao tao bien watershed
Mat dst;//anh ve len imshow
vector<Vec3b>colors;//mau tung objects 
Mat markers;// anh marker
Mat imgResult,bw; //anh sau bo loc laplace, imgResult anh tao anh marker
vector<vector<Point> > contours;// contours cua anh marker
vector<vector<Point>>contours_fruit;//duong bien tung qua
vector<RotatedRect>rectangle_of_fruit;//hinh chu nhat bao tung qua
int main()
{
		src = read_image(link);
		    imshow("src", src);
		src1 = read_image(link);
		bold(src1);
		src1 = erodil(src1, 5,3);
		GaussianBlur(src1, src1, Size(3, 3), 0, 0, 4);
		    imshow("src1", src1);
    // Phan he mau HSV lay mau do va xanh
		detectcolor(src);
		src=imwrite_and_imread(link_write,src);
    // dilate diem anh trang
		src = erodil(src, 2,3);
		    imshow("src after dilate", src);

   // dung laplace lam ro cac edges

		filteredges(src, imgResult);
		filteredges(src1, bw);
		    imshow("ddss", bw);
		    imshow("New Sharped Image", imgResult);
	// Tao anh nhi phan tu imgResult
		segmentation(bw, markers, imgResult,contours);

			// Generate random colors
		addcolor(dst, contours, markers,colors);
		//Luu lai duong bien cac qua va  4 dinh hinh chu nhat bao
		contoursofdst(dst, colors, contours_fruit, rectangle_of_fruit);
		convex(dst, contours_fruit,contours.size(),colors);
			//ve hinh chu nhat
		findaxis(dst, rectangle_of_fruit);
			imshow("Final Result2", dst);
	waitKey(0);
    return 0;
}

