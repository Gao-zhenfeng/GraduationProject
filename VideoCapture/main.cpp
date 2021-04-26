#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "MvCameraControl.h"
#include "Camera.h"

using namespace cv;
int main()
{
	Mycamera Hikvision;
	Hikvision.connectCamera();
	Hikvision.startCamera();
	while (1)
	{
		Mat frame;
		if (Hikvision.ReadBuffer(frame) == 0)
			imshow("HikVision", frame);
		char c = waitKey(10);
		if (c == 27)  Hikvision.stopCamera();
		frame.release();
	}

	return 0;
}