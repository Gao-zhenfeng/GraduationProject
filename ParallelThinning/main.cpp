#include <opencv2/opencv.hpp>
#include <iostream>

int main(int argc, char** argv)
{
	cv::Mat img = cv::imread("E:\\课程资料\\毕设\\Program\\Graduation_Project\\ParallelThinning\\Picture\\JK.jpg");
	if (img.empty())
	{
		std::cout << "can't read image" << std::endl;
		return -1;
	}
	cv::namedWindow("JK", cv::WINDOW_AUTOSIZE);
	cv::imshow("JK", img);
	cv::waitKey(0);
	cv::destroyWindow("JK");
	return 0;
}