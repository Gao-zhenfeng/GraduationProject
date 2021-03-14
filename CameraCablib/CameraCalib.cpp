// CameraCablib.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <opencv2/opencv.hpp>

using std::cout;
using std::endl;
using std::vector;
using namespace cv;

int main(int argc, char* argv[])
{
	int n_boards = 0; // will be set by input list
	float image_sf = 0.5f;//image scale factor, for shrinking an image
	float delay = 1.f;
	int board_w = 0;// corners per row 棋盘格横向角点个数
	int board_h = 0;// corner per col

	if (argc < 4 || argc > 6)
	{
		cout << "\nERROR: Wrong number of input paramerers";
		return -1;
	}

	board_w = atoi(argv[1]);//the width of chessboard
	board_h = atoi(argv[2]);//the height of chessboard
	n_boards = atoi(argv[3]);//number of chessboards
	if (argc > 4) delay = atof(argv[4]);
	if (argc > 5) image_sf = atof(argv[5]);

	int board_n = board_w * board_h;
	Size board_sz = Size(board_w, board_h);//corners per row, corner per column

	VideoCapture capture(0);
	if (!capture.isOpened())
	{
		cout << "\nCould not open the camera\n";
		return -1;
	}

	//ALLOCATE STORAGE
	//
	vector< vector<Point2f> > image_points;
	vector< vector<Point3f> > object_points;

	//Capture corner views: loop until we've got n_boards successful
	//captures (all corners on the boards are found)
	//
	double last_captured_timestamp = 0;
	Size image_size;

	while (image_points.size() < (size_t)n_boards)
	{
		Mat image0, image;
		capture >> image0;
		image_size = image0.size();
		resize(image0, image, Size(), image_sf, image_sf, INTER_LINEAR);

		//Find the board
		//
		vector<Point2f> corners;
		bool found = findChessboardCorners(image, board_sz, corners);

		//Draw it
		//
		drawChessboardCorners(image, board_sz, corners, found);

		//If we got a good board, add it to out data
		//
		double timestamp = (double)clock() / CLOCKS_PER_SEC;

		if (found && timestamp - last_captured_timestamp > 1)
		{
			last_captured_timestamp = timestamp;
			image ^= cv::Scalar::all(255);
			cv::Mat mcorners(corners); // do not copy the data
			mcorners *= (1. / image_sf); // scale the corner coordinates
			image_points.push_back(corners);
			object_points.push_back(vector<Point3f>());//往里面push的啥？？？
			vector<cv::Point3f>& opts = object_points.back();//利用opts，往里面赋值

			opts.resize(board_n);
			for (int j = 0; j < board_n; j++)
			{
				opts[j] = cv::Point3f((float)(j / board_w), (float)(j % board_w), 0.f);//何必这么麻烦
			}
			cout << "Collected our " << (int)image_points.size() <<
				" of " << n_boards << " needed chessboard images\n" << endl;
		}

		cv::imshow("Calibration", image); //show in color if we did collect the image
		if ((cv::waitKey(30) & 255) == 27)
			return -1;
	}
	// END COLLECTION WHILE LOOP.
	cv::destroyWindow("Calibration");
	cout << "\n\n*** CALIBRATING THE CAMERA...\n" << endl;
	// CALIBRATE THE CAMERA!
	//
	cv::Mat intrinsic_matrix, distortion_coeffs;
	double err = cv::calibrateCamera(
		object_points,
		image_points,
		image_size,
		intrinsic_matrix,
		distortion_coeffs,
		cv::noArray(),
		cv::noArray(),
		cv::CALIB_ZERO_TANGENT_DIST | cv::CALIB_FIX_PRINCIPAL_POINT
	);
	// SAVE THE INTRINSICS AND DISTORTIONS
	cout << " *** DONE!\n\nReprojection error is " << err <<
		"\nStoring Intrinsics.xml and Distortions.xml files\n\n";
	cv::FileStorage fs("intrinsics.xml", FileStorage::WRITE);
	fs << "image_width" << image_size.width << "image_height" << image_size.height
		<< "camera_matrix" << intrinsic_matrix << "distortion_coefficients"
		<< distortion_coeffs;
	fs.release();
	// EXAMPLE OF LOADING THESE MATRICES BACK IN:
	fs.open("intrinsics.xml", cv::FileStorage::READ);
	cout << "\nimage width: " << (int)fs["image_width"];
	cout << "\nimage height: " << (int)fs["image_height"];
	cv::Mat intrinsic_matrix_loaded, distortion_coeffs_loaded;
	fs["camera_matrix"] >> intrinsic_matrix_loaded;
	fs["distortion_coefficients"] >> distortion_coeffs_loaded;
	cout << "\nintrinsic matrix:" << intrinsic_matrix_loaded;
	cout << "\ndistortion coefficients: " << distortion_coeffs_loaded << endl;

	// Build the undistort map which we will use for all
	// subsequent frames.
	//
	cv::Mat map1, map2;
	cv::initUndistortRectifyMap(
		intrinsic_matrix_loaded,
		distortion_coeffs_loaded,
		cv::Mat(),
		intrinsic_matrix_loaded,
		image_size,
		CV_16SC2,
		map1,
		map2
	);
	// Just run the camera to the screen, now showing the raw and
	// the undistorted image.
	//
	for (;;) {
		cv::Mat image, image0;
		capture >> image0;
		if (image0.empty()) break;
		cv::remap(
			image0,
			image,
			map1,
			map2,
			cv::INTER_LINEAR,
			cv::BORDER_CONSTANT,
			cv::Scalar()
		);
		cv::imshow("Undistorted", image);
		if ((cv::waitKey(30) & 255) == 27) break;
	}

	return 0;
}