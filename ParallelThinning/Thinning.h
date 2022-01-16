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
Mat thinImage(const cv::Mat& src, const int maxIterations = -1);
void cvThin(cv::Mat& src, cv::Mat& dst, int intera);
void filterOver(cv::Mat thinSrc);
std::vector<cv::Point2f> getPoints(const cv::Mat& thinSrc, unsigned int raudis = 4, unsigned int thresholdMax = 6, unsigned int thresholdMin = 4);
std::vector<cv::Point2f> getKeyPoints(std::vector<cv::Point2f> InputKeyPoints, int radius = 10);
float sumGray(const Mat& src, Point2f& center, int radius);
void grayCenter(const Mat& src, Point2f& center, int radius);
void preciseCorner(const Mat& src, Point2f& center, int radius);
std::vector<Point> getLineAllPoint(Point2d p1, Point2d p2);
float getK(Mat src, std::vector<Vec4i> linesP, int threshold, int mimLineLength, int maxLineGap);

bool fitPoints(LineData& pts);

bool cmp(const Point& a, const Point& b);
std::vector<LineData> getLineData(std::vector<Point2f>keyPoints, float k);

std::vector<LineData> classifyHorizonLines(Mat& src);
int updatePoint(const Mat& src, Mat& maskImage, Point2d& p, Point2d& nextPoint);

bool cmpVerticalLineData(const LineData& a, const LineData& b);
std::vector<LineData> classifyVerticalLines(Mat& src);
int updateColsPoint(const Mat& src, Mat& maskImage, Point2d& p, Point2d& nextPoint);
