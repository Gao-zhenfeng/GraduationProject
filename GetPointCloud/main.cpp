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
	string rootpath = "../Data/20210526/";
	//读取相机内参矩阵、畸变矩阵和光平面参数
	FileStorage fs{ rootpath + "LinePlaneData.yml", FileStorage::READ };
	Matx33d M;//相机内参矩阵
	fs["cameraMatrix"] >> M;
	Matx41d distCoeffs; //相机畸变矩阵
	fs["distCoeffs"] >> distCoeffs;
	Mat abc; //光平面参数  hconcat（B,C，A）
	fs["Lineplane0"] >> abc;
	int numLines;
	fs["planes number"] >> numLines;
	// 获取abc所有信息
	//for (size_t i = 1; i < numLines; i++)
	//{
	//	Matx31d coeffs;
	//	fs["lineplane" + std::to_string(i)] >> coeffs;
	//	hconcat(abc, coeffs, abc); //abc 3 * 20
	//}
	//fs.release();
	//cout << abc << endl;
	string picturename = "0l1";
	fs::path fs2{ rootpath + picturename + ".txt" };
	std::ofstream out1{ fs2, std::ios::out };
	Corner corners{ rootpath + picturename + ".bmp" , M, distCoeffs };
	corners.getAllLine();
	Mat m = corners.m_keyPointsImage;
	size_t numOfLines = corners.m_lines.size();
	for (size_t i = 0; i < numOfLines; i++)
	{
		Mat ABC;// 光平面参数
		fs["Lineplane" + std::to_string(i)] >> ABC;

		size_t numOfPoints = corners.m_lines[i].m_points.size();
		for (size_t j = 0; j < numOfPoints; j++)
		{
			Mat planedata;
			planedata = findPlaneFuntction(corners.m_lines[i].m_points[j], ABC);
			double AA = planedata.at<double>(0, 0);
			double B = planedata.at<double>(0, 1);
			double C = planedata.at<double>(0, 2);
			//double AA = ABC.at<double>(0, 0);
			//double B = ABC.at<double>(1, 0);
			//double C = ABC.at<double>(2, 0);
			double u = corners.m_lines[i].m_points[j].x;
			double v = corners.m_lines[i].m_points[j].y;
			Matx33d A = { M(0, 0), M(0, 1), M(0, 2) - u,
						 M(1, 0), M(1, 1), M(1, 2) - v,
							   AA,        B,         -1 };
			Matx31d b = { 0, 0, -C };
			Matx31d X; //(xc, yc, zc)
			solve(A, b, X);
			// 向文件写入{u, v, i, j, xc, yc, zc}
			out1 << std::setprecision(6) << std::fixed << u << "    " << v << "    "
				<< i << "    " << j << "    " << X(0, 0) << "    " << X(1, 0)
				<< "    " << X(2, 0) << endl;
			//out1 << std::setprecision(6) << std::fixed << u << "    " << v << "    "
			//	<< i << "    " << j << endl;
		}
	}

	return 0;
}