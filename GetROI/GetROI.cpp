// GetROI.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

using namespace cv;
using std::endl;
using std::cout;

//CheckMode: 0代表去除黑区域，1代表去除白区域; NeihborMode：0代表4邻域，1代表8邻域;
void RemoveSmallRegion(Mat& Src, Mat& Dst, int AreaLimit, int CheckMode, int NeihborMode)
{
	int RemoveCount = 0;       //记录除去的个数
	//记录每个像素点检验状态的标签，0代表未检查，1代表正在检查,2代表检查不合格（需要反转颜色），3代表检查合格或不需检查
	Mat Pointlabel = Mat::zeros(Src.size(), CV_8UC1);

	if (CheckMode == 1)
	{
		cout << "Mode: 去除小区域. ";
		for (int i = 0; i < Src.rows; ++i)
		{
			uchar* iData = Src.ptr<uchar>(i);
			uchar* iLabel = Pointlabel.ptr<uchar>(i);
			for (int j = 0; j < Src.cols; ++j)
			{
				if (iData[j] < 10)
				{
					iLabel[j] = 3;
				}
			}
		}
	}
	else
	{
		cout << "Mode: 去除孔洞. ";
		for (int i = 0; i < Src.rows; ++i)
		{
			uchar* iData = Src.ptr<uchar>(i);
			uchar* iLabel = Pointlabel.ptr<uchar>(i);
			for (int j = 0; j < Src.cols; ++j)
			{
				if (iData[j] > 10)
				{
					iLabel[j] = 3;
				}
			}
		}
	}

	std::vector<Point2i> NeihborPos;  //记录邻域点位置
	NeihborPos.push_back(Point2i(-1, 0));
	NeihborPos.push_back(Point2i(1, 0));
	NeihborPos.push_back(Point2i(0, -1));
	NeihborPos.push_back(Point2i(0, 1));
	if (NeihborMode == 1)
	{
		cout << "Neighbor mode: 8邻域." << endl;
		NeihborPos.push_back(Point2i(-1, -1));
		NeihborPos.push_back(Point2i(-1, 1));
		NeihborPos.push_back(Point2i(1, -1));
		NeihborPos.push_back(Point2i(1, 1));
	}
	else cout << "Neighbor mode: 4邻域." << endl;
	int NeihborCount = 4 + 4 * NeihborMode;
	int CurrX = 0, CurrY = 0;
	//开始检测
	for (int i = 0; i < Src.rows; ++i)
	{
		uchar* iLabel = Pointlabel.ptr<uchar>(i);
		for (int j = 0; j < Src.cols; ++j)
		{
			if (iLabel[j] == 0)
			{
				//********开始该点处的检查**********
				std::vector<Point2i> GrowBuffer;                                      //堆栈，用于存储生长点
				GrowBuffer.push_back(Point2i(j, i));
				Pointlabel.at<uchar>(i, j) = 1;
				int CheckResult = 0;                                               //用于判断结果（是否超出大小），0为未超出，1为超出

				for (int z = 0; z < GrowBuffer.size(); z++)
				{
					for (int q = 0; q < NeihborCount; q++)                                      //检查四个邻域点
					{
						CurrX = GrowBuffer.at(z).x + NeihborPos.at(q).x;
						CurrY = GrowBuffer.at(z).y + NeihborPos.at(q).y;
						if (CurrX >= 0 && CurrX < Src.cols && CurrY >= 0 && CurrY < Src.rows)  //防止越界
						{
							if (Pointlabel.at<uchar>(CurrY, CurrX) == 0)
							{
								GrowBuffer.push_back(Point2i(CurrX, CurrY));  //邻域点加入buffer
								Pointlabel.at<uchar>(CurrY, CurrX) = 1;           //更新邻域点的检查标签，避免重复检查
							}
						}
					}
				}
				if (GrowBuffer.size() > AreaLimit) CheckResult = 2;                 //判断结果（是否超出限定的大小），1为未超出，2为超出
				else { CheckResult = 1;   RemoveCount++; }
				for (int z = 0; z < GrowBuffer.size(); z++)                         //更新Label记录
				{
					CurrX = GrowBuffer.at(z).x;
					CurrY = GrowBuffer.at(z).y;
					Pointlabel.at<uchar>(CurrY, CurrX) += CheckResult;
				}
				//********结束该点处的检查**********
			}
		}
	}

	CheckMode = 255 * (1 - CheckMode);
	//开始反转面积过小的区域
	for (int i = 0; i < Src.rows; ++i)
	{
		uchar* iData = Src.ptr<uchar>(i);
		uchar* iDstData = Dst.ptr<uchar>(i);
		uchar* iLabel = Pointlabel.ptr<uchar>(i);
		for (int j = 0; j < Src.cols; ++j)
		{
			if (iLabel[j] == 2)
			{
				iDstData[j] = CheckMode;
			}
			else if (iLabel[j] == 3)
			{
				iDstData[j] = iData[j];
			}
		}
	}

	cout << RemoveCount << " objects removed." << endl;
}

int main(int argc, char** argv)
{
	double time1 = static_cast<double>(getTickCount());

	Mat src = imread("../Picture/I8.bmp", CV_8UC1);
	//GaussianBlur(src, src, Size(3, 3), 0);
	//imshow("源图", src);
	Mat topHatImage;
	Mat element = getStructuringElement(MORPH_ELLIPSE, Size(17, 17));
	morphologyEx(src, topHatImage, MORPH_TOPHAT, element);
	//imshow("形态学操作后", topHatImage);
	//自适应二值化
	Mat dst;
	adaptiveThreshold(topHatImage, dst, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 7, 0);
	//imshow("阈值分割", dst);
	RemoveSmallRegion(dst, dst, 50, 1, 0);
	imshow("remove small region", dst);
	//imwrite("region.bmp", dst);
	Mat labels, stats, centroids;
	int i;
	size_t nccomps = connectedComponentsWithStats(dst, labels, stats, centroids);
	cout << "Total Connected Components Detected: " << nccomps << endl;

	std::vector<cv::Vec3b> colors(nccomps + 1);
	colors[0] = Vec3b(0, 0, 0); // background pixels remain black.
	for (i = 1; i < nccomps; i++)
	{
		cout << "连通域大小" << stats.at<int>(i, cv::CC_STAT_AREA) << endl;
		colors[i] = Vec3b(255, 255, 255);
		cout << (int)colors[i][0] << " " << (int)colors[i][1] << " " << (int)colors[i][2] << endl;
		if (stats.at<int>(i, cv::CC_STAT_AREA) < 20000)
		{
			colors[i] = Vec3b(0, 0, 0); // small regions are painted with black too.
		}
	}
	Mat img_color = Mat::zeros(dst.size(), CV_8UC3);
	for (int y = 0; y < img_color.rows; y++)
	{
		for (int x = 0; x < img_color.cols; x++)
		{
			int label = labels.at<int>(y, x);
			CV_Assert(0 <= label && label <= nccomps);
			img_color.at<cv::Vec3b>(y, x) = colors[label];
		}
	}
	cv::imshow("Labeled map", img_color);

	Mat img_binary;
	cvtColor(img_color, img_binary, COLOR_RGB2GRAY);
	//imwrite("region.bmp", img_binary);
	double time2 = (static_cast<double>(getTickCount()) - time1) / getTickFrequency();
	cout << time2 << "ms" << endl;
	waitKey(0);
	return 0;
}