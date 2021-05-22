#pragma once
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <filesystem>
#include "LineData.h"

namespace fs = std::filesystem;
using std::ifstream;
using std::ofstream;
using namespace cv;
using std::endl;
using std::cout;
using std::string;

// 三维坐标点
class LinePlane
{
public:
	LinePlane();
	LinePlane(std::vector<Point3d> points, int label);
	LinePlane(LineData lines, Matx33d M, Matx41d distCoeffs, Matx13d R, Matx13d t);
	void addPoints(LineData lines, Matx33d M, Matx41d distCoeffs, Matx13d R, Matx13d t);
	void planeFitting();
	std::vector<Point3d> m_points;//光平面上各点
	int m_label{ 0 };//光平面标号
	//z = ax + by + c
	Mat coeffient; //{a, b, c}
	LineData m_line;
};

Mat ReadMatFromTxt(string filename, int rows, int cols);
