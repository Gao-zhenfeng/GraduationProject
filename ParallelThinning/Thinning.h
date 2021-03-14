#pragma once
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;
using std::endl;
using std::cout;

Mat thinImage(const cv::Mat& src, const int maxIterations = -1);

void filterOver(cv::Mat thinSrc);

std::vector<cv::KeyPoint> getPoints(const cv::Mat& thinSrc, unsigned int raudis = 4, unsigned int thresholdMax = 6, unsigned int thresholdMin = 4);
std::vector<cv::KeyPoint> getKeyPoints(std::vector<cv::KeyPoint> InputKeyPoints, int radius = 10);
