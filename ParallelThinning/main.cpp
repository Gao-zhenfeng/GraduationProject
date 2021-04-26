#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "Thinning.h"
#include "dbscan.h"

#define MINIMUM_POINTS 3     // minimum number of cluster
#define EPSILON  (30 * 30)  // distance for clustering, metre^2

using namespace cv;
using std::endl;
using std::cout;

int g_threshold = 80;
int g_mimLineLength = 26;
int g_maxLineGap = 24;

struct HoughLine
{
	std::string m_winname;
	Mat m_src;
	std::vector<Vec4i> m_linesP;
	HoughLine(std::string winname, Mat& src, std::vector<Vec4i>& linesP)
		: m_winname(winname), m_src(src), m_linesP(linesP)
	{
	}
};

bool comp(const Point& p1, const Point& p2)
{
	return p1.x < p2.x;
}

void on_threshold1(int bar_val, void* userdata)
{
	std::filesystem::path p{ "k&&b.txt" };
	std::ofstream fout{ p };
	HoughLine hL = *(HoughLine*)(userdata);
	Mat cdstP;
	cvtColor(hL.m_src, cdstP, COLOR_GRAY2BGR);
	HoughLinesP(hL.m_src, hL.m_linesP, 1, CV_PI / 180, g_threshold, g_mimLineLength, g_maxLineGap); // runs the actual detection
	cout << "lines size: " << hL.m_linesP.size() << endl;
	//std::vector<Vec6d> lineData; // 存储点、斜率、截距
	std::vector<classifiedPoint> cPoints;
	// Draw the lines
	for (size_t i = 0; i < hL.m_linesP.size(); i++)
	{
		Vec4i l = hL.m_linesP[i];
		circle(cdstP, Point(l[0], l[1]), 5, Scalar(0, 255, 0), -1);
		circle(cdstP, Point(l[2], l[3]), 5, Scalar(0, 255, 0), -1);
		line(cdstP, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 1, LINE_AA);
		if ((l[2] - l[0]) != 0)
		{
			// 需要对数据归一化
			float k = (float(l[3]) - l[1]) / (l[2] - l[0]);
			float b = (float(l[1]) * l[2] - l[0] * l[3]) / (l[2] - l[0]) / 1.0f;
			cout << " k: " << k << "  " << "b: " << b << endl;
			fout << k << "  " << b << "  " << l[0] << "  " << l[1] << "  " << l[2] << "  " << l[3] << endl;
			//lineData.push_back(Vec6d{ double(l[0]), double(l[1]), double(l[2]), double(l[3]), k, b });
			cPoints.push_back(classifiedPoint{ Point2f{k, b} });
		}
	}
	fout.close();
	// 给直线归类
	//DBSCAN ds{ MINIMUM_POINTS, EPSILON, cPoints };
	//ds.run();
	//std::vector<classifiedPoint> classifiedPoints = ds.getPoints();
	//std::vector<std::vector<cv::Point> > linePointsVector;
	//std::size_t clusters = ds.getNumOfCluster();
	//// i为点集的组号,本循环目的为将结构光上的点存在一个向量里，多条结构光构成向量的向量
	//for (size_t i = 1; i <= clusters; i++)
	//{
	//	std::vector<cv::Point> linesPoints;
	//	for (size_t j = 0; j < classifiedPoints.size(); j++)
	//	{
	//		if (classifiedPoints[j].clusterID == i)
	//		{
	//			Vec4i l = hL.m_linesP[j];
	//			linesPoints.push_back(Point{ l[0], l[1] });
	//			linesPoints.push_back(Point{ l[2], l[3] });
	//			std::cout << std::setprecision(3) << std::fixed << "k: " << classifiedPoints[j].p.x << "  ";
	//			std::cout << std::setprecision(3) << std::fixed << "b: " << classifiedPoints[j].p.y << "  ";
	//			std::cout << "clusterID" << classifiedPoints[j].clusterID << endl;
	//		}
	//	}

	//	linePointsVector.push_back(linesPoints);
	//	linesPoints.~vector();
	//}

	//for (size_t i = 0; i < linePointsVector.size() - 1; i++)
	//{
	//	Point maxPoint = *std::max_element(linePointsVector[i].begin(), linePointsVector[i].end(), comp);
	//	Point minPoint = *std::min_element(linePointsVector[i].begin(), linePointsVector[i].end(), comp);
	//	line(cdstP, maxPoint, minPoint, Scalar(0, 0, 255), 2, LINE_AA);
	//	circle(cdstP, maxPoint, 5, Scalar(0, 255, 0), -1);
	//	circle(cdstP, minPoint, 5, Scalar(0, 255, 0), -1);
	//	//for (size_t j = 0; j < linePointsVector[i].size(); j++)
	//	//{
	//	//	circle(cdstP, linePointsVector[i][j], 5, Scalar(0, 255, 0), -1);
	//	//}
	//}
	imshow(hL.m_winname, cdstP);
}

