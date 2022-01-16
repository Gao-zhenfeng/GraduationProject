#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include<opencv2/xfeatures2d.hpp>

namespace fs = std::filesystem;
using std::endl;
using std::cout;
using namespace cv;

using std::ifstream;
using std::ofstream;

using std::string;

int main(int argc, char** argv)
{
	string rootpath = "../Data/20210528c/";
	string picturename = "/Helmet/l158";
	//读取相机内参矩阵、畸变矩阵和光平面参数
	FileStorage fs{ rootpath + "LinePlaneData.yml", FileStorage::READ };
	Matx33d M;//相机内参矩阵
	fs["cameraMatrix"] >> M;
	Matx41d distCoeffs; //相机畸变矩阵
	fs["distCoeffs"] >> distCoeffs;
	Mat abc; //光平面参数  hconcat（B,C，A）
	fs["Lineplane0"] >> abc;
	cout << abc;
	int numLines;
	fs["planes number"] >> numLines;

	return 0;
}