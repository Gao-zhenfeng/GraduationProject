#include"PlaneFitting.h"

LinePlane::LinePlane()
{
}

LinePlane::LinePlane(std::vector<Point3d> points, int label) : m_label{ label }
{
	coeffient = Mat(3, 1, CV_64F, Scalar(0));
	m_points.insert(m_points.end(), points.begin(), points.end());
}

// 对lines上二维点，求其三维坐标，
LinePlane::LinePlane(LineData& lines, Matx33d M, Matx41d distCoeffs, Matx13d R, Matx13d t)
{
	this->m_line = lines;
	this->m_label = lines.m_label;
	std::vector<Point2d> UndistortPoints;
	undistortPoints(lines.m_points, UndistortPoints, M, distCoeffs, cv::noArray(), M);
	Matx33d RMatrix;
	Rodrigues(R, RMatrix);

	/*求解如下方程组：为求 zc xw yw
			|u|		  |xw|		   |u|			|xw|
		zc	|v| = H * |yw|     q = |v|		X = |yw|
			|1|		  |1 |		   |1|			|zc|

转换为		|h11  h12  -u|		 |-h13|
			|h21  h22  -v| * X = |-h23|    Ax = b
			|h31  h32  -1|		 |-h33|
*/

	double c = RMatrix(0, 2) * t(0, 0) + RMatrix(1, 2) * t(0, 1) + RMatrix(2, 2) * t(0, 2);
	Matx41d b = Matx41d{ 0, 0, 0,  c };
	int sizeUts = UndistortPoints.size();
	for (int i = 0; i < sizeUts; i++)
	{
		double u = UndistortPoints[i].x;
		double v = UndistortPoints[i].y;
		Matx44d A = Matx44d{
							M(0, 0), M(0, 1), M(0, 2), -u,
							M(1, 0), M(1, 1), M(1, 2), -v,
							M(2, 0), M(2, 1), M(2, 2), -1,
							RMatrix(0, 2), RMatrix(1, 2), RMatrix(2, 2), 0
		};
		Matx41d X;
		solve(A, b, X);
		double xc = X(0, 0);
		double yc = X(1, 0);
		double zc = X(2, 0);
		Point3d p3 = Point3d{ xc, yc, zc };
		this->m_points.push_back(p3);
	}

	//fs::path p1{ "cornerdata.txt" };
	//std::ofstream out1{ p1, std::ios::out | std::ios::app };
	//size_t numOfPoints = lines.m_points.size();
	//for (size_t i = 0; i < numOfPoints; i++)
	//{
	//	out1 << std::setprecision(6) << std::fixed << lines.m_points[i].x << "    " << lines.m_points[i].y << "    "
	//		<< lines.m_label << "    " << i << "    " << m_points[i].x << "    " << m_points[i].y
	//		<< "    " << m_points[i].z << endl;
	//}
	//out1.close();
}

void LinePlane::addPoints(LineData& lines, Matx33d M, Matx41d distCoeffs, Matx13d R, Matx13d t)
{
	this->m_line = lines;
	this->m_label = lines.m_label;
	std::vector<Point2d> UndistortPoints;
	undistortPoints(lines.m_points, UndistortPoints, M, distCoeffs, cv::noArray(), M);
	Matx33d RMatrix;
	Rodrigues(R, RMatrix);

	/*求解如下方程组：为求 zc xw yw
			|u|		  |xw|		   |u|			|xw|
		zc	|v| = H * |yw|     q = |v|		X = |yw|
			|1|		  |1 |		   |1|			|zc|

转换为		|h11  h12  -u|		 |-h13|
			|h21  h22  -v| * X = |-h23|    Ax = b
			|h31  h32  -1|		 |-h33|
*/

	double c = RMatrix(0, 2) * t(0, 0) + RMatrix(1, 2) * t(0, 1) + RMatrix(2, 2) * t(0, 2);
	Matx41d b = Matx41d{ 0, 0, 0,  c };
	int sizeUts = UndistortPoints.size();
	for (int i = 0; i < sizeUts; i++)
	{
		double u = UndistortPoints[i].x;
		double v = UndistortPoints[i].y;
		Matx44d A = Matx44d{
							M(0, 0), M(0, 1), M(0, 2), -u,
							M(1, 0), M(1, 1), M(1, 2), -v,
							M(2, 0), M(2, 1), M(2, 2), -1,
							RMatrix(0, 2), RMatrix(1, 2), RMatrix(2, 2), 0
		};
		Matx41d X;
		solve(A, b, X);
		double xc = X(0, 0);
		double yc = X(1, 0);
		double zc = X(2, 0);
		Point3d p3 = Point3d{ xc, yc, zc };
		this->m_points.push_back(p3);
	}
}

