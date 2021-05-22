#include "LineData.h"

LineData::LineData()
{
	this->m_label = 0;
	this->m_k = 0.0;
	this->m_b = 0.0;
}

LineData::LineData(std::vector<Point2d> points, int label, float k, float b) : m_points{ points }
{
	this->m_label = label;
	this->m_k = k;
	this->m_b = b;
}

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