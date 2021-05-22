#pragma once
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <filesystem>

using namespace cv;
using std::endl;
using std::cout;
using std::string;

// 二维图像上点排序
struct LineData
{
	std::vector<Point2d> m_points; // 光条上各个特征点
	int m_label; // 光条的标号
	float m_k; // 光条斜率
	float m_b; //光条截距

	LineData();
	LineData(std::vector<Point2d> points, int label, float k, float b);
};

bool cmp(const Point& a, const Point& b);

bool fitPoints(LineData& pts);
