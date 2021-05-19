#include"PlaneFitting.h"

LinePlane::LinePlane()
{
}

LinePlane::LinePlane(std::vector<Point3d> points, int label) : m_points{ points }, m_label{ label }
{
	coeffient = Mat(3, 1, CV_64F, Scalar(0));
	planeFitting();
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
	Mat sumXi2 = xi * xi.t();
	Mat sumXiYi = xi * yi.t();
	Mat sumYi2 = yi * yi.t();
	Mat sumXiZi = xi * zi.t();
	Mat sumYiZi = yi * zi.t();

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
	cv::solve(A, b, coeffient, DECOMP_QR);
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