void on_threshold2(int bar_val, void* userdata)
{
	std::filesystem::path p{ "k&&b.txt" };
	std::ofstream fout{ p };
	HoughLine hL = *(HoughLine*)(userdata);
	Mat cdstP;
	cvtColor(hL.m_src, cdstP, COLOR_GRAY2BGR);
	HoughLinesP(hL.m_src, hL.m_linesP, 1, CV_PI / 180, g_threshold, g_mimLineLength, g_maxLineGap); // runs the actual detection
	cout << "lines size: " << hL.m_linesP.size() << endl;
	std::vector<classifiedPoint> cPoints;
	// Draw the lines
	for (size_t i = 0; i < hL.m_linesP.size(); i++)
	{
		Vec4i l = hL.m_linesP[i];
		line(cdstP, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 1, LINE_AA);
		if ((l[2] - l[0]) != 0)
		{
			float k = (float(l[3]) - l[1]) / (l[2] - l[0]) / 1.0;
			float b = (float(l[1]) * l[2] - l[0] * l[3]) / (l[2] - l[0]) / 1.0;
			fout << k << "  " << b << endl;
			//lineData.push_back(Vec6d{ double(l[0]), double(l[1]), double(l[2]), double(l[3]), k, b });
			cPoints.push_back(classifiedPoint{ Point2f{k, b} });
		}
	}
	fout.close();

	// 给直线归类
	DBSCAN ds{ MINIMUM_POINTS, EPSILON, cPoints };
	ds.run();
	std::vector<classifiedPoint> classifiedPoints = ds.getPoints();

	for (size_t i = 1; i <= ds.getNumOfCluster(); i++)
	{
		for (size_t j = 0; j < classifiedPoints.size(); j++)
		{
			if (classifiedPoints[j].clusterID == i)
			{
				std::cout << std::setprecision(3) << std::fixed << "k: " << classifiedPoints[j].p.x << "  ";
				std::cout << std::setprecision(3) << std::fixed << "b: " << classifiedPoints[j].p.y << "  ";
				std::cout << "clusterID" << classifiedPoints[j].clusterID << endl;
			}
		}
	}

	imshow(hL.m_winname, cdstP);
}

void on_threshold3(int bar_val, void* userdata)
{
	std::filesystem::path p{ "k&&b.txt" };
	std::ofstream fout{ p };
	HoughLine hL = *(HoughLine*)(userdata);
	Mat cdstP;
	cvtColor(hL.m_src, cdstP, COLOR_GRAY2BGR);
	HoughLinesP(hL.m_src, hL.m_linesP, 1, CV_PI / 180, g_threshold, g_mimLineLength, g_maxLineGap); // runs the actual detection
	cout << "lines size: " << hL.m_linesP.size() << endl;
	std::vector<classifiedPoint> cPoints;
	// Draw the lines
	for (size_t i = 0; i < hL.m_linesP.size(); i++)
	{
		Vec4i l = hL.m_linesP[i];
		line(cdstP, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 1, LINE_AA);
		if ((l[2] - l[0]) != 0)
		{
			float k = (float(l[3]) - l[1]) / (l[2] - l[0]) / 1.0;
			float b = (float(l[1]) * l[2] - l[0] * l[3]) / (l[2] - l[0]) / 1.0;
			fout << k << "  " << b << endl;

			//lineData.push_back(Vec6d{ double(l[0]), double(l[1]), double(l[2]), double(l[3]), k, b });
			cPoints.push_back(classifiedPoint{ Point2f{k, b} });
		}
	}
	fout.close();
	// 给直线归类
	DBSCAN ds{ MINIMUM_POINTS, EPSILON, cPoints };
	ds.run();
	std::vector<classifiedPoint> classifiedPoints = ds.getPoints();
	for (size_t i = 1; i <= ds.getNumOfCluster(); i++)
	{
		for (size_t j = 0; j < classifiedPoints.size(); j++)
		{
			if (classifiedPoints[j].clusterID == i)
			{
				std::cout << std::setprecision(3) << std::fixed << "k: " << classifiedPoints[j].p.x << "  ";
				std::cout << std::setprecision(3) << std::fixed << "b: " << classifiedPoints[j].p.y << "  ";
				std::cout << "clusterID" << classifiedPoints[j].clusterID << endl;
			}
		}
	}

	imshow(hL.m_winname, cdstP);
}