void LinePlane::addPointsAndPrint(LineData& lines, Matx33d M, Matx41d distCoeffs, Matx13d R, Matx13d t, std::ofstream& out)
{
	this->m_line = lines;
	this->m_label = lines.m_label;
	std::vector<Point2d> UndistortPoints;
	undistortPoints(lines.m_points, UndistortPoints, M, distCoeffs, cv::noArray(), M);
	Matx33d RMatrix;
	Rodrigues(R, RMatrix);

	double c = RMatrix(0, 2) * t(0, 0) + RMatrix(1, 2) * t(0, 1) + RMatrix(2, 2) * t(0, 2);
	Matx41d b = Matx41d{ 0, 0, 0,  c };
	int sizeUts = UndistortPoints.size();
	for (int i = 0; i < sizeUts; i++)
	{
		double u = UndistortPoints[i].x;
		double v = UndistortPoints[i].y;
		Matx44d A = Matx44d{
							M(0, 0), M(0, 1), M(0, 2), -u,
							M(1, 0), M(1, 1), M(1, 2), -v,
							M(2, 0), M(2, 1), M(2, 2), -1,
							RMatrix(0, 2), RMatrix(1, 2), RMatrix(2, 2), 0
		};
		Matx41d X;
		solve(A, b, X);
		double xc = X(0, 0);
		double yc = X(1, 0);
		double zc = X(2, 0);
		Point3d p3 = Point3d{ xc, yc, zc };
		this->m_points.push_back(p3);
		out << std::setprecision(9) << std::fixed << lines.m_points[i].x << "    " << lines.m_points[i].y << "    " //u, v
			<< lines.m_label << "    " << i << "    "	//xindex, yindex
			<< xc << "    " << yc << "    " << zc << endl;	//x, y, z
	}
}

void LinePlane::planeFitting()
{
	//求解超定方程 Ax = b
	size_t npoints = this->m_points.size();
	Mat xi = Mat::zeros(1, npoints, CV_64F);
	Mat yi = Mat::zeros(1, npoints, CV_64F);
	Mat zi = Mat::zeros(1, npoints, CV_64F);
	Mat A = Mat(3, 3, CV_64F, Scalar(0));//初始化系数矩阵A
	Mat b = Mat(3, 1, CV_64F, Scalar(0));//初始化矩阵b
	// 初始化（xi,y1,zi)
	for (size_t i = 0; i < npoints; i++)
	{
		xi.at<double>(0, i) = m_points[i].x;
		yi.at<double>(0, i) = m_points[i].y;
		zi.at<double>(0, i) = m_points[i].z;
	}
	Mat sumXi2 = xi * xi.t();// xi * xi
	Mat sumXiYi = xi * yi.t();// xi * yi
	Mat sumYi2 = yi * yi.t();// yi * yi
	Mat sumXiZi = xi * zi.t();//xi * zi
	Mat sumYiZi = yi * zi.t();// yi * zi

	double sumXi = sum(xi)[0];
	double sumYi = sum(yi)[0];
	double sumZi = sum(zi)[0];

	A.at<double>(0, 0) = sumXi2.at<double>(0, 0);
	A.at<double>(0, 1) = sumXiYi.at<double>(0, 0);
	A.at<double>(0, 2) = sumXi;

	A.at<double>(1, 0) = sumXiYi.at<double>(0, 0);
	A.at<double>(1, 1) = sumYi2.at<double>(0, 0);
	A.at<double>(1, 2) = sumYi;

	A.at<double>(2, 0) = sumXi;
	A.at<double>(2, 1) = sumYi;
	A.at<double>(2, 2) = npoints;

	b.at<double>(0, 0) = sumXiZi.at<double>(0, 0);
	b.at<double>(1, 0) = sumYiZi.at<double>(0, 0);
	b.at<double>(2, 0) = sumZi;
	//cout << "A:\n" << A << endl;
	//cout << "b:\n" << b << endl;

	//solve(A, b, coeffient, DECOMP_SVD);
	cv::solve(A, b, this->coeffient, DECOMP_SVD);
}

// 将 { x y z} 以此顺序打印出来
void LinePlane::printTXT(string filename)
{
	fs::path p1{ filename };
	std::ofstream out1{ p1, std::ios::out };
	size_t numOfPoints = m_points.size();

	for (size_t i = 0; i < numOfPoints; i++)
	{
		out1 << std::setprecision(9) << std::fixed << m_points[i].x
			<< "    " << m_points[i].y << "    " << m_points[i].z << endl;
	}
	out1.close();
}

Mat ReadMatFromTxt(string filename, int rows, int cols)
{
	double m;
	Mat out = Mat::zeros(rows, cols, CV_64F);//Matrix to store values

	std::ifstream fileStream(filename);
	int cnt = 0;//index starts from 0
	while (fileStream >> m)
	{
		int temprow = cnt / cols;
		int tempcol = cnt % cols;
		out.at<double>(temprow, tempcol) = m;
		cnt++;
	}
	return out;
}