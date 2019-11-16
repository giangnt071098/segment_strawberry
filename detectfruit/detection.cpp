//file ham con
#include"stdafx.h"
#include"detection.h"
int rmin1[3] = { 0,120,70 };
int rmax1[3] = { 10,255,255 };
int rmin2[3] = { 160,120,70 };
int rmax2[3] = { 180,255,255 };
int gmin[3] = { 30,70,40 };
int gmax[3] = { 80,255,255 };
void detectcolor(Mat &src)
{
	Mat imgHSV;
	cvtColor(src, imgHSV, COLOR_BGR2HSV);//Convert the captured frame from BGR to HSV
										 //imshow("srcHSV", src);
	Mat mask1, mask2, mask3, mask4;
	//detect red color
	inRange(imgHSV, Scalar(rmin1[0], rmin1[1], rmin1[2]), Scalar(rmax1[0], rmax1[1], rmax1[2]), mask1);
	inRange(imgHSV, Scalar(rmin2[0], rmin2[1], rmin2[2]), Scalar(rmax2[0], rmax2[1], rmax2[2]), mask2);
	//Detect green color
	inRange(imgHSV, Scalar(gmin[0], gmin[1], gmin[2]), Scalar(gmax[0], gmax[1], gmax[2]), mask3);
	// Generating the final mask
	bitwise_or(mask1, mask2, src);
	bitwise_or(mask3, src, src);

	imshow("mask1", src);
}
void filteredges(Mat src, Mat &imgResult)
{
	Mat kernel = (Mat_<float>(3, 3) <<
		1, 1, 1,
		1, -8, 1,
		1, 1, 1);
	Mat imgLaplacian;
	filter2D(src, imgLaplacian, CV_32F, kernel);
	imgLaplacian.convertTo(imgLaplacian, CV_32F);
	Mat sharp;
	src.convertTo(sharp, CV_32F);
	imgResult = sharp - imgLaplacian;
	imgResult.convertTo(imgResult, CV_8UC3);
}
void segmentation(Mat src, Mat &markers, Mat imgResult, vector<vector<Point> > &contours)
{
	Mat dist;
	cvtColor(imgResult, dist, COLOR_BGR2GRAY);

	// su dung thuat toan distanceTransorm 
	//de sang nhat o tam mo dan ve bien cua 1 object
	distanceTransform(dist, dist, DIST_L2, 3);

	// chuan hoa cuong do sang do trong khoang [0,1]
	normalize(dist, dist, 0, 1.0, NORM_MINMAX);
	imshow("Distance Transform Image", dist);

	//phan nguong lai anh
	// nhung phan co gia tri tu 0.4->1 la foreground
	threshold(dist, dist, 0.4, 1.0, THRESH_BINARY);
	//lam ro hon foreground
	Mat kernel1 = Mat::ones(3, 3, CV_8U);
	dilate(dist, dist, kernel1);
	//imshow("Peaks", dist);
	Mat dist_8u;
	dist.convertTo(dist_8u, CV_8U);
	//imshow("distCV-8U", dist_8u);
	// Tim contours cua anh
	findContours(dist_8u, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	//Xoa bot nhung contours co size nho
	size_t a = contours.size();
	for (size_t i = 0; i < a; i++)
	{
		if (contours[i].size() < 50)
		{
			contours.erase(contours.begin() + i);
			a -= 1;
			i--;
		}

	}
	//for (size_t i = 0; i < contours.size(); i++)
	{
		//cout << contours[i].size()<<endl;
	}
	cout << contours.size();
	// Tao  ma tran markers
	markers = Mat::zeros(dist.size(), CV_32S);


	// ve cac "seed" len marker
	for (size_t i = 0; i < contours.size(); i++)
	{
		drawContours(markers, contours, (int)(i), Scalar((int)(i)+1), -1);
	}
	// Draw the background marker
	circle(markers, Point(5, 5), 3, Scalar(255), -1);
	circle(markers, Point(markers.cols - 5, markers.rows - 5), 3, Scalar(255), -1);
	circle(markers, Point(5, markers.rows - 5), 3, Scalar(255), -1);
	circle(markers, Point(markers.cols - 5, 5), 3, Scalar(255), -1);
	imshow("Markers", markers * 10000);

	//cout<<markers.size();

	// watershed algorithm

	watershed(src, markers);
}
void addcolor(Mat &dst, vector<vector<Point>>contours, Mat markers, vector<Vec3b>&colors)
{
	for (size_t i = 0; i < contours.size(); i++)
	{
		int b = theRNG().uniform(0, 256);
		int g = theRNG().uniform(0, 256);
		int r = theRNG().uniform(0, 256);

		colors.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
	}

	// Khai bao ma tran result
	dst = Mat::zeros(markers.size(), CV_8UC3);

	// them mau cho tung object
	for (int i = 0; i < markers.rows; i++)
	{
		for (int j = 0; j < markers.cols; j++)
		{
			int index = markers.at<int>(i, j);
			if (index > 0 && index <= (int)(contours.size()))
			{
				dst.at<Vec3b>(i, j) = colors[index - 1];
			}
		}
	}

	imshow("Final Result", dst);
}
void bold(Mat &src)
{
	cvtColor(src, src, CV_BGR2HSV);
	Mat test = Mat::zeros(src.rows, src.cols, CV_8UC3);
	imshow("boldsrc", src);
	for (int i = 0; i < src.rows; i++)
		for (int j = 0; j < src.cols; j++)
		{
			Vec3b point = src.at<Vec3b>(i, j);
			/*if ((point[0]>=rmin1[0]&&point[1]>rmin1[1]&&point[2]>rmin1[2]&&point[0]<rmax1[0]&&point[1]<rmax1[1]&&point[2]<rmax1[2]) ||
				(point[0] > rmin2[0] &&point[1]>rmin2[1]&&point[2]>rmin2[2]&& point[0] < rmax2[0]&&point[1]<rmax2[1]&&point[2]<rmax2[2]))
			{
				test.at<Vec3b>(i, j) = src.at<Vec3b>(i, j);
				src.at<Vec3b>(i, j)[0] = 0;
				src.at<Vec3b>(i, j)[1] = 100;
				src.at<Vec3b>(i, j)[2] = 100;

			}*/
			if ((point[0] > (gmin[0] - 21) && point[1] > (gmin[1]) && point[2] > (gmin[2]) && point[0] < gmax[0] && point[1] < (gmax[1]) && point[2] < gmax[2]))
			{
				test.at<Vec3b>(i, j) = src.at<Vec3b>(i, j);
				src.at<Vec3b>(i, j)[0] = 250;//17
				src.at<Vec3b>(i, j)[1] = 250;
				src.at<Vec3b>(i, j)[2] = 250;//170
			}
		}
	cvtColor(src, src, CV_HSV2BGR);
	imshow("test", test);
	imshow("srctrans", src);

}
void contoursofdst(Mat src, vector<Vec3b>cl, vector<vector<Point>>&ct, vector<RotatedRect> &s)
{

	vector<Mat> mah(cl.size());
	vector<vector<Point>>ct2;
	// luu tung mau vao mah[i]
	for (size_t i = 0; i < cl.size(); i++)
		inRange(src, cl[i], cl[i], mah[i]);
	for (size_t i = 0; i < mah.size(); i++)
	{
		findContours(mah[i], ct2, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
		ct.push_back(ct2[0]);
		s.push_back(minAreaRect(ct2[0]));
	}

}
void findaxis(Mat src, vector<RotatedRect>s)
{
	for (size_t i = 0; i < s.size(); i++)
	{
		Point2f point[4];
		s[i].points(point);
		for (int j = 0; j < 4; j++)
			line(src, point[j], point[(j + 1) % 4], 255);
		if ((pow((point[0].x - point[1].x), 2) + pow((point[0].y - point[1].y), 2))
		> (pow((point[1].x - point[2].x), 2) + pow((point[1].y - point[2].y), 2)))
		{
			line(src, (point[1] + point[2]) / 2, (point[3] + point[0]) / 2, 255);
		}
		else
			line(src, (point[0] + point[1]) / 2, (point[2] + point[3]) / 2, 255);
	}
}
void convex(Mat src, vector<vector<Point>>ct, size_t size,vector<Vec3b>color)
{
	Mat convex_img = Mat::zeros(src.size(), CV_8UC3);
	vector<RotatedRect>s2;
	vector<vector<Point>>hull(size);
	for (int i = 0; i < size; i++)
	{
		convexHull(ct[i], hull[i], false);
	}
	for (int i = 0; i < hull.size(); i++)
	{
		s2.push_back(minAreaRect(hull[i]));
		drawContours(src, hull, i, color[i], 1);
		drawContours(convex_img, hull, i, color[i], -1);

	}
	//drawContours(convex_img, hull, 2, color[2], -1);
	findaxis(convex_img, s2);
	imshow("convex img", convex_img);
}
Mat erodil(Mat src, int i, int sizematrix)
{
	Mat kernel = Mat::ones(sizematrix, sizematrix, CV_8U);
	if (i == 1) erode(src, src, kernel);
	if (i == 2) dilate(src, src, kernel);
	if (i == 3)morphologyEx(src, src, CV_MOP_GRADIENT, kernel, Point(-1, -1));
	if (i == 4)morphologyEx(src, src, CV_MOP_CLOSE, kernel, Point(-1, -1));
	if (i == 5)morphologyEx(src, src, CV_MOP_OPEN, kernel, Point(-1, -1));
	if (i == 6)morphologyEx(src, src, CV_MOP_TOPHAT, kernel, Point(-1, -1));
	return src;
}
Mat read_image(string link)
{
	return imread(link, IMREAD_COLOR);
}
Mat imwrite_and_imread(string link,Mat src)
{
	imwrite(link, src);
	return src = imread(link, IMREAD_COLOR);
}