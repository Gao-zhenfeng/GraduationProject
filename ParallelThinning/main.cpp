#include <opencv2/opencv.hpp>
#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "Thinning.h"

using namespace cv;
using std::endl;
using std::cout;

int main(int argc, char** argv)
{
	namedWindow("源图像", WINDOW_NORMAL);
	Mat src = imread("../Picture/I4.bmp", CV_8UC1);
	if (src.empty())
	{
		cout << "image is empty. " << endl;
		return -1;
	}
	imshow("源图像", src);
	cout << "源图像格式为： " << src.type() << endl;
	Mat dst;
	medianBlur(src, dst, 3);
	imshow("中值滤波后", dst);
	//namedWindow("After Sobel： ", WINDOW_AUTOSIZE);
	//int xorder = 2;//x方向导数阶数
	//int yorder = 2;//y方向导数阶数
	//int ksize = 3; //卷积核的大小
	//Sobel(dst, dst, CV_8U, xorder, yorder, ksize);
	//imshow("After Sobel： ", dst);
	//threshold(dst, dst, 20, 255, THRESH_BINARY);
	adaptiveThreshold(dst, dst, 1, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 7, 0);
	medianBlur(dst, dst, 5);

	namedWindow("自适应二值化后： ", WINDOW_NORMAL);
	imshow("自适应二值化后： ", dst);

	Mat thinsrc;
	thinsrc = thinImage(dst);
	filterOver(thinsrc);
	Mat thinImage = thinsrc * 255;

	//绘制细化后图像
	namedWindow("细化后： ", WINDOW_NORMAL);
	imshow("细化后： ", thinImage);

	//Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
	//morphologyEx(thinImage, thinImage, MORPH_DILATE, kernel);

	std::vector<KeyPoint> roughKeyPoints = getPoints(thinsrc, 5, 7, 0);
	std::vector<KeyPoint>keyPoints = getKeyPoints(roughKeyPoints, 15);
	//std::vector<KeyPoint> keyPoints;
	//Ptr<GFTTDetector> gfttDetector = GFTTDetector::create(10000, 0.35);
	//gfttDetector->detect(thinImage, keyPoints);

	//绘制keyPoints
	Mat keyPointsImage;
	drawKeypoints(src, keyPoints, keyPointsImage, Scalar(0, 0, 255));
	namedWindow("keyPoints", WINDOW_NORMAL);
	imshow("keyPoints", keyPointsImage);

	//Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
	//morphologyEx(dst, dst, MORPH_DILATE, kernel);
	//morphologyEx(dst, dst, MORPH_DILATE, kernel);
	//morphologyEx(dst, dst, MORPH_DILATE, kernel);
	//morphologyEx(dst, dst, MORPH_CLOSE, kernel);
	//medianBlur(dst, dst, 5);
	//namedWindow("形态学操作后： ", WINDOW_NORMAL);
	//imshow("形态学操作后： ", dst);
	waitKey(0);
	return 0;
}