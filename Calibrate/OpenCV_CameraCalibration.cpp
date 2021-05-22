#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <iostream>

using namespace std;
using namespace cv;

// Defining the dimensions of checkerboard
// 定义棋盘格的尺寸
int board_w = 9;
int board_h = 13;
double checkBoardRadius = 12; //棋盘格每个格子的边长 mm
Size board_sz = Size(board_w, board_h);//corners per row, corner per column
//Size patternSize = Size(board_h - 1, board_w - 1);
int main()
{
	// Creating vector to store vectors of 3D points for each checkerboard image
	// 创建矢量以存储每个棋盘图像的三维点矢量
	std::vector<std::vector<cv::Point3f> > objpoints;

	// Creating vector to store vectors of 2D points for each checkerboard image
	// 创建矢量以存储每个棋盘图像的二维点矢量
	std::vector<std::vector<cv::Point2f> > imgpoints;

	// Defining the world coordinates for 3D points
	// 为三维点定义世界坐标系
	// objp各元素都是整数，是否应该加一个系数，如棋盘格单元的半径为25mm
	//后面将会以 mm来定义距离
	std::vector<cv::Point3f> objp;
	for (size_t i = 0; i < board_sz.height; i++) //9列， width is 9
	{
		for (size_t j = 0; j < board_sz.width; j++) // 6行，height is 6
		{
			objp.push_back(cv::Point3f(float(j * checkBoardRadius), float(i * checkBoardRadius), 0));
		}
	}

	// Extracting path of individual image stored in a given directory
	// 提取存储在给定目录中的单个图像的路径
	std::vector<cv::String> images;

	// Path of the folder containing checkerboard images
	// 包含棋盘图像的文件夹的路径
	std::string path = "./images/b/*.bmp";

	//std::string path = "./calib_left/*.bmp";

	// 使用glob函数读取所有图像的路径
	cv::glob(path, images);

	cv::Mat frame, gray;

	// vector to store the pixel coordinates of detected checker board corners
	// 存储检测到的棋盘转角像素坐标的矢量
	std::vector<cv::Point2f> corner_pts;
	bool success;

	// Looping over all the images in the directory
	// 循环读取图像
	Size image_size;
	for (int i = 0; i < images.size(); i++)
	{
		frame = cv::imread(images[i]);
		image_size = frame.size();
		if (frame.empty())
		{
			continue;
		}
		cout << "the current image is " << i << "th" << images[i] << endl;
		cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

		// Finding checker board corners
		// 寻找角点
		// If desired number of corners are found in the image then success = true
		// 如果在图像中找到所需数量的角，则success = true
		// opencv4以下版本，flag参数为CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE
		success = cv::findChessboardCorners(gray, Size(board_w, board_h), corner_pts, CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);

		/*
		 * If desired number of corner are detected,
		 * we refine the pixel coordinates and display
		 * them on the images of checker board
		*/
		// 如果检测到所需数量的角点，我们将细化像素坐标并将其显示在棋盘图像上
		if (success)
		{
			// 如果是OpenCV4以下版本，第一个参数为CV_TERMCRIT_EPS | CV_TERMCRIT_ITER
			cv::TermCriteria criteria(TermCriteria::EPS | TermCriteria::Type::MAX_ITER, 30, 0.001);

			// refining pixel coordinates for given 2d points.
			// 为给定的二维点细化像素坐标
			cv::cornerSubPix(gray, corner_pts, cv::Size(11, 11), cv::Size(-1, -1), criteria);

			// Displaying the detected corner points on the checker board
			// 在棋盘上显示检测到的角点
			cv::drawChessboardCorners(frame, board_sz, corner_pts, success);

			objpoints.push_back(objp);
			imgpoints.push_back(corner_pts);
		}

		//cv::imshow("chessboard detection", frame);
		//cv::waitKey(0);
	}

	cv::destroyAllWindows();

	cv::Mat cameraMatrix, distCoeffs, R, T;

	/*
	 * Performing camera calibration by
	 * passing the value of known 3D points (objpoints)
	 * and corresponding pixel coordinates of the
	 * detected corners (imgpoints)
	*/
	// 通过传递已知3D点（objpoints）的值和检测到的角点（imgpoints）的相应像素坐标来执行相机校准
	cv::calibrateCamera(
		objpoints,
		imgpoints,
		image_size,
		cameraMatrix,
		distCoeffs,
		R,
		T,
		cv::CALIB_ZERO_TANGENT_DIST | cv::CALIB_FIX_PRINCIPAL_POINT
	);

	// 内参矩阵
	std::cout << "cameraMatrix :\n" << cameraMatrix << std::endl;
	// 透镜畸变系数
	std::cout << "distCoeffs :\n" << distCoeffs << std::endl;
	// rvecs
	std::cout << "Rotation vector :\n" << R << std::endl;
	// tvecs
	std::cout << "Translation vector :\n" << T << std::endl;

	cv::Mat map1, map2;
	cv::initUndistortRectifyMap(
		cameraMatrix,
		distCoeffs,
		cv::Matx33f::eye(),
		cameraMatrix,
		cv::Size(gray.rows, gray.cols),
		CV_32FC1,
		map1,
		map2
	);
	for (int i = 0; i < images.size(); i++)
	{
		Mat srcimage = cv::imread(images[i]);
		Mat image0;
		if (frame.empty())
		{
			continue;
		}
		cv::remap(
			srcimage,
			image0,
			map1,
			map2,
			cv::INTER_LINEAR,
			cv::BORDER_CONSTANT,
			cv::Scalar()
		);
		//cv::imshow("Undistorted", image0);
		//waitKey(0);
	}

	FileStorage fs{ "calibdata.yml", FileStorage::WRITE };

	fs << "cameraMatrix" << cameraMatrix << "distCoeffs" << distCoeffs;
	fs << "rotationMatrix" << R << "translationMatrix" << T;
	fs.release();
	return 0;
}