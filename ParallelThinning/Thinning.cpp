#include "Thinning.h"

LineData::LineData()
{
	this->m_label = 0;
	this->m_k = 0.0;
	this->m_b = 0.0;
}

LineData::LineData(std::vector<Point> points, int label, float k, float b)
{
	this->m_points = points;
	this->m_label = label;
	this->m_k = k;
	this->m_b = b;
}
// 细化图像画到原图上
Mat drawCornerOnImage(Mat image, const Mat& binary)
{
	cvtColor(image, image, COLOR_GRAY2BGR);
	Mat_<uchar>::const_iterator it = binary.begin<uchar>();
	Mat_<uchar>::const_iterator itd = binary.end<uchar>();
	for (int i = 0; it != itd; it++, i++)
	{
		if (*it)
			circle(image, Point(i % image.cols, i / image.cols), 1, Scalar(0, 0, 255), -1);
	}
	return image;
}

/**
* @brief 对输入图像进行细化,骨骼化
* @param src为输入图像,用cvThreshold函数处理过的8位灰度图像格式，元素中只有0与1,1代表有元素，0代表为空白
* @param maxIterations限制迭代次数，如果不进行限制，默认为-1，代表不限制迭代次数，直到获得最终结果
* @return 为对src细化后的输出图像,格式与src格式相同，元素中只有0与1,1代表有元素，0代表为空白
*/
cv::Mat thinImage(const cv::Mat& src, const int maxIterations)
{
	assert(src.type() == CV_8UC1);
	cv::Mat dst;
	int width = src.cols;//图像列数是宽
	int height = src.rows;//图像行数是高
	src.copyTo(dst);
	int count = 0;  //记录迭代次数
	while (true)
	{
		count++;
		if (maxIterations != -1 && count > maxIterations) //限制次数并且迭代次数到达
			break;
		std::vector<uchar*> mFlag; //用于标记需要删除的点
		//对点标记
		for (int i = 0; i < height; ++i)
		{
			uchar* p = dst.ptr<uchar>(i);
			for (int j = 0; j < width; ++j)
			{
				//如果满足四个条件，进行标记
				//  p9 p2 p3
				//  p8 p1 p4
				//  p7 p6 p5
				uchar p1 = p[j];
				if (p1 != 1) continue;
				uchar p4 = (j == width - 1) ? 0 : *(p + j + 1);
				uchar p8 = (j == 0) ? 0 : *(p + j - 1);
				uchar p2 = (i == 0) ? 0 : *(p - dst.step + j);
				uchar p3 = (i == 0 || j == width - 1) ? 0 : *(p - dst.step + j + 1);
				uchar p9 = (i == 0 || j == 0) ? 0 : *(p - dst.step + j - 1);
				uchar p6 = (i == height - 1) ? 0 : *(p + dst.step + j);
				uchar p5 = (i == height - 1 || j == width - 1) ? 0 : *(p + dst.step + j + 1);
				uchar p7 = (i == height - 1 || j == 0) ? 0 : *(p + dst.step + j - 1);
				if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) >= 2 && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) <= 6)
				{
					int ap = 0;
					if (p2 == 0 && p3 == 1) ++ap;
					if (p3 == 0 && p4 == 1) ++ap;
					if (p4 == 0 && p5 == 1) ++ap;
					if (p5 == 0 && p6 == 1) ++ap;
					if (p6 == 0 && p7 == 1) ++ap;
					if (p7 == 0 && p8 == 1) ++ap;
					if (p8 == 0 && p9 == 1) ++ap;
					if (p9 == 0 && p2 == 1) ++ap;

					if (ap == 1 && p2 * p4 * p6 == 0 && p4 * p6 * p8 == 0)
					{
						//标记
						mFlag.push_back(p + j);
					}
				}
			}
		}

		//将标记的点删除
		for (std::vector<uchar*>::iterator i = mFlag.begin(); i != mFlag.end(); ++i)
		{
			**i = 0;
		}

		//直到没有点满足，算法结束
		if (mFlag.empty())
		{
			break;
		}
		else
		{
			mFlag.clear();//将mFlag清空
		}

		//对点标记
		for (int i = 0; i < height; ++i)
		{
			uchar* p = dst.ptr<uchar>(i);
			for (int j = 0; j < width; ++j)
			{
				//如果满足四个条件，进行标记
				//  p9 p2 p3
				//  p8 p1 p4
				//  p7 p6 p5
				uchar p1 = p[j];
				if (p1 != 1) continue;
				uchar p4 = (j == width - 1) ? 0 : *(p + j + 1);
				uchar p8 = (j == 0) ? 0 : *(p + j - 1);
				uchar p2 = (i == 0) ? 0 : *(p - dst.step + j);
				uchar p3 = (i == 0 || j == width - 1) ? 0 : *(p - dst.step + j + 1);
				uchar p9 = (i == 0 || j == 0) ? 0 : *(p - dst.step + j - 1);
				uchar p6 = (i == height - 1) ? 0 : *(p + dst.step + j);
				uchar p5 = (i == height - 1 || j == width - 1) ? 0 : *(p + dst.step + j + 1);
				uchar p7 = (i == height - 1 || j == 0) ? 0 : *(p + dst.step + j - 1);

				if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) >= 2 && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) <= 6)
				{
					int ap = 0;
					if (p2 == 0 && p3 == 1) ++ap;
					if (p3 == 0 && p4 == 1) ++ap;
					if (p4 == 0 && p5 == 1) ++ap;
					if (p5 == 0 && p6 == 1) ++ap;
					if (p6 == 0 && p7 == 1) ++ap;
					if (p7 == 0 && p8 == 1) ++ap;
					if (p8 == 0 && p9 == 1) ++ap;
					if (p9 == 0 && p2 == 1) ++ap;

					if (ap == 1 && p2 * p4 * p8 == 0 && p2 * p6 * p8 == 0)
					{
						//标记
						mFlag.push_back(p + j);
					}
				}
			}
		}

		//将标记的点删除
		for (std::vector<uchar*>::iterator i = mFlag.begin(); i != mFlag.end(); ++i)
		{
			**i = 0;
		}

		//直到没有点满足，算法结束
		if (mFlag.empty())
		{
			break;
		}
		else
		{
			mFlag.clear();//将mFlag清空
		}
	}
	return dst;
}

