#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "Thinning.h"

using namespace cv;
using std::endl;
using std::cout;

int main(int argc, char** argv)
{
	double time1 = static_cast<double>(getTickCount());
	Mat graySrc = imread("../Picture/I9.bmp", CV_8UC1);
	Mat src = imread("../Picture/I10.bmp", CV_8UC1);
	medianBlur(src, src, 3);
	if (src.empty())
	{
		cout << "image is empty. " << endl;
		return -1;
	}

	/*mask(r1).setTo(255);
	Mat maskSrc;
	src.copyTo(maskSrc, mask);*/

	Mat dst = src / 255;
	//medianBlur(src, dst, 3);
	//adaptiveThreshold(dst, dst, 1, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 7, 0);
	//medianBlur(dst, dst, 5);

	Mat thinsrc;
	cvThin(dst, thinsrc, 5);
	filterOver(thinsrc);
	Mat thinImage = thinsrc * 255;

	//Mat rgbSrc;
	//cvtColor(src, rgbSrc, COLOR_GRAY2RGB);
	//for (int i = 0; i < rgbSrc.rows; i++)
	//{
	//	for (int j = 0; j < rgbSrc.cols; j++)
	//	{
	//		if (thinImage.at<uchar>(i, j) > 10)
	//		{
	//			rgbSrc.at<Vec3b>(i, j) = Vec3b{ 0, 0, 255 };
	//		}
	//	}
	//}

	/*std::vector<Point2f> corners;
	double qualityLevel = 0.01;
	double minDistance = 10;
	int blockSize = 5, gradientSize = 3;
	bool useHarrisDetector = false;
	double k = 0.04;
	goodFeaturesToTrack(thinImage,
		corners,
		1000,
		qualityLevel,
		minDistance,
		Mat(),
		blockSize,
		gradientSize,
		useHarrisDetector,
		k);
	cout << "** Number of corners detected: " << corners.size() << endl;
	int radius = 2;
	Mat copy = src.clone();
	for (size_t i = 0; i < corners.size(); i++)
	{
		circle(copy, corners[i], radius, Scalar(0, 0, 255), FILLED);
	}
	imshow("harris corner", src);*/

	std::vector<Point2f> roughKeyPoints = getPoints(thinsrc, 5, 8, 0);
	Mat roughKeyPointsImage;
	//drawKeypoints(src, roughKeyPoints, roughKeyPointsImage);
	std::vector<Point2f>keyPoints = getKeyPoints(roughKeyPoints, 10);

	//绘制keyPoints
	Mat keyPointsImage;
	cvtColor(graySrc, keyPointsImage, COLOR_GRAY2RGB);
	for (size_t i = 0; i < keyPoints.size(); i++)
	{
		grayCenter(graySrc, keyPoints[i], 2);
		//preciseCorner(graySrc, keyPoints[i].pt, 2);
		circle(keyPointsImage, keyPoints[i], 1, Scalar(0, 0, 255), FILLED);
	}
	//drawKeypoints(src, keyPoints, keyPointsImage, Scalar(0, 0, 255));

	//std::filesystem::path p{ "keypoint.txt" };
	//std::ofstream fout{ p };

	//for (size_t i = 0; i < keyPoints.size(); i++)
	//{
	//	fout << keyPoints[i].pt.x << "  " << keyPoints[i].pt.y << endl;
	//}
	//fout.close();

	//float k = getK(thinImage, linesP, g_threshold, g_mimLineLength, g_maxLineGap);
	std::vector<LineData> lines = getLineData(keyPoints, 20);

	Mat rgbSRC;
	cvtColor(src, rgbSRC, COLOR_GRAY2BGR);

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

	double time2 = (static_cast<double>(getTickCount()) - time1) / getTickFrequency();
	cout << time2 << "s" << endl;
	waitKey(0);
	return 0;
}