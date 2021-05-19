// Harris-Shi-TomasiDetector.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;
using std::endl;
using std::cout;

int main()
{
	namedWindow("源图像", WINDOW_AUTOSIZE);
	// graySrc 是提取到的光条区域，是一张二值图像
	Mat graySrc = imread("../Picture/I9.bmp", CV_8UC1);
	// 灰度图像
	Mat src = imread("../Picture/I11.bmp", CV_8UC1);
	if (src.empty())
	{
		cout << "Can not open image. " << endl;
		return -1;
	}
	//imshow("源图像", src);
	Mat dst;
	bitwise_and(src, graySrc, dst);

	//检测关键点
	std::vector<KeyPoint> keyPoints;
	Ptr<FastFeatureDetector> ffDetector = FastFeatureDetector::create(130, true, FastFeatureDetector::TYPE_9_16);
	ffDetector->detect(dst, keyPoints);

	//绘制keyPoints
	Mat keyPointsImage;
	drawKeypoints(dst, keyPoints, keyPointsImage);
	namedWindow("keyPoints", WINDOW_NORMAL);
	imshow("keyPoints", keyPointsImage);
	waitKey(0);
	return 0;
}