#include "stdafx.h"
#include "HistogramDib.h"

CHistogramDib::CHistogramDib(CDib2 * dib)
{
	theDIB = dib;
}

CHistogramDib::~CHistogramDib()
{
}
//直方图统计
void CHistogramDib::Histogram_Statistic(float* probability)
{
	//找到源图像的起始位置
	LPBYTE lpDIBBits = theDIB->GetData();
	//获得图像的宽度
	long lWidth = theDIB->GetWidth();
	//获得图像的高度
	long lHeight = theDIB->GetHeight();
	//灰度计数变量初始化
	int gray[256];
	memset(gray, 0, sizeof(gray));
	//逐个扫描图像中的像素点，进行灰度计数统计
	for (int j = 0; j < lHeight; j++)
	{
		for (int i = 0; i < lWidth; i++)
		{
			//指向源图像倒数第j行第i个像素的指针
			LPBYTE lpSrc = (LPBYTE)lpDIBBits + lWidth * j + i;
			//源像素值
			BYTE pixel = (BYTE)*lpSrc;
			//灰度统计计数
			gray[pixel]++;

		}
	}

	//计算灰度概率密度
	for (int i = 0; i < 256; i++)
	{
		probability[i] = gray[i] / (lHeight * lWidth * 1.0);
	}
}