void cvThin(cv::Mat& src, cv::Mat& dst, int intera)
{
	if (src.type() != CV_8UC1)
	{
		printf("只能处理二值或灰度图像\n");
		return;
	}
	//非原地操作时候，copy src到dst
	if (dst.data != src.data)
	{
		src.copyTo(dst);
	}

	int i, j, n;
	int width, height;
	width = src.cols - 1;
	//之所以减1，是方便处理8邻域，防止越界
	height = src.rows - 1;
	int step = src.step;
	int  p2, p3, p4, p5, p6, p7, p8, p9;
	uchar* img;
	bool ifEnd;
	int A1;
	cv::Mat tmpimg;
	//n表示迭代次数
	for (n = 0; n < intera; n++)
	{
		dst.copyTo(tmpimg);
		ifEnd = false;
		img = tmpimg.data;
		for (i = 1; i < height; i++)
		{
			img += step;
			for (j = 1; j < width; j++)
			{
				uchar* p = img + j;
				A1 = 0;
				if (p[0] > 0)
				{
					if (p[-step] == 0 && p[-step + 1] > 0) //p2,p3 01模式
					{
						A1++;
					}
					if (p[-step + 1] == 0 && p[1] > 0) //p3,p4 01模式
					{
						A1++;
					}
					if (p[1] == 0 && p[step + 1] > 0) //p4,p5 01模式
					{
						A1++;
					}
					if (p[step + 1] == 0 && p[step] > 0) //p5,p6 01模式
					{
						A1++;
					}
					if (p[step] == 0 && p[step - 1] > 0) //p6,p7 01模式
					{
						A1++;
					}
					if (p[step - 1] == 0 && p[-1] > 0) //p7,p8 01模式
					{
						A1++;
					}
					if (p[-1] == 0 && p[-step - 1] > 0) //p8,p9 01模式
					{
						A1++;
					}
					if (p[-step - 1] == 0 && p[-step] > 0) //p9,p2 01模式
					{
						A1++;
					}
					p2 = p[-step] > 0 ? 1 : 0;
					p3 = p[-step + 1] > 0 ? 1 : 0;
					p4 = p[1] > 0 ? 1 : 0;
					p5 = p[step + 1] > 0 ? 1 : 0;
					p6 = p[step] > 0 ? 1 : 0;
					p7 = p[step - 1] > 0 ? 1 : 0;
					p8 = p[-1] > 0 ? 1 : 0;
					p9 = p[-step - 1] > 0 ? 1 : 0;
					if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) > 1 && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) < 7 && A1 == 1)
					{
						if ((p2 == 0 || p4 == 0 || p6 == 0) && (p4 == 0 || p6 == 0 || p8 == 0)) //p2*p4*p6=0 && p4*p6*p8==0
						{
							dst.at<uchar>(i, j) = 0; //满足删除条件，设置当前像素为0
							ifEnd = true;
						}
					}
				}
			}
		}

		dst.copyTo(tmpimg);
		img = tmpimg.data;
		for (i = 1; i < height; i++)
		{
			img += step;
			for (j = 1; j < width; j++)
			{
				A1 = 0;
				uchar* p = img + j;
				if (p[0] > 0)
				{
					if (p[-step] == 0 && p[-step + 1] > 0) //p2,p3 01模式
					{
						A1++;
					}
					if (p[-step + 1] == 0 && p[1] > 0) //p3,p4 01模式
					{
						A1++;
					}
					if (p[1] == 0 && p[step + 1] > 0) //p4,p5 01模式
					{
						A1++;
					}
					if (p[step + 1] == 0 && p[step] > 0) //p5,p6 01模式
					{
						A1++;
					}
					if (p[step] == 0 && p[step - 1] > 0) //p6,p7 01模式
					{
						A1++;
					}
					if (p[step - 1] == 0 && p[-1] > 0) //p7,p8 01模式
					{
						A1++;
					}
					if (p[-1] == 0 && p[-step - 1] > 0) //p8,p9 01模式
					{
						A1++;
					}
					if (p[-step - 1] == 0 && p[-step] > 0) //p9,p2 01模式
					{
						A1++;
					}
					p2 = p[-step] > 0 ? 1 : 0;
					p3 = p[-step + 1] > 0 ? 1 : 0;
					p4 = p[1] > 0 ? 1 : 0;
					p5 = p[step + 1] > 0 ? 1 : 0;
					p6 = p[step] > 0 ? 1 : 0;
					p7 = p[step - 1] > 0 ? 1 : 0;
					p8 = p[-1] > 0 ? 1 : 0;
					p9 = p[-step - 1] > 0 ? 1 : 0;
					if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) > 1 && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) < 7 && A1 == 1)
					{
						if ((p2 == 0 || p4 == 0 || p8 == 0) && (p2 == 0 || p6 == 0 || p8 == 0)) //p2*p4*p8=0 && p2*p6*p8==0
						{
							dst.at<uchar>(i, j) = 0; //满足删除条件，设置当前像素为0
							ifEnd = true;
						}
					}
				}
			}
		}

		//如果两个子迭代已经没有可以细化的像素了，则退出迭代
		if (!ifEnd) break;
	}
}

