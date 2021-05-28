#include "GetCorner.h"

Corner::Corner()
{
}

Corner::Corner(std::string filename)
{
	this->m_src = imread(filename, CV_8UC1);
	this->m_cameramatrix = Matx33d::zeros();
	this->m_distCoeffs = Matx41d::zeros();;
	getROI(m_src, m_img_binary);
}

Corner::Corner(std::string filename, Matx33d M, Matx41d distCoeffs)
{
	this->m_cameramatrix = M;
	this->m_distCoeffs = distCoeffs;
	this->m_src = imread(filename, CV_8UC1);
	getROI(m_src, m_img_binary);
}

Corner::Corner(Mat& src)
{
	m_src = src.clone();
	getROI(m_src, m_img_binary);
}

void Corner::getROI(const Mat& src, Mat& img_binary)
{
	Mat topHatImage;
	Mat element = getStructuringElement(MORPH_ELLIPSE, Size(10, 10));
	morphologyEx(src, topHatImage, MORPH_TOPHAT, element);

	//自适应二值化
	Mat dst;
	adaptiveThreshold(topHatImage, dst, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 15, 0);

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
	img_binary = Mat::zeros(dst.size(), CV_8UC1);
	for (int y = 0; y < img_binary.rows; y++)
	{
		for (int x = 0; x < img_binary.cols; x++)
		{
			int label = labels.at<int>(y, x);
			CV_Assert(0 <= label && label <= nccomps);
			img_binary.at<uchar>(y, x) = colors[label];
		}
	}
}

void Corner::cvThin(cv::Mat& src, cv::Mat& dst, int intera)
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
void Corner::filterOver(cv::Mat thinSrc)
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
std::vector<cv::Point2d> Corner::getPoints(const cv::Mat& thinSrc, unsigned int raudis, unsigned int thresholdMax, unsigned int thresholdMin)
{
	assert(thinSrc.type() == CV_8UC1);
	int width = thinSrc.cols;
	int height = thinSrc.rows;
	cv::Mat tmp;
	thinSrc.copyTo(tmp);
	std::vector<cv::Point2d> points;
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
				Point2d point(j, i);
				points.push_back(point);
			}
		}
	}
	if (m_cameramatrix(0, 0) != 0)
	{
		undistortPoints(points, points, m_cameramatrix, m_distCoeffs, cv::noArray(), m_cameramatrix);
	}

	return points;
}

// 该算法可以再改进，eraser 向量的一个元素非常耗时间
std::vector<cv::Point2d> Corner::getKeyPoints(std::vector<cv::Point2d> InputKeyPoints, int radius)
{
	std::vector<cv::Point2d> OutputKeyPoints;

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
		Point2d sumOfPoint{ 0, 0 };
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
			Point2d meanPoint((sumOfPoint.x / nPoints), (sumOfPoint.y / nPoints));
			OutputKeyPoints.push_back(meanPoint);
		}
	}
	return OutputKeyPoints;
}

