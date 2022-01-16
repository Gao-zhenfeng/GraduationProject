#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "Thinning.h"

using namespace cv;
using std::endl;
using std::cout;

int main(int argc, char** argv)
{
	double time1 = static_cast<double>(getTickCount());

	Mat src = imread("E:\\课程资料\\毕设\\Program\\Graduation_Project\\Data\\0.5mm\\light\\l1.bmp", CV_8UC1);
	medianBlur(src, src, 3);

	Mat topHatImage;
	Mat element = getStructuringElement(MORPH_ELLIPSE, Size(15, 15));
	morphologyEx(src, topHatImage, MORPH_TOPHAT, element);
	for (size_t i = 0; i < topHatImage.rows; i++)
	{
		for (size_t j = 0; j < topHatImage.cols; j++)
		{
			if (topHatImage.at<uchar>(i, j) < 5)
			{
				topHatImage.at<uchar>(i, j) = 0;
			}
		}
	}
	//自适应二值化
	Mat dst;
	adaptiveThreshold(topHatImage, dst, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 9, 0);

	//// 对图像进行开操作，断开狭窄的间断和消除细的突出物
	Mat element2 = getStructuringElement(MORPH_RECT, Size(3, 3));
	////// 对img_binary膨胀，方便后面对灰度光条区域进行提取
	morphologyEx(dst, dst, MORPH_CLOSE, element2);

	Mat labels, stats, centroids;
	int i;
	size_t nccomps = connectedComponentsWithStats(dst, labels, stats, centroids);
	cout << "Total Connected Components Detected: " << nccomps << endl;

	std::vector<uchar> colors(nccomps + 1);
	colors[0] = (uchar)0; // background pixels remain black.
	for (i = 1; i < nccomps; i++)
	{
		//cout << "连通域大小" << stats.at<int>(i, cv::CC_STAT_AREA) << endl;
		colors[i] = (uchar)255;

		if (stats.at<int>(i, cv::CC_STAT_AREA) < 30000)
		{
			colors[i] = (uchar)0; // small regions are painted with black too.
		}
	}
	Mat img_binary = Mat::zeros(dst.size(), CV_8UC1);
	for (int y = 0; y < img_binary.rows; y++)
	{
		for (int x = 0; x < img_binary.cols; x++)
		{
			int label = labels.at<int>(y, x);
			CV_Assert(0 <= label && label <= nccomps);
			img_binary.at<uchar>(y, x) = colors[label];
		}
	}

	// 对图像进行开操作，断开狭窄的间断和消除细的突出物
	Mat element3 = getStructuringElement(MORPH_RECT, Size(5, 5));
	//// 对img_binary膨胀，方便后面对灰度光条区域进行提取
	morphologyEx(img_binary, img_binary, MORPH_CLOSE, element3);

	/*mask(r1).setTo(255);
	Mat maskSrc;
	src.copyTo(maskSrc, mask);*/

	dst = img_binary / 255;

	Mat thinsrc;
	cvThin(dst, thinsrc, 5);
	filterOver(thinsrc);
	Mat thinImage = thinsrc * 255;

	//Mat maskImage;
	//Point2d p = Point2d{ ,  };
	//Point2d nextPoint;
	//while (updatePoint(src, maskImage, p, nextPoint))
	//{
	//	//更新p点坐标
	//	p.x = nextPoint.x;
	//	p.y = nextPoint.y;
	//}

	//对横纵光条进行分类
	std::vector<LineData> LINE = classifyHorizonLines(thinImage);
	std::vector<LineData> LINE2 = classifyVerticalLines(thinImage);
	LINE.insert(LINE.end(), LINE2.begin(), LINE2.end());
	Mat temp;
	cvtColor(src, temp, COLOR_GRAY2RGB);
	int numLINE = LINE.size();
	for (size_t i = 0; i < numLINE; i++)
	{
		for (size_t j = 0; j < LINE[i].m_points.size(); j++)
		{
			int u0 = LINE[i].m_points[j].x;
			int v0 = LINE[i].m_points[j].y;

			temp.at<Vec3b>(v0, u0) = Vec3b{ 47, 0, uchar(i * 5) };
		}
	}
	imwrite("temp.bmp", temp);

	//Mat rgbSrc;
	//cvtColor(src, rgbSrc, COLOR_GRAY2RGB);
	//for (int i = 0; i < rgbSrc.rows; i++)
	//{
	//	for (int j = 0; j < rgbSrc.cols; j++)
	//	{
	//		if (thinImage.at<uchar>(i, j) > 10)
	//		{
	//			rgbSrc.at<Vec3b>(i, j) = Vec3b{ 0, 0, 255 };
	//		}
	//	}
	//}
	/*std::vector<Point2f> corners;
	double qualityLevel = 0.01;
	double minDistance = 10;
	int blockSize = 5, gradientSize = 3;
	bool useHarrisDetector = false;
	double k = 0.04;
	goodFeaturesToTrack(thinImage,
		corners,
		1000,
		qualityLevel,
		minDistance,
		Mat(),
		blockSize,
		gradientSize,
		useHarrisDetector,
		k);
	cout << "** Number of corners detected: " << corners.size() << endl;
	int radius = 2;
	Mat copy = src.clone();
	for (size_t i = 0; i < corners.size(); i++)
	{
		circle(copy, corners[i], radius, Scalar(0, 0, 255), FILLED);
	}
	imshow("harris corner", src);*/

	std::vector<Point2f> roughKeyPoints = getPoints(thinsrc, 5, 6, 0);
	Mat roughKeyPointsImage;
	//drawKeypoints(src, roughKeyPoints, roughKeyPointsImage);
	std::vector<Point2f>keyPoints = getKeyPoints(roughKeyPoints, 10);

	//绘制keyPoints
	Mat keyPointsImage;
	cvtColor(src, keyPointsImage, COLOR_GRAY2RGB);
	for (size_t i = 0; i < keyPoints.size(); i++)
	{
		grayCenter(src, keyPoints[i], 2);
		//preciseCorner(graySrc, keyPoints[i].pt, 2);
		circle(keyPointsImage, keyPoints[i], 1, Scalar(0, 0, 255), FILLED);
	}
	//drawKeypoints(src, keyPoints, keyPointsImage, Scalar(0, 0, 255));

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
			String s = std::to_string(lines[i].m_label);
			String s2 = std::to_string(j);

			//cout << lines[i].m_points[j].x << "  " << lines[i].m_points[j].y << "  "
				//<< lines[i].m_k << "  " << lines[i].m_b << "  " << lines[i].m_label << endl;
			circle(rgbSRC, lines[i].m_points[j], 1, Scalar(255, 0, 0), -1);
			Point2d p;
			p.x = lines[i].m_points[j].x;
			p.y = lines[i].m_points[j].y + 10;
			putText(rgbSRC, s, lines[i].m_points[j], FONT_HERSHEY_SIMPLEX, 0.4, Scalar(0, 0, 255), 1, LINE_AA);//在图片上写文字
			putText(rgbSRC, s2, p, FONT_HERSHEY_SIMPLEX, 0.4, Scalar(0, 0, 255), 1, LINE_AA);//在图片上写文字
		}
	}

	double time2 = (static_cast<double>(getTickCount()) - time1) / getTickFrequency();
	cout << time2 << "s" << endl;
	waitKey(0);
	return 0;
}