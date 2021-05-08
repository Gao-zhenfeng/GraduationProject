// StructuredLineCalibration.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <opencv2/opencv.hpp>

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
			|h32  h32  -1|		 |-h33|

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
	FileStorage fs{ "Camera_CaliResult.xml", FileStorage::READ };
	Matx33f M;//相机内参矩阵
	fs["intrinsic_matrix1"] >> M;
	cout << M << endl;
	fs.release();
	//Mat R;//旋转矩阵
	//fs["rotation_vectors1"] >> R;
	//cout << R << endl;
	//Matx31f t = Matx31f::eye();//平移矩阵
	//Matx31f q = Matx31f::eye();//图像坐标
	// T = [r1, r2, t]
	//Matx33f T = Matx33f(R(0, 0), R(0, 1), t(0, 1),
	//	R(1, 0), R(1, 1), t(1, 2),
	//	R(2, 0), R(2, 1), t(2, 2));
	//// H： 单应性矩阵
	//Matx33f H = M * T;
	//Matx31f b = Matx31f{ H(0, 2), H(1,2), H(2,2) } *-1;
	//转换为		|h11 h12  -u|		 | -h13 |
	//			|h21  h22  -v| *X =  | -h23|
	//			|h32  h32  -1|		 |-h33 |
	//Matx33f A = Matx33f{ H(0,0), H(0,1), q(0,0) * -1,
	//					H(1,0), H(1,1), q(1,0) * -1,
	//					H(2,0), H(2,1), q(2,0) * -1
	//};
	//Matx31f X;
	//solve(A, b, X, DECOMP_LU);

	return 0;
}