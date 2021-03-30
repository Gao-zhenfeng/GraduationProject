#include "dbscan.h"

classifiedPoint::classifiedPoint(cv::Point2f p)
{
	this->clusterID = UNCLASSIFIED;
	this->p = p;
}

DBSCAN::DBSCAN(unsigned int minPts, float eps, std::vector<classifiedPoint> points) {
	m_minPoints = minPts;
	m_epsilon = eps;
	m_points = points;
	m_pointSize = points.size();
	m_numOfCluster = 0;
}

int DBSCAN::run()
{
	int clusterID = 1;
	std::vector<classifiedPoint>::iterator iter;
	for (iter = m_points.begin(); iter != m_points.end(); ++iter)
	{
		if (iter->clusterID == UNCLASSIFIED)
		{
			if (expandCluster(*iter, clusterID) != FAILURE)
			{
				clusterID += 1;
			}
		}
	}
	m_numOfCluster = clusterID;
	return 0;
}

int DBSCAN::expandCluster(classifiedPoint point, int clusterID)
{
	std::vector<int> clusterSeeds = calculateCluster(point);

	if (clusterSeeds.size() < m_minPoints)
	{
		// 该点邻域内点数 < m_minPoints
		point.clusterID = NOISE;
		return FAILURE;
	}
	else
	{
		int index = 0, indexCorePoint = 0;
		std::vector<int>::iterator iterSeeds;
		for (iterSeeds = clusterSeeds.begin(); iterSeeds != clusterSeeds.end(); ++iterSeeds)
		{
			m_points.at(*iterSeeds).clusterID = clusterID; //给各点标号,表明该点在哪个群
			if (m_points.at(*iterSeeds).p.x == point.p.x && m_points.at(*iterSeeds).p.y == point.p.y)
			{
				indexCorePoint = index;
			}
			++index;
		}
		// 删除中心点
		clusterSeeds.erase(clusterSeeds.begin() + indexCorePoint);

		for (std::vector<int>::size_type i = 0, n = clusterSeeds.size(); i < n; ++i)
		{
			std::vector<int> clusterNeighors = calculateCluster(m_points.at(clusterSeeds[i]));

			if (clusterNeighors.size() >= m_minPoints)
			{
				std::vector<int>::iterator iterNeighors;
				for (iterNeighors = clusterNeighors.begin(); iterNeighors != clusterNeighors.end(); ++iterNeighors)
				{
					if (m_points.at(*iterNeighors).clusterID == UNCLASSIFIED || m_points.at(*iterNeighors).clusterID == NOISE)
					{
						if (m_points.at(*iterNeighors).clusterID == UNCLASSIFIED)
						{
							clusterSeeds.push_back(*iterNeighors);
							n = clusterSeeds.size();
						}
						m_points.at(*iterNeighors).clusterID = clusterID;
					}
				}
			}
		}

		return SUCCESS;
	}
}

// 遍历各个点，返回邻域内点的索引向量
std::vector<int> DBSCAN::calculateCluster(classifiedPoint point)
{
	int index = 0;
	std::vector<classifiedPoint>::iterator iter;
	std::vector<int> clusterIndex;
	for (iter = m_points.begin(); iter != m_points.end(); ++iter)
	{
		if (calculateDistance(point, *iter) <= m_epsilon)
		{
			clusterIndex.push_back(index);
		}
		index++;
	}
	return clusterIndex;
}

// 计算两点间的距离
inline double DBSCAN::calculateDistance(classifiedPoint pointCore, classifiedPoint pointTarget)
{
	return pow(pointCore.p.x - pointTarget.p.x, 2) + pow(pointCore.p.y - pointTarget.p.y, 2);
}

int DBSCAN::getNumOfCluster() { return m_numOfCluster; }
int DBSCAN::getTotalPointSize() { return m_pointSize; }
int DBSCAN::getMinimumClusterSize() { return m_minPoints; }
int DBSCAN::getEpsilonSize() { return m_epsilon; }
std::vector<classifiedPoint> DBSCAN::getPoints() { return m_points; }