#include "Camera.h"

MV_CC_DEVICE_INFO_LIST m_stDevList;       // ch:设备信息列表结构体变量，用来存储设备列表
MV_CC_DEVICE_INFO* m_Device = NULL;         //设备对象

Mycamera::Mycamera()
{
	m_hDevHandle = NULL;
}

Mycamera::~Mycamera()
{
	if (m_hDevHandle)
	{
		MV_CC_DestroyHandle(m_hDevHandle);
		m_hDevHandle = NULL;
	}
}

//查询设备列表
int Mycamera::EnumDevices(MV_CC_DEVICE_INFO_LIST* pstDevList)
{
	int temp = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, pstDevList);
	if (MV_OK != temp)
	{
		return -1;
	}
	return 0;
}

//连接相机
int  Mycamera::connectCamera(int id)
{
	int temp = EnumDevices(&m_stDevList);
	if (temp != 0)
		//设备更新成功接收命令的返回值为0，返回值不为0则为异常
		return -1;
	if (m_stDevList.nDeviceNum == 0)
		//未找到任何相机
		return 2;
	for (unsigned int i = 0; i < m_stDevList.nDeviceNum; i++)
	{
		MV_CC_DEVICE_INFO* pDeviceInfo = m_stDevList.pDeviceInfo[i];
		if (NULL == pDeviceInfo)
		{
			continue;
		}
		if (id == i)
		{
			m_Device = m_stDevList.pDeviceInfo[i];
			std::cout << "连接成功" << endl;
			break;
		}
		else
		{
			continue;
		}
	}
	if (m_Device == NULL)
	{
		//未找到指定名称的相机
		return 3;
	}
	temp = MV_CC_CreateHandle(&m_hDevHandle, m_Device);
	if (temp != 0)
		return -1;

	temp = MV_CC_OpenDevice(m_hDevHandle);
	if (temp != 0)
	{
		MV_CC_DestroyHandle(m_hDevHandle);
		m_hDevHandle = NULL;
		return -1;
	}
	else
	{
		//setTriggerMode(1);
		return 0;
	}
}
//启动相机采集
int Mycamera::startCamera()
{
	int temp = MV_CC_StartGrabbing(m_hDevHandle);
	if (temp != 0)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}
//发送软触发
int Mycamera::softTrigger()
{
	int tempValue = MV_CC_SetCommandValue(m_hDevHandle, "TriggerSoftware");
	if (tempValue != 0)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

//读取相机中的图像
int Mycamera::ReadBuffer(Mat& image)
{
	unsigned int nRecvBufSize = 0;
	MVCC_INTVALUE stParam;
	memset(&stParam, 0, sizeof(MVCC_INTVALUE));
	int temp = MV_CC_GetIntValue(m_hDevHandle, "PayloadSize", &stParam);
	if (temp != 0)
	{
		return -1;
	}
	nRecvBufSize = stParam.nCurValue;
	m_pBufForDriver = (unsigned char*)malloc(nRecvBufSize);
	MV_FRAME_OUT_INFO_EX stImageInfo = { 0 };
	int tempValue = MV_CC_GetOneFrameTimeout(m_hDevHandle, m_pBufForDriver, nRecvBufSize, &stImageInfo, 700);
	if (tempValue != 0)
	{
		return -1;
	}
	m_nBufSizeForSaveImage = stImageInfo.nWidth * stImageInfo.nHeight * 3 + 2048;
	m_pBufForSaveImage = (unsigned char*)malloc(m_nBufSizeForSaveImage);

	bool isMono;//判断是否为黑白图像
	switch (stImageInfo.enPixelType)
	{
	case PixelType_Gvsp_Mono8:
	case PixelType_Gvsp_Mono10:
	case PixelType_Gvsp_Mono10_Packed:
	case PixelType_Gvsp_Mono12:
	case PixelType_Gvsp_Mono12_Packed:
		isMono = true;
		break;
	default:
		isMono = false;
		break;
	}

	if (isMono)
	{
		image = Mat(stImageInfo.nHeight, stImageInfo.nWidth, CV_8UC1, m_pBufForDriver);
	}
	else
	{
		//转换图像格式为BGR8
		MV_CC_PIXEL_CONVERT_PARAM stConvertParam = { 0 };
		memset(&stConvertParam, 0, sizeof(MV_CC_PIXEL_CONVERT_PARAM));
		stConvertParam.nWidth = stImageInfo.nWidth;                 //ch:图像宽 | en:image width
		stConvertParam.nHeight = stImageInfo.nHeight;               //ch:图像高 | en:image height
		stConvertParam.pSrcData = m_pBufForDriver;                  //ch:输入数据缓存 | en:input data buffer
		stConvertParam.nSrcDataLen = stImageInfo.nFrameLen;         //ch:输入数据大小 | en:input data size
		stConvertParam.enSrcPixelType = stImageInfo.enPixelType;    //ch:输入像素格式 | en:input pixel format
		//stConvertParam.enDstPixelType = PixelType_Gvsp_BGR8_Packed; //ch:输出像素格式 | en:output pixel format  适用于OPENCV的图像格式
		stConvertParam.enDstPixelType = PixelType_Gvsp_RGB8_Packed; //ch:输出像素格式 | en:output pixel format
		stConvertParam.pDstBuffer = m_pBufForSaveImage;                    //ch:输出数据缓存 | en:output data buffer
		stConvertParam.nDstBufferSize = m_nBufSizeForSaveImage;            //ch:输出缓存大小 | en:output buffer size
		MV_CC_ConvertPixelType(m_hDevHandle, &stConvertParam);
		image = Mat(stImageInfo.nHeight, stImageInfo.nWidth, CV_8UC3, m_pBufForSaveImage);
	}
	return 0;
}
//设置心跳时间
int Mycamera::setHeartBeatTime(unsigned int time)
{
	//心跳时间最小为500ms
	if (time < 500)
		time = 500;
	int temp = MV_CC_SetIntValue(m_hDevHandle, "GevHeartbeatTimeout", time);
	if (temp != 0)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}
//设置曝光时间
int Mycamera::setExposureTime(float ExposureTimeNum)
{
	int temp = MV_CC_SetFloatValue(m_hDevHandle, "ExposureTime", ExposureTimeNum);
	if (temp != 0)
		return -1;
	return 0;
}