/**
* @brief 对骨骼化图数据进行过滤，实现两个点之间至少隔一个空白像素
* @param thinSrc为输入的骨骼化图像,8位灰度图像格式，元素中只有0与1,1代表有元素，0代表为空白
*/
void filterOver(cv::Mat thinSrc)
{
	assert(thinSrc.type() == CV_8UC1);
	int width = thinSrc.cols;
	int height = thinSrc.rows;
	for (int i = 0; i < height; ++i)
	{
		uchar* p = thinSrc.ptr<uchar>(i);
		for (int j = 0; j < width; ++j)
		{
			// 实现两个点之间至少隔一个像素
			//  p9 p2 p3
			//  p8 p1 p4
			//  p7 p6 p5
			uchar p1 = p[j];
			if (p1 != 1) continue;
			uchar p4 = (j == width - 1) ? 0 : *(p + j + 1);
			uchar p8 = (j == 0) ? 0 : *(p + j - 1);
			uchar p2 = (i == 0) ? 0 : *(p - thinSrc.step + j);
			uchar p3 = (i == 0 || j == width - 1) ? 0 : *(p - thinSrc.step + j + 1);
			uchar p9 = (i == 0 || j == 0) ? 0 : *(p - thinSrc.step + j - 1);
			uchar p6 = (i == height - 1) ? 0 : *(p + thinSrc.step + j);
			uchar p5 = (i == height - 1 || j == width - 1) ? 0 : *(p + thinSrc.step + j + 1);
			uchar p7 = (i == height - 1 || j == 0) ? 0 : *(p + thinSrc.step + j - 1);
			if (p2 + p3 + p8 + p9 >= 1)
			{
				p[j] = 0;
			}
		}
	}
}

