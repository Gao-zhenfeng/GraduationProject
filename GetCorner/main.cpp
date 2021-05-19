#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include<opencv2/xfeatures2d.hpp>
using namespace cv;
using std::endl;
using std::cout;

int main(int argc, char** argv)
{
	Mat graySrc = imread("../Picture/I9.bmp", CV_8UC1);
	Mat src = imread("../Picture/I10.bmp", CV_8UC1);

	return 0;
}