int main(int argc, char** argv)
{
	double time1 = static_cast<double>(getTickCount());
	Mat src = imread("region.bmp", CV_8UC1);
	//Rect r1{ 307,188,638,684 };
	//Mat mask = Mat::zeros(src.size(), CV_8UC1);
	if (src.empty())
	{
		cout << "image is empty. " << endl;
		return -1;
	}
	namedWindow("源图像", WINDOW_NORMAL);
	imshow("源图像", src);
	/*mask(r1).setTo(255);
	Mat maskSrc;
	src.copyTo(maskSrc, mask);*/
	Mat dst = src / 255;
	//medianBlur(src, dst, 3);
	//adaptiveThreshold(dst, dst, 1, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 7, 0);
	//medianBlur(dst, dst, 5);

	//namedWindow("自适应二值化后： ", WINDOW_NORMAL);
	//imshow("自适应二值化后： ", dst * 255);

	Mat thinsrc;
	thinsrc = thinImage(dst);
	filterOver(thinsrc);
	Mat thinImage = thinsrc * 255;

	namedWindow("细化后： ", WINDOW_NORMAL);
	imshow("细化后： ", thinImage);

	//std::string winname = "Detected Lines (in red) - Probabilistic Line Transform";
	//namedWindow(winname, WINDOW_NORMAL);
	// Probabilistic Line Transform
	std::vector<Vec4i> linesP;
	//dst = dst * 255;
	//HoughLine hL{ winname, thinImage, linesP };

	std::vector<KeyPoint> roughKeyPoints = getPoints(thinsrc, 7, 7, 0);
	std::vector<KeyPoint>keyPoints = getKeyPoints(roughKeyPoints, 15);

	//绘制keyPoints
	Mat keyPointsImage;
	drawKeypoints(thinImage, keyPoints, keyPointsImage, Scalar(0, 0, 255));
	namedWindow("keyPoints", WINDOW_NORMAL);
	imshow("keyPoints", keyPointsImage);

	//std::filesystem::path p{ "keypoint.txt" };
	//std::ofstream fout{ p };

	//for (size_t i = 0; i < keyPoints.size(); i++)
	//{
	//	fout << keyPoints[i].pt.x << "  " << keyPoints[i].pt.y << endl;
	//}
	//fout.close();

	//float k = getK(thinImage, linesP, g_threshold, g_mimLineLength, g_maxLineGap);
	std::vector<LineData> lines = getLineData(keyPoints, 20);

	Mat rgbSRC;
	cvtColor(src, rgbSRC, COLOR_GRAY2BGR);

	for (size_t i = 0; i < lines.size(); i++)
	{
		for (size_t j = 0; j < lines[i].m_points.size(); j++)
		{
			String s = "(" + std::to_string(lines[i].m_label) + "," + std::to_string(j) + ")";
			cout << lines[i].m_points[j].x << "  " << lines[i].m_points[j].y << "  "
				<< lines[i].m_k << "  " << lines[i].m_b << "  " << lines[i].m_label << endl;

			circle(rgbSRC, lines[i].m_points[j], 1, Scalar(255, 0, 0), -1);
			putText(rgbSRC, s, lines[i].m_points[j], FONT_HERSHEY_SIMPLEX, 0.25, Scalar(0, 0, 255), 1, LINE_AA);//在图片上写文字
		}
	}
	namedWindow("光条编码", WINDOW_NORMAL);
	imshow("光条编码", rgbSRC);
	//imwrite("labeled.bmp", rgbSRC);
	double time2 = (static_cast<double>(getTickCount()) - time1) / getTickFrequency();
	cout << time2 << "ms" << endl;
	waitKey(0);
	return 0;
}