/**
* @brief 从过滤后的骨骼化图像中寻找端点和交叉点
* @param thinSrc为输入的过滤后骨骼化图像,8位灰度图像格式，元素中只有0与1,1代表有元素，0代表为空白
* @param raudis卷积半径，以当前像素点位圆心，在圆范围内判断点是否为端点或交叉点
* @param thresholdMax交叉点阈值，大于这个值为交叉点
* @param thresholdMin端点阈值，小于这个值为端点
* @return 为对src细化后的输出图像,格式与src格式相同，元素中只有0与1,1代表有元素，0代表为空白
*
*/
std::vector<cv::Point2f> getPoints(const cv::Mat& thinSrc, unsigned int raudis, unsigned int thresholdMax, unsigned int thresholdMin)
{
	assert(thinSrc.type() == CV_8UC1);
	int width = thinSrc.cols;
	int height = thinSrc.rows;
	cv::Mat tmp;
	thinSrc.copyTo(tmp);
	std::vector<cv::Point2f> points;
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			if (*(tmp.data + tmp.step * i + j) == 0)
			{
				continue;
			}
			int count = 0;
			for (int k = i - raudis; k < i + raudis + 1; k++)
			{
				for (int l = j - raudis; l < j + raudis + 1; l++)
				{
					if (k < 0 || l < 0 || k>height - 1 || l>width - 1)
					{
						continue;
					}
					else if (*(tmp.data + tmp.step * k + l) == 1)
					{
						count++;
					}
				}
			}

			if (count > thresholdMax || count < thresholdMin)
			{
				Point2f point(j, i);
				points.push_back(point);
			}
		}
	}
	return points;
}

