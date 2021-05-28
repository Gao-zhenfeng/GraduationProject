// GetROI.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

using namespace cv;
using std::endl;
using std::cout;

int main(int argc, char** argv)
{
	double time1 = static_cast<double>(getTickCount());

	Mat src = imread("E:\\课程资料\\毕设\\Program\\Graduation_Project\\Data\\20210526\\l3.bmp", CV_8UC1);

	Mat topHatImage;
	Mat element = getStructuringElement(MORPH_ELLIPSE, Size(10, 10));
	morphologyEx(src, topHatImage, MORPH_TOPHAT, element);

	//自适应二值化
	Mat dst;
	adaptiveThreshold(topHatImage, dst, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 11, 0);
	// 对图像进行开操作，断开狭窄的间断和消除细的突出物
	//Mat element2 = getStructuringElement(MORPH_RECT, Size(2, 2));
	//// 对img_binary膨胀，方便后面对灰度光条区域进行提取
	//morphologyEx(dst, dst, MORPH_ERODE, element2);
	//medianBlur(dst, dst, 3);

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

	//Mat dilateImgBinary;
	//Mat element3 = getStructuringElement(MORPH_ELLIPSE, Size(4, 4));
	//// 对img_binary膨胀，方便后面对灰度光条区域进行提取
	//morphologyEx(img_binary, dilateImgBinary, MORPH_OPEN, element3);

	//imwrite("E:\\课程资料\\毕设\\Program\\Graduation_Project\\Data\\20210522a\\helmetROI.bmp", img_binary);

	double time2 = (static_cast<double>(getTickCount()) - time1) / getTickFrequency();
	cout << time2 << "s" << endl;
	waitKey(0);
	return 0;
}