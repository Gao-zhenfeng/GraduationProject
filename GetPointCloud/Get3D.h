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

class LinePlane
{
public:
	LinePlane();
	LinePlane(std::vector<Point3d> points, int label);
	void planeFitting();
	std::vector<Point3d> m_points;//光平面上各点
	int m_label{ 0 };//光平面标号
	//z = ax + by + c
	Mat coeffient; //{a, b, c}
};

Mat ReadMatFromTxt(string filename, int rows, int cols);