// 该算法可以再改进，eraser 向量的一个元素非常耗时间
std::vector<cv::Point2f> getKeyPoints(std::vector<cv::Point2f> InputKeyPoints, int radius)
{
	std::vector<cv::Point2f> OutputKeyPoints;

	Range rangeX{ 0, 0 };
	Range rangeY{ 0, 0 };
	int sizeKp = InputKeyPoints.size();
	for (int i = 0; i < sizeKp; i++)
	{
		if (i >= InputKeyPoints.size())
		{
			break;
		}
		// (X,Y) 为特征点坐标
		double X = InputKeyPoints[i].x;
		double Y = InputKeyPoints[i].y;

		// 更新范围
		rangeX.start = X - radius; rangeX.end = X + radius;
		rangeY.start = Y - radius; rangeY.end = Y + radius;
		Point2f sumOfPoint{ 0, 0 };
		int nPoints = 0;
		// 对在范围内的KeyPoints进行累加求均值
		for (int j = 0; j < InputKeyPoints.size(); j++)
		{
			double X1 = InputKeyPoints[j].x;
			double Y1 = InputKeyPoints[j].y;
			if (X1 >= rangeX.start && X1 <= rangeX.end
				&& Y1 >= rangeY.start && Y1 <= rangeY.end)
			{
				nPoints++;
				sumOfPoint.x += InputKeyPoints[j].x;
				sumOfPoint.y += InputKeyPoints[j].y;
				InputKeyPoints.erase(InputKeyPoints.begin() + j);
				//因为删除了一个元素，为防止少遍历一个元素，j--
				j -= 1;
			}
		}
		if (sizeKp != InputKeyPoints.size())
		{
			i -= 1;
		}
		if (nPoints >= 3)	//大于等于3个点时才认为是候选点
		{
			Point2f meanPoint((sumOfPoint.x / nPoints), (sumOfPoint.y / nPoints));
			OutputKeyPoints.push_back(meanPoint);
		}
	}
	return OutputKeyPoints;
}

/*
返回p1 p2 所表示线段的所有点集
算法步骤：

　　1.分别计算两点之间的横坐标和纵坐标差值：

   　　 deltaY = abs(y1 - y2);

	　　deltaX = abs(x1 - x2);

	2.设置循环变量范围，如果deltaY < deltaX,则自变量范围是[x1,x2](假设x1<x2)，反之自变量范围是[y1,y2];

	3. 当deltaY < deltaX时，选择A、B中任意一点做起始点，分别计算当横坐标自变量为i、纵坐标为j时，根据直线
	方程得到的截距Tmpb和b之间的差值，差值最小时对应的j即为纵坐标位置
*/
std::vector<Point> getLineAllPoint(Point2d p1, Point2d p2)
{
	double k = (p2.y - p1.y) / (p2.x - p1.x); // 斜率
	double b = (p1.y * p2.x - p1.x * p2.y) / (p2.x - p1.x); // 截距
	double deltaX = abs(p2.x - p1.x);
	double deltaY = abs(p2.y - p1.y);
	std::vector<Point> pointOnLine;
	if (deltaY < deltaX)
	{
		// 保证p1.x 小于 p2.x, 如果p1.x > p2.x ，交换数据
		if (p1.x >= p2.x)
		{
			double tempX = p1.x;
			double tempY = p1.y;
			p1.x = p2.x; p1.y = p2.y;
			p2.x = tempX; p2.y = tempY;
		}
		for (size_t i = p1.x; i < p2.x; i++)
		{
			static double y = p1.y;
			double yBegin = y - 2;	//[yBegin, yEnd] 为遍历范围
			double yEnd = y + 2;
			double Y = 0;
			double X = 0;
			double Min = 1000;

			for (size_t j = yBegin; j <= yEnd; j++)
			{
				double bTemp = j - k * i;
				double delta = abs(b - bTemp);
				if (delta < Min)
				{
					Min = delta;
					Y = j;
					X = i;
				}
			}
			if (X != 0 && Y != 0)
			{
				pointOnLine.push_back(Point2d{ X,Y });
				y = Y; //更新y, 下次判断（x1 +1, y)
			}
		}
	}
	else
	{
		if (p1.y >= p2.y)
		{
			double tempX = p1.x;
			double tempY = p1.y;
			p1.x = p2.x; p1.y = p2.y;
			p2.x = tempX; p2.y = tempY;
		}
		for (size_t j = p1.y; j < p2.y; j++)
		{
			static double x = p1.x;
			double xBegin = x - 2;	//[yBegin, yEnd] 为遍历范围
			double xEnd = x + 2;
			double Y = 0;
			double X = 0;
			double Min = 1000;

			for (size_t i = xBegin; i <= xEnd; i++)
			{
				double bTemp = j - k * i;
				double delta = abs(b - bTemp);
				if (delta < Min)
				{
					Min = delta;
					Y = j;
					X = i;
				}
			}
			if (X != 0 && Y != 0)
			{
				pointOnLine.push_back(Point2d{ X,Y });
				x = Y; //更新y, 下次判断（x1 +1, y)
			}
		}
	}
	return pointOnLine;
}