// 获取center点邻域内灰度累加和, 并选出累加和最大的那一个点
float Corner::sumGray(const Mat& src, Point2d& center, int radius)
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
void Corner::grayCenter(const Mat& src, Point2d& center, int radius)
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
void Corner::preciseCorner(const Mat& src, Point2d& center, int radius)
{
	// side选取小一些
	int side = 3;

	float maxSumGray = 0;
	Point2d p;
	Point2d fixCenter{ center };
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

std::vector<LineData> Corner::getLineData(std::vector<Point2d>keyPoints, float k)
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
			double k1 = -0.1310;
			double b1 = keyPoints[i].y - k1 * keyPoints[i].x;

			for (size_t j = 0; j < ksize; j++)
			{
				if (j != i && flag[j] == 0)
				{
					// k1 表示pt(i) pt(j) 两点组成的斜率
					//float k1 = 1.0 * (float(keyPoints[j].pt.y) - float(keyPoints[i].pt.y)) / (keyPoints[j].pt.x - keyPoints[i].pt.x);
					double u = keyPoints[j].x;
					double v = keyPoints[j].y;
					double vpredict = k1 * u + b1;
					double delta = v - vpredict;
					if (abs(delta) < k)
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
	for (int i = 0; i < lines.size(); i++)
	{
		for (int j = 0; j < lines[i].m_points.size(); j++)
		{
			String s = "(" + std::to_string(lines[i].m_label) + "," + std::to_string(j) + ")";
			//cout << lines[i].m_points[j].x << "  " << lines[i].m_points[j].y << "  "
				//<< lines[i].m_k << "  " << lines[i].m_b << "  " << lines[i].m_label << endl;
			circle(m_keyPointsImage, lines[i].m_points[j], 1, Scalar(255, 0, 0), -1);
			putText(m_keyPointsImage, s, lines[i].m_points[j], FONT_HERSHEY_SIMPLEX, 0.25, Scalar(0, 0, 255), 1, LINE_AA);//在图片上写文字
		}
	}
	return lines;
}

void Corner::getCorner()
{
	medianBlur(m_img_binary, m_img_binary, 3);
	Mat dst = m_img_binary / 255;

	Mat thinsrc;
	cvThin(dst, thinsrc, 5);
	filterOver(thinsrc);
	Mat thinImage = thinsrc * 255;
	//获取扎堆角点
	std::vector<Point2d> roughKeyPoints = getPoints(thinsrc, 5, 6, 0);

	//对扎堆角点做均值
	m_keyPoints = getKeyPoints(roughKeyPoints, 10);
	//绘制keyPoints
	cvtColor(m_src, m_keyPointsImage, COLOR_GRAY2RGB);
	for (size_t i = 0; i < m_keyPoints.size(); i++)
	{
		grayCenter(m_src, m_keyPoints[i], 2);
		//preciseCorner(graySrc, keyPoints[i].pt, 2);
		//circle(m_keyPointsImage, m_keyPoints[i], 1, Scalar(0, 0, 255), FILLED);
	}

	m_lines = getLineData(m_keyPoints, 9);
}

void Corner::getAllLine()
{
	Mat dst = m_img_binary / 255;
	Mat thinsrc;
	cvThin(dst, thinsrc, 5);
	filterOver(thinsrc);
	Mat thinImage = thinsrc * 255;
	this->m_lines = classifyHorizonLines(thinImage);
	//验证画到m_keyPointsImage上
	cvtColor(m_src, m_keyPointsImage, COLOR_GRAY2RGB);
	int numLINE = m_lines.size();
	for (size_t i = 0; i < numLINE; i++)
	{
		for (size_t j = 0; j < m_lines[i].m_points.size(); j++)
		{
			int u0 = m_lines[i].m_points[j].x;
			int v0 = m_lines[i].m_points[j].y;

			m_keyPointsImage.at<Vec3b>(v0, u0) = Vec3b{ 47, 0, uchar(i * 10) };
		}
	}
}

bool cmpLineData(const LineData& a, const LineData& b)
{
	return a.m_label < b.m_label;
}
std::vector<LineData> Corner::classifyHorizonLines(Mat& src)
{
	std::vector<LineData> lines;//返回值
	int rows = src.rows;//图像行数 1024
	int cols = src.cols;//图像列数 1280
	Mat maskImage = Mat::zeros(src.size(), CV_8UC1);
	int label = 0;

	for (int j = 0; j < cols; j++)
	{
		//int j = 400;
		for (int i = 0; i < rows; i++)
		{
			if (src.at<uchar>(i, j) != 0 && maskImage.at<uchar>(i, j) == 0)
			{
				maskImage.at<uchar>(i, j) == uchar(150);
				LineData horizonLine;
				int flag;
				Point2d p = Point2d{ double(j), double(i) };
				horizonLine.m_points.push_back(p);
				Point2d nextPoint;
				while (updatePoint(src, maskImage, p, nextPoint))
				{
					horizonLine.m_points.push_back(nextPoint);
					//更新p点坐标
					p.x = nextPoint.x;
					p.y = nextPoint.y;
				}

				// 对LineData计算参数，同时根据LineData 的标签标记maskImage
				if (horizonLine.m_points.size() > 150)
				{
					horizonLine.m_label = label;
					//标记maskImage
					for (int k = 0; k < horizonLine.m_points.size(); k++)
					{
						int u = horizonLine.m_points[k].x;
						int v = horizonLine.m_points[k].y;
					}
					fitPoints(horizonLine);//计算直线的截距与斜率
					lines.push_back(horizonLine);
					if (lines.size() == 20)
					{
						break;
					}
					label++;
				}
			}
		}
		if (lines.size() == 20)
		{
			break;
		}
	}

	// 根据 b 值对各直线排序，对label进行更新， b 的值最小label为1，以此递增
	std::vector<float> vlabel;
	for (size_t i = 0; i < lines.size(); i++)
	{
		vlabel.push_back(lines[i].m_b);
	}
	sort(vlabel.begin(), vlabel.end());

	//根据横坐标进行排序
	for (size_t i = 0; i < lines.size(); i++)
	{
		std::vector<float>::iterator iter;
		iter = std::find(vlabel.begin(), vlabel.end(), lines[i].m_b);
		int pos = iter - vlabel.begin();
		lines[i].m_label = pos;
		sort(lines[i].m_points.begin(), lines[i].m_points.end(), cmp);
	}
	sort(lines.begin(), lines.end(), cmpLineData);
	return lines;
}

int Corner::updatePoint(const Mat& src, Mat& maskImage, Point2d& p, Point2d& nextPoint)
{
	double u0 = p.x;
	double v0 = p.y;
	maskImage.at<uchar>(v0, u0) = (uchar)150;
	std::vector<Point2d> points;
	Mat roi_uchar{ src, Rect(u0 + 1, v0 - 1, 3, 3) };
	Mat roi;
	roi_uchar.convertTo(roi, CV_64F);
	int n_points = 0;
	//先检测3邻域内的点
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			maskImage.at<uchar>(v0 - 1 + j, u0 + 1 + i) = (uchar)150;
			if (roi.at<double>(j, i) != 0)
			{
				n_points++;
				points.push_back(Point2d{ u0 + 1 + i , v0 - 1 + j });
			}
		}
	}

	if (n_points == 0)//两种情况：（1）整条直线检测结束 （2）交点初漏检测
	{
		std::vector<Point2d> points5;
		//扩大检测区域
		int radius = 7;
		int heng = 4;
		Mat roi_uchar5{ src, Rect(u0 + 1, v0 - heng, radius ,  2 * heng + 1) };
		Mat roi5;
		roi_uchar5.convertTo(roi5, CV_64F);
		int n_5 = 0;//五邻域内点
		for (int i = 0; i < radius; i++)
		{
			for (int j = 0; j < 2 * heng + 1; j++)
			{
				maskImage.at<uchar>(v0 - heng + j, u0 + 1 + i) = (uchar)150;
				maskImage.at<uchar>(v0, u0) = (uchar)255;

				if (roi5.at<double>(j, i) != 0)
				{
					n_5++;
					points5.push_back(Point2d{ u0 + 1 + i , v0 - heng + j });
				}
			}
		}
		if (n_5 != 0)
		{
			std::vector<int> v;
			int maxY = 0;
			//寻找邻域内具有最大横坐标的那个点，并将该点作为nextPoint
			for (int i = 0; i < n_5; i++)
			{
				v.push_back(points5[i].x);
				if (points5[i].x > maxY) maxY = points5[i].x;
			}
			for (int i = 0; i < n_5; i++)
			{
				if (points5[i].x == maxY)
				{
					nextPoint.x = points5[i].x;
					nextPoint.y = points5[i].y;
					maskImage.at<uchar>(nextPoint.y, nextPoint.x) = (uchar)255;
					maskImage.at<uchar>(v0, u0) = (uchar)255;

					return 1;
				}
			}
		}
		else
		{
			return 0;
		}
	}
	else if (n_points == 1)//最常见情况,直接将非零点作为nextPoint
	{
		if (maskImage.at<uchar>(points[0].y, points[0].x) == 255)
		{
			return 0;
		}
		nextPoint.x = points[0].x;
		nextPoint.y = points[0].y;
		maskImage.at<uchar>(nextPoint.y, nextPoint.x) = (uchar)255;
		maskImage.at<uchar>(v0, u0) = (uchar)255;

		return 1;
	}
	else //邻域有多个点，说明此时在交点区域,扩大检测范围
	{
		std::vector<Point2d> points5;
		int radius = 7;
		int heng = 4;
		Mat roi_uchar5{ src, Rect(u0 + 1, v0 - heng, radius, heng * 2 + 1) };
		Mat roi5;
		roi_uchar5.convertTo(roi5, CV_64F);
		int n_5 = 0;//五邻域内点
		for (int i = 0; i < radius; i++)
		{
			for (int j = 0; j < heng * 2 + 1; j++)
			{
				if (maskImage.at<uchar>(v0 - heng + j, u0 + 1 + i) == 255)
				{
					return 0;
				}
				maskImage.at<uchar>(v0 - heng + j, u0 + 1 + i) = (uchar)150;
				if (roi5.at<double>(j, i) != 0)
				{
					n_5++;
					points5.push_back(Point2d{ u0 + 1 + i , v0 - heng + j });
				}
			}
		}
		std::vector<int> v;
		int maxY = 0;
		//找到横坐标最大的那个点，作为nextPoint
		for (int i = 0; i < n_5; i++)
		{
			v.push_back(points5[i].x);
			if (points5[i].x > maxY) maxY = points5[i].x;
		}

		for (int i = 0; i < n_5; i++)
		{
			if (points5[i].x == maxY)
			{
				nextPoint.x = points5[i].x;
				nextPoint.y = points5[i].y;
				maskImage.at<uchar>(nextPoint.y, nextPoint.x) = (uchar)255;
				maskImage.at<uchar>(v0, u0) = (uchar)255;
			}
		}
		return 1;
	}
}