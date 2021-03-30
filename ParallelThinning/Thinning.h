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

Mat thinImage(const cv::Mat& src, const int maxIterations = -1);
void filterOver(cv::Mat thinSrc);
std::vector<cv::KeyPoint> getPoints(const cv::Mat& thinSrc, unsigned int raudis = 4, unsigned int thresholdMax = 6, unsigned int thresholdMin = 4);
std::vector<cv::KeyPoint> getKeyPoints(std::vector<cv::KeyPoint> InputKeyPoints, int radius = 10);
std::vector<Point> getLineAllPoint(Point2d p1, Point2d p2);
float getK(Mat src, std::vector<Vec4i> linesP, int threshold, int mimLineLength, int maxLineGap);

bool fitPoints(LineData& pts);
std::vector<LineData> getLineData(std::vector<KeyPoint>keyPoints, float k);
