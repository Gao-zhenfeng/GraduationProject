#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "Thinning.h"

#define MINIMUM_POINTS 3     // minimum number of cluster
#define EPSILON  (30 * 30)  // distance for clustering, metre^2

using namespace cv;
using std::endl;
using std::cout;

int g_adaptiveThresholdBlocksize = 80;
int g_medianBlurBlockSize = 26;

struct ThresholdGUI
{
	std::string m_winname;
	Mat m_src;
	ThresholdGUI(std::string winname, Mat& src) : m_winname(winname), m_src(src)
	{
	}
};

void on_threshold1(int bar_val, void* userdata)
{
	ThresholdGUI tGUI = *(ThresholdGUI*)userdata;
}

void on_threshold2(int bar_val, void* userdata)
{
	ThresholdGUI tGUI = *(ThresholdGUI*)userdata;
}

int main(int argc, char** argv)
{
	Mat src = imread("filter.bmp", CV_8UC1);
	Rect r1{ 630,191,588,634 };
	Mat mask = Mat::zeros(src.size(), CV_8UC1);
	if (src.empty())
	{
		cout << "image is empty. " << endl;
		return -1;
	}
	namedWindow("源图像", WINDOW_AUTOSIZE);
	imshow("源图像", src);
	mask(r1).setTo(255);
	Mat maskSrc;
	src.copyTo(maskSrc, mask);
	Mat dst;
	medianBlur(maskSrc, dst, 3);

	//std::string winname = "自适应二值化后： ";
	//ThresholdGUI tGUI{ winname, dst };
	/*createTrackbar("自适应阈值 邻域", winname, &g_adaptiveThresholdBlocksize, 20, on_threshold1, &tGUI);
	createTrackbar("中值滤波 邻域 ", winname, &g_medianBlurBlockSize, 20, on_threshold2, &tGUI);*/
	//adaptiveThreshold(dst, dst, 1, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 7, 0);
	//medianBlur(dst, dst, 5);

	//namedWindow("自适应二值化后： ", WINDOW_AUTOSIZE);
	//imshow("自适应二值化后： ", dst * 255);

	threshold(dst, dst, 10, 1, THRESH_BINARY);
	Mat thinsrc;
	thinsrc = thinImage(dst);
	filterOver(thinsrc);
	Mat thinImage = thinsrc * 255;

	//绘制细化后图像
	namedWindow("细化后： ", WINDOW_NORMAL);
	imshow("细化后： ", thinImage);

	//std::string winname = "Detected Lines (in red) - Probabilistic Line Transform";
	////namedWindow(winname, WINDOW_NORMAL);
	//// Probabilistic Line Transform
	//std::vector<Vec4i> linesP;
	////dst = dst * 255;
	//HoughLine hL{ winname, thinImage, linesP };

	std::vector<KeyPoint> roughKeyPoints = getPoints(thinsrc, 5, 7, 0);
	std::vector<KeyPoint>keyPoints = getKeyPoints(roughKeyPoints, 20);

	//绘制keyPoints
	Mat keyPointsImage;
	drawKeypoints(src, keyPoints, keyPointsImage, Scalar(0, 0, 255));
	namedWindow("keyPoints", WINDOW_AUTOSIZE);
	imshow("keyPoints", keyPointsImage);

	waitKey(0);
	return 0;
}