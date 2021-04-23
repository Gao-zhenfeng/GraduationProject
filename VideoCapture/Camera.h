#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "MvCameraControl.h"
#include <string>
using std::endl;
using std::cout;
using namespace cv;
class Mycamera
{
public:
	Mycamera();
	~Mycamera();
	//声明相关变量及函数等
	static int EnumDevices(MV_CC_DEVICE_INFO_LIST* pstDevList);

	// ch:连接相机
	int connectCamera(int id = 0);

	//设置是否为触发模式
	//int setTriggerMode(unsigned int TriggerModeNum);

	//开启相机采集
	int startCamera();

	//停止采集
	int stopCamera();

	//发送软触发
	int softTrigger();

	//读取buffer
	int ReadBuffer(Mat& image);

	//设置心跳时间
	int setHeartBeatTime(unsigned int time);

	//设置曝光时间
	int setExposureTime(float ExposureTimeNum);

	// print the discovered devices information to user
	bool PrintDeviceInfo(MV_CC_DEVICE_INFO* pstMVDevInfo);

	// 转换图像格式
	int RGB2BGR(unsigned char* pRgbData, unsigned int nWidth, unsigned int nHeight);

	// convert data stream in Mat format
	bool Convert2Mat(IN MV_FRAME_OUT_INFO_EX* pstImageInfo, IN unsigned char* pData, OUT Mat& srcImage);

private:
	void* m_hDevHandle;

public:
};
