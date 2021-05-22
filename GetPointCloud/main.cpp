#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <opencv2/opencv.hpp>
#include "GetCorner.h"
#include "LineData.h"

using namespace cv;
namespace fs = std::filesystem;
using std::ifstream;
using std::ofstream;
using std::endl;
using std::cout;

int main()
{
	FileStorage fs{ "LinePlaneData.yml", FileStorage::READ };
	fs::path fs2{ "l55cornerdata.txt" };
	std::ofstream out1{ fs2, std::ios::out };

	Matx33d M;//相机内参矩阵
	fs["cameraMatrix"] >> M;
	Matx41d distCoeffs; //相机畸变矩阵
	fs["distCoeffs"] >> distCoeffs;
	Mat abc; //光平面参数  hconcat（B,C，A）
	fs["lineplane0"] >> abc;
	for (size_t i = 1; i < 20; i++)
	{
		Matx31d coeffs;
		fs["lineplane" + std::to_string(i)] >> coeffs;
		hconcat(abc, coeffs, abc); //abc 3 * 20
	}
	fs.release();
	//cout << abc << endl;

	Corner corners{ "../Picture/l55.bmp" , M, distCoeffs };
	corners.getCorner();
	Mat m = corners.m_keyPointsImage;
	size_t numOfLines = corners.m_lines.size();
	for (size_t i = 0; i < numOfLines; i++)
	{
		double a = abc.at<double>(0, i);
		double b = abc.at<double>(1, i);
		double c = abc.at<double>(2, i);
		size_t numOfPoints = corners.m_lines[i].m_points.size();
		for (size_t j = 0; j < numOfPoints; j++)
		{
			double u = corners.m_lines[i].m_points[j].x;
			double v = corners.m_lines[i].m_points[j].y;
			Matx33d A = { M(0, 0), M(0, 1), M(0, 2) - u,
						 M(1, 0), M(1, 1), M(1, 2) - v,
							   a,        b,         -1 };
			Matx31d b = { 0, 0, -c };
			Matx31d X;
			solve(A, b, X);
			out1 << std::setprecision(6) << std::fixed << u << "    " << v << "    "
				<< i << "    " << j << "    " << X(0, 0) << "    " << X(1, 0)
				<< "    " << X(2, 0) << endl;
		}
	}

	return 0;
}