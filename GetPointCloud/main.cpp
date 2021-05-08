#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "Thinning.h"
#include<opencv2/xfeatures2d.hpp>
using namespace cv;
using std::endl;
using std::cout;

bool comp(const Point& p1, const Point& p2)
{
	return p1.x < p2.x;
}

int main(int argc, char** argv)
{
	double time1 = static_cast<double>(getTickCount());
	Mat src = imread("../Picture/I10.bmp", CV_8UC1);
	if (src.empty())
	{
		cout << "image is empty. " << endl;
		return -1;
	}
	namedWindow("源图像", WINDOW_NORMAL);
	imshow("源图像", src);

	Mat dst = src / 255;

	Mat thinsrc;
	thinsrc = thinImage(dst);
	filterOver(thinsrc);
	Mat thinImage = thinsrc * 255;

	namedWindow("细化后： ", WINDOW_NORMAL);
	imshow("细化后： ", thinImage);

	std::vector<Vec4i> linesP;

	std::vector<KeyPoint> roughKeyPoints = getPoints(thinsrc, 4, 6, 0);
	std::vector<KeyPoint>keyPoints = getKeyPoints(roughKeyPoints, 20);

	//绘制keyPoints
	Mat keyPointsImage;
	drawKeypoints(thinImage, keyPoints, keyPointsImage, Scalar(0, 0, 255));
	namedWindow("keyPoints", WINDOW_NORMAL);
	imshow("keyPoints", keyPointsImage);

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
	namedWindow("光条编码", WINDOW_NORMAL);
	imshow("光条编码", rgbSRC);
	//imwrite("labeled.bmp", rgbSRC);
	double time2 = (static_cast<double>(getTickCount()) - time1) / getTickFrequency();
	cout << time2 << "s" << endl;
	waitKey(0);
	return 0;
}