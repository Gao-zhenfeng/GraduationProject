#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

using namespace cv;
using std::endl;
using std::cout;

struct LineData
{
	std::vector<Point> m_points; // 光条上各个特征点
	int m_label; // 光条的标号
	float m_k; // 光条斜率
	float m_b; //光条截距

	LineData();
	LineData(std::vector<Point> points, int label, float k, float b);
};

Mat drawCornerOnImage(Mat image, const Mat& binary);
std::vector<cv::KeyPoint> getKeyPoints(std::vector<cv::KeyPoint> InputKeyPoints, int radius = 10);
std::vector<Point> getLineAllPoint(Point2d p1, Point2d p2);
float getK(Mat src, std::vector<Vec4i> linesP, int threshold, int mimLineLength, int maxLineGap);

bool fitPoints(LineData& pts);

bool cmp(const Point& a, const Point& b);
std::vector<LineData> getLineData(std::vector<KeyPoint>keyPoints, float k);
