// StructuredLineCalibration.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
/*
1.读取相机内外参和畸变参数
2.根据相机内外参，计算标定平面的三维信息
3.将三维点写入文件

*/
#include <iostream>
#include <fstream>
#include <filesystem>
#include <opencv2/opencv.hpp>
#include "PlaneFitting.h"
#include "GetCorner.h"
#include "LineData.h"

using namespace cv;
using std::endl;
using std::cout;

/*
相机内参矩阵M 单应性矩阵H
				|fx 0  u0|
			M = |0  fy v0|     H = M * [r1 r2 t]
				|0  0  1 |

求解如下方程组：为求 zc xw yw
			|u|		  |xw|		   |u|			|xw|
		zc	|v| = H * |yw|     q = |v|		X = |yw|
			|1|		  |1 |		   |1|			|zc|

转换为		|h11  h12  -u|		 |-h13|
			|h21  h22  -v| * X = |-h23|
			|h31  h32  -1|		 |-h33|

进而求得：
		|xc|		   |xw|
		|yc| = [R T] * |yw|
		|zc|		   |0 |
					   |1 |

最小二乘求得光平面参数：  A*xc + B*yc + Czc + D = 0

						|xc1 yc1 zc1 1|		  |A|
						|xc2 yc2 zc2 1|   *   |B|	= 0
						|..  ..  ..  1|		  |C|
						|xcn ycn zcn 1|		  |D|

求光平面上各点的三维坐标：
									 |xc|
						zc * q = M * |yc|
									 |zc|

						|xc/zc|				|u|
						|yc/zc|  =  M(-1) * |v|   联立光平面方程求解
						|  1  |				|1|

*/

int main()
{
	string rootpath = "../Data/20210526/";
	FileStorage fs{ rootpath + "camera_coeffs.xml", FileStorage::READ };
	FileStorage fs2{ rootpath + "LinePlaneData.yml", FileStorage::WRITE };
	Matx33d M;//相机内参矩阵
	fs["intrinsic_matrix1"] >> M;
	Matx41d distCoeffs;
	fs["distortion_coeffs1"] >> distCoeffs;
	Mat rotation_matrix;
	fs["rotation_vectors1"] >> rotation_matrix;
	Mat translation_vectors;
	fs["translation_vectors1"] >> translation_vectors;
	fs.release();

	fs2 << "cameraMatrix" << M;
	fs2 << "distCoeffs" << distCoeffs;

	std::vector<cv::String> images;

	std::string path = rootpath + "*.bmp";

	// 使用glob函数读取所有图像的路径
	cv::glob(path, images);
	std::vector<Corner> allImageCorner;
	for (size_t i = 0; i < images.size(); i++)
	{
		allImageCorner.push_back(Corner{ images[i] , M, distCoeffs });
		allImageCorner[i].getAllLine();
		//m0作验证用
		Mat m0 = allImageCorner[i].m_keyPointsImage;
	}

	size_t numOfImage = allImageCorner.size(); //n张图片
	size_t numOfLines = allImageCorner[0].m_lines.size(); // 每张图片横向光条数量： 20
	fs2 << "planes number" << int(numOfLines);

	// 计算光平面上的各点的三维坐标和光平面的参数，将相机内参、畸变参数、光平面参数写入XML文件
	//for (size_t i = 0; i < numOfLines; i++)  // 记得改为 i < numOfLines - 1
	//{
	//	LinePlane lp;
	//	for (size_t j = 0; j < numOfImage; j++)
	//	{
	//		// 每张图片的旋转矩阵和平移矩阵
	//		Matx13d R = Matx13d{ rotation_matrix.at<double>(j, 0),
	//									rotation_matrix.at<double>(j, 1),
	//										rotation_matrix.at<double>(j, 2) };
	//		Matx13d t = Matx13d{ translation_vectors.at<double>(j, 0),
	//								translation_vectors.at<double>(j, 1),
	//									translation_vectors.at<double>(j, 2) };
	//		lp.addPoints(allImageCorner[j].m_lines[i], M, distCoeffs, R, t);
	//	}
	//	lp.planeFitting();
	//	fs2 << "Lineplane" + std::to_string(i) << lp.coeffient;
	//}
	//fs2.release();

	// 计算光平面上的各点的三维坐标和光平面的参数，将相机内参、畸变参数、光平面参数写入XML文件
	//新方法，计算相邻两张图片
	for (size_t i = 0; i < numOfLines; i++)
	{
		LinePlane lp;
		Mat lpcoeffient;
		for (size_t j = 0; j < numOfImage - 1; j++)
		{
			// 每张图片的旋转矩阵和平移矩阵
			Matx13d R = Matx13d{ rotation_matrix.at<double>(j, 0),
										rotation_matrix.at<double>(j, 1),
											rotation_matrix.at<double>(j, 2) };
			Matx13d t = Matx13d{ translation_vectors.at<double>(j, 0),
									translation_vectors.at<double>(j, 1),
										translation_vectors.at<double>(j, 2) };
			lp.addPoints(allImageCorner[j].m_lines[i], M, distCoeffs, R, t);
			// 相邻两张图
			lp.addPoints(allImageCorner[j + 1].m_lines[i], M, distCoeffs, R, t);
			lp.planeFitting();

			Mat tempCoeffient = Mat::zeros(1, 7, CV_64F);
			tempCoeffient.at<double>(0, 0) = lp.coeffient.at<double>(0, 0);
			tempCoeffient.at<double>(0, 1) = lp.coeffient.at<double>(1, 0);
			tempCoeffient.at<double>(0, 2) = lp.coeffient.at<double>(2, 0);
			tempCoeffient.at<double>(0, 3) = allImageCorner[j].m_lines[i].m_k;
			tempCoeffient.at<double>(0, 4) = allImageCorner[j].m_lines[i].m_b;
			tempCoeffient.at<double>(0, 5) = allImageCorner[j + 1].m_lines[i].m_k;
			tempCoeffient.at<double>(0, 6) = allImageCorner[j + 1].m_lines[i].m_b;

			if (lpcoeffient.empty())
			{
				lpcoeffient = tempCoeffient.clone();
			}
			else
			{
				vconcat(lpcoeffient, tempCoeffient, lpcoeffient);
			}
		}
		//cout << "Lineplane" + std::to_string(i) << "\n" << lpcoeffient;
		fs2 << "Lineplane" + std::to_string(i) << lpcoeffient;
	}
	fs2.release();

	// 计算标定板各点的三维坐标， 并写入文件
	for (size_t i = 0; i < numOfImage; i++)
	{
		//写入文件路径
		string filename = rootpath + "Image" + std::to_string(i) + ".txt";
		fs::path p1{ filename };
		std::ofstream out1{ p1, std::ios::out };
		LinePlane imagePlane;
		for (size_t j = 0; j < numOfLines; j++)
		{
			Matx13d R = Matx13d{ rotation_matrix.at<double>(i, 0),
											rotation_matrix.at<double>(i, 1),
												rotation_matrix.at<double>(i, 2) };
			Matx13d t = Matx13d{ translation_vectors.at<double>(i, 0),
									translation_vectors.at<double>(i, 1),
										translation_vectors.at<double>(i, 2) };
			// 写入（u, v, xindex, yindex, x, y, z)
			imagePlane.addPointsAndPrint(allImageCorner[i].m_lines[j], M, distCoeffs, R, t, out1);
		}
		//imagePlane.printTXT(filename);
	}

	return 0;
}