// 获取center点邻域内灰度累加和, 并选出累加和最大的那一个点
float sumGray(const Mat& src, Point2f& center, int radius)
{
	int side = 2 * radius + 1;
	// 选取邻域，以center 为中心，边长为2 * radius + 1的方形
	Mat roi_uchar{ src, Rect(center.x - radius, center.y - radius, side, side) };
	Mat roi;
	roi_uchar.convertTo(roi, CV_32F);
	float sumRoi = sum(roi)[0];
	return sumRoi;
}

// 获取灰度重心
void grayCenter(const Mat& src, Point2f& center, int radius)
{
	int side = 2 * radius + 1;
	// 选取邻域，以center 为中心，边长为2 * radius + 1的方形
	Mat roi_uchar{ src, Rect(center.x - radius, center.y - radius, side, side) };
	Mat roi;
	roi_uchar.convertTo(roi, CV_32F);
	float sumRoi = sum(roi)[0];
	//边长

	// xi yi组成的矩阵
	Mat xi = Mat::zeros(side, side, CV_32F);
	Mat yi = Mat::zeros(side, side, CV_32F);
	for (int i = 0; i < side; i++)
	{
		for (int j = 0; j < side; j++)
		{
			xi.at<float>(i, j) = center.x - radius + i;
			yi.at<float>(i, j) = center.y - radius + j;
		}
	}
	center.x = sum(xi.mul(roi))[0] / sumRoi;
	center.y = sum(yi.mul(roi))[0] / sumRoi;
}
//算法思想
//以center为中心，radius为半径的区域内每一个点->计算其邻域的灰度累加值
//选出累加值最大的点坐标，以该点为中心，再进行灰度中心计算
void preciseCorner(const Mat& src, Point2f& center, int radius)
{
	// side选取小一些
	int side = 3;

	float maxSumGray = 0;
	Point2f p;
	Point2f fixCenter{ center };
	//Mat gray = Mat::zeros(side, side, CV_32F);

	// 对center邻域一点（i, j), 以(i, j)为中心，计算radius邻域的灰度和
	for (int i = 0; i < side; i++)
	{
		for (int j = 0; j < side; j++)
		{
			float SUMGRAY; //以p点为中心的radius邻域灰度和
			p.x = fixCenter.x - 1 + float(i);
			p.y = fixCenter.y - 1 + float(j);
			SUMGRAY = sumGray(src, p, radius);
			//gray.at<float>(i, j) = SUMGRAY;
			if (SUMGRAY > maxSumGray)
			{
				maxSumGray = SUMGRAY;
				center.x = p.x;
				center.y = p.y;
			}
		}
	}

	grayCenter(src, center, radius);
}

