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
	Mat src = imread("../Picture/I4.bmp", CV_8UC1);
	if (src.empty())
	{
		cout << "Can not open image. " << endl;
		return -1;
	}
	imshow("源图像", src);
	Mat dst;
	medianBlur(src, dst, 3);
	adaptiveThreshold(dst, dst, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 7, 0);
	medianBlur(dst, dst, 5);

	namedWindow("自适应二值化后： ", WINDOW_KEEPRATIO);
	imshow("自适应二值化后： ", dst);

	//检测关键点
	std::vector<KeyPoint> keyPoints;
	Ptr<FastFeatureDetector> ffDetector = FastFeatureDetector::create(80, true, FastFeatureDetector::TYPE_9_16);
	ffDetector->detect(dst, keyPoints);

	//绘制keyPoints
	Mat keyPointsImage;
	drawKeypoints(src, keyPoints, keyPointsImage);
	namedWindow("keyPoints", WINDOW_NORMAL);
	imshow("keyPoints", keyPointsImage);
	waitKey(0);
	return 0;
}