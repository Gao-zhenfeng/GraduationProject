#pragma once

#include <vector>
#include <cmath>
#include <opencv2/opencv.hpp>

#define UNCLASSIFIED -1
#define CORE_POINT 1
#define BORDER_POINT 2
#define NOISE -2
#define SUCCESS 0
#define FAILURE -3

struct classifiedPoint
{
	cv::Point2f p;
	int clusterID;  // clustered ID
	classifiedPoint(cv::Point2f p);
};

class DBSCAN {
public:
	DBSCAN(unsigned int minPts, float eps, std::vector<classifiedPoint> points);

	int run();
	std::vector<int> calculateCluster(classifiedPoint point);
	int expandCluster(classifiedPoint point, int clusterID);
	inline double calculateDistance(classifiedPoint pointCore, classifiedPoint pointTarget);

	int getNumOfCluster();
	int getTotalPointSize();
	int getMinimumClusterSize();
	int getEpsilonSize();
	std::vector<classifiedPoint> getPoints();

private:
	std::vector<classifiedPoint> m_points{};
	unsigned int m_pointSize{ 0 };
	unsigned int m_minPoints{ 0 };
	float m_epsilon{ 0 };
	int m_numOfCluster{ 0 };
};