float getK(Mat src, std::vector<Vec4i> linesP, int threshold, int mimLineLength, int maxLineGap)
{
	Mat cdstP; float meanK;
	std::vector<float> vectorK;
	HoughLinesP(src, linesP, 1, CV_PI / 180, threshold, mimLineLength, maxLineGap); // runs the actual detection
	cvtColor(src, cdstP, COLOR_GRAY2BGR);
	for (size_t i = 0; i < linesP.size(); i++)
	{
		Vec4i l = linesP[i];
		circle(cdstP, Point(l[0], l[1]), 5, Scalar(0, 255, 0), -1);
		circle(cdstP, Point(l[2], l[3]), 5, Scalar(0, 255, 0), -1);
		line(cdstP, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 1, LINE_AA);
		if ((l[2] - l[0]) != 0)
		{
			float k = (float(l[3]) - l[1]) / (l[2] - l[0]);
			vectorK.push_back(k);
		}
	}
	sort(vectorK.begin(), vectorK.end());
	for (size_t i = 0; i < vectorK.size(); i++)
	{
		cout << vectorK[i] << endl;
	}
	size_t end = vectorK.size();
	meanK = (vectorK[end - 5] + vectorK[end - 4] + vectorK[end - 3]) / 3;
	cout << "斜率均值为" << meanK << endl;
	imshow("搜寻直线", cdstP);
	return meanK;
}

// 最小二乘法求斜率
bool fitPoints(LineData& pts)
{
	int nPoints = pts.m_points.size();
	if (nPoints < 2) {
		// Fail: infinitely many lines passing through this single point
		return false;
	}
	double sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;
	for (int i = 0; i < nPoints; i++) {
		sumX += pts.m_points[i].x;
		sumY += pts.m_points[i].y;
		sumXY += double(pts.m_points[i].x) * pts.m_points[i].y;
		sumX2 += double(pts.m_points[i].x) * pts.m_points[i].x;
	}
	double xMean = sumX / nPoints;
	double yMean = sumY / nPoints;
	double denominator = sumX2 - sumX * xMean;
	// You can tune the eps (1e-7) below for your specific task
	if (std::fabs(denominator) < 1e-7) {
		// Fail: it seems a vertical line
		return false;
	}
	pts.m_k = (sumXY - sumX * yMean) / denominator;
	pts.m_b = yMean - pts.m_k * xMean;
	return true;
}

bool cmp(const Point& a, const Point& b)
{
	return a.x < b.x;
}

std::vector<LineData> getLineData(std::vector<Point2f>keyPoints, float k)
{
	std::vector<LineData> lines;
	const size_t ksize = keyPoints.size();
	std::vector<int> flag;
	for (size_t i = 0; i < ksize; i++)
	{
		flag.push_back(0);
	}
	int label = 1;
	// 对各点进行分类
	for (size_t i = 0; i < ksize; i++)
	{
		LineData l;// 表示某条光条
		// 先判断该点是否被分类 未分类时为0
		if (flag[i] == 0)
		{
			flag[i] = label; //标记该点
			l.m_points.push_back(keyPoints[i]);
			l.m_label = label;
			for (size_t j = 0; j < ksize; j++)
			{
				if (j != i && flag[j] == 0)
				{
					// k1 表示pt(i) pt(j) 两点组成的斜率
					//float k1 = 1.0 * (float(keyPoints[j].pt.y) - float(keyPoints[i].pt.y)) / (keyPoints[j].pt.x - keyPoints[i].pt.x);
					int delta = keyPoints[j].y - keyPoints[i].y;
					if (delta < k && delta > -k)
					{
						flag[j] = label;
						l.m_points.push_back(keyPoints[j]);
					}
				}
			}
			label++;
			fitPoints(l);
			lines.push_back(l);
		}
	}

	// 根据 b 值对各直线排序，对label进行更新， b 的值最小label为1，以此递增
	std::vector<float> vlabel;
	for (size_t i = 0; i < lines.size(); i++)
	{
		vlabel.push_back(lines[i].m_b);
	}
	sort(vlabel.begin(), vlabel.end());
	for (size_t i = 0; i < lines.size(); i++)
	{
		std::vector<float>::iterator iter;
		iter = std::find(vlabel.begin(), vlabel.end(), lines[i].m_b);
		int pos = iter - vlabel.begin();
		lines[i].m_label = pos;
		sort(lines[i].m_points.begin(), lines[i].m_points.end(), cmp);
	}
	// 根据关键点的横坐标进行排序

	return lines;
}