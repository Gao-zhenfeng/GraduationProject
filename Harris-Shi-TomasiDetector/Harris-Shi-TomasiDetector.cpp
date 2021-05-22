// Harris-Shi-TomasiDetector.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include "GetKeyPoints.h"
using namespace cv;
using std::endl;
using std::cout;

int main()
{
	// graySrc 是提取到的光条区域，是一张二值图像
	Mat graySrc = imread("../Picture/I14.bmp", CV_8UC1);
	// 灰度图像
	Mat src = imread("../Picture/I13.bmp", CV_8UC1);
	if (src.empty())
	{
		cout << "Can not open image. " << endl;
		return -1;
	}
	//imshow("源图像", src);
	Mat dst;
	bitwise_and(src, graySrc, dst);

	//检测关键点
	std::vector<KeyPoint> roughKeyPoints;
	//Fast角点
	//Ptr<FastFeatureDetector> ffDetector =
	//	FastFeatureDetector::create(50, true, FastFeatureDetector::TYPE_7_12);
	//ffDetector->detect(dst, roughKeyPoints);

	// Harris corner
	int 	maxCorners = 1200;
	double 	qualityLevel = 0.04;
	double 	minDistance = 7;
	int 	blockSize = 5;// 5 7
	bool 	useHarrisDetector = false;
	double 	k = 0.04;
	Ptr<GFTTDetector> gftt = GFTTDetector::create(maxCorners,
		qualityLevel,
		minDistance,
		blockSize,
		useHarrisDetector,
		k
	);
	gftt->detect(dst, roughKeyPoints);

	//绘制keyPoints
	Mat keyPointsImage;
	drawKeypoints(dst, roughKeyPoints, keyPointsImage);

	// 对粗略特征点做平均，二次筛选
	std::vector<KeyPoint>keyPoints = getKeyPoints(roughKeyPoints, 10);// 10

	//绘制keyPoints，调试用
	Mat Image;
	cvtColor(graySrc, Image, COLOR_GRAY2RGB);
	for (size_t i = 0; i < keyPoints.size(); i++)
	{
		circle(Image, keyPoints[i].pt, 2, Scalar(0, 0, 255), FILLED);
	}

	// 对特征点进行编码，参数一般为 11 ~ 13
	std::vector<LineData> lines = getLineData(keyPoints, 13);

	Mat rgbSRC;
	cvtColor(dst, rgbSRC, COLOR_GRAY2BGR);

	for (size_t i = 0; i < lines.size(); i++)
	{
		for (size_t j = 0; j < lines[i].m_points.size(); j++)
		{
			String s = "(" + std::to_string(lines[i].m_label) + "," + std::to_string(j) + ")";
			//cout << lines[i].m_points[j].x << "  " << lines[i].m_points[j].y << "  "
				//<< lines[i].m_k << "  " << lines[i].m_b << "  " << lines[i].m_label << endl;

			circle(rgbSRC, lines[i].m_points[j], 1, Scalar(255, 0, 0), -1);
			putText(rgbSRC, s, lines[i].m_points[j], FONT_HERSHEY_SIMPLEX, 0.25, Scalar(0, 0, 255), 1, LINE_AA);//在图片上写文字
		}
	}

	waitKey(0);
	return 0;
}