#pragma once
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <filesystem>
#include "LineData.h"

using namespace cv;
using std::endl;
using std::cout;
using std::string;

class Corner
{
public:
	Corner();
	Corner(std::string filename);
	Corner(std::string filename, Matx33d M, Matx41d distCoeffs);
	Corner(Mat& src);
	void getROI(const Mat& src, Mat& img_binary);
	void cvThin(cv::Mat& src, cv::Mat& dst, int intera);
	void filterOver(cv::Mat thinSrc);
	std::vector<cv::Point2d> getPoints(const cv::Mat& thinSrc, unsigned int raudis = 4, unsigned int thresholdMax = 6, unsigned int thresholdMin = 4);
	std::vector<cv::Point2d> getKeyPoints(std::vector<cv::Point2d> InputKeyPoints, int radius = 10);
	float sumGray(const Mat& src, Point2d& center, int radius);
	void grayCenter(const Mat& src, Point2d& center, int radius);
	void preciseCorner(const Mat& src, Point2d& center, int radius);
	void getCorner();
	std::vector<LineData> getLineData(std::vector<Point2d>keyPoints, float k);
	std::vector<LineData> sortKeyPoints(std::vector<Point2d>& keyPoints);

	//使用光条上所有点
	void getAllLine();
	//获取各条横光条
	int updatePoint(const Mat& src, Mat& maskImage, Point2d& p, Point2d& nextPoint);
	std::vector<LineData> classifyHorizonLines(Mat& src);
	//获取各条纵光条
	std::vector<LineData> classifyVerticalLines(Mat& src);
	int updateColsPoint(const Mat& src, Mat& maskImage, Point2d& p, Point2d& nextPoint);

	Mat m_src; //原图
	Mat m_img_binary; //对原图做区域提取
	Mat m_keyPointsImage;//提取到的角点作到原图上
	Mat m_skeletonImage;//光条中心图
	std::vector<Point2d> m_keyPoints;//精确角点
	std::vector<LineData> m_lines;//存放网格角点，标定时使用
	std::vector<LineData> m_linePoints;//存放网格所有点
	Matx33d m_cameramatrix;
	Matx41d m_distCoeffs;
};
bool cmpLineData(const LineData& a, const LineData& b);
bool cmpVerticalLineData(const LineData& a, const LineData& b);
