// StructuredLineCalibration.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

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
	FileStorage fs{ "Camera_CaliResult2.xml", FileStorage::READ };
	FileStorage fs2{ "LinePlaneData.yml", FileStorage::WRITE };

	Matx33d M;//相机内参矩阵
	fs["intrinsic_matrix1"] >> M;
	Matx41d distCoeffs;
	fs["distortion_coeffs1"] >> distCoeffs;
	Mat rotation_matrix;
	fs["rotation_vectors1"] >> rotation_matrix;
	Mat translation_vectors;
	fs["translation_vectors1"] >> translation_vectors;
	fs.release();

	fs2 << "planes number" << 20;
	fs2 << "cameraMatrix" << M;
	fs2 << "distCoeffs" << distCoeffs;

	std::vector<Corner> allImageCorner;
	allImageCorner.push_back(Corner{ "../Picture/l52.bmp" }); allImageCorner[0].getCorner(); Mat m0 = allImageCorner[0].m_keyPointsImage;
	allImageCorner.push_back(Corner{ "../Picture/l55.bmp" }); allImageCorner[1].getCorner(); Mat m1 = allImageCorner[1].m_keyPointsImage;
	allImageCorner.push_back(Corner{ "../Picture/l57.bmp" }); allImageCorner[2].getCorner(); Mat m2 = allImageCorner[2].m_keyPointsImage;
	allImageCorner.push_back(Corner{ "../Picture/l59.bmp" }); allImageCorner[3].getCorner(); Mat m3 = allImageCorner[3].m_keyPointsImage;
	int numOfImage = allImageCorner.size(); //4张图片
	int numOfLines = allImageCorner[0].m_lines.size(); // 每张图片横向光条数量： 20

	for (int i = 0; i < numOfLines; i++)
	{
		LinePlane lp;
		for (int j = 0; j < numOfImage; j++)
		{
			// 每张图片的旋转矩阵和平移矩阵
			Matx13d R = Matx13d{ rotation_matrix.at<double>(j, 0),
										rotation_matrix.at<double>(j, 1),
											rotation_matrix.at<double>(j, 2) };
			Matx13d t = Matx13d{ translation_vectors.at<double>(j, 0),
									translation_vectors.at<double>(j, 1),
										translation_vectors.at<double>(j, 2) };
			lp.addPoints(allImageCorner[j].m_lines[i], M, distCoeffs, R, t);
		}
		lp.planeFitting();
		fs2 << "lineplane" + std::to_string(i) << lp.coeffient;
	}

	fs2.release();
	return 0;
}