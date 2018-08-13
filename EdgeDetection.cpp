#include "stdafx.h"
#include "EdgeDetection.h"

CEdgeDetection::CEdgeDetection(CDib2 * pDib)
{
	m_pDib = pDib;
}

CEdgeDetection::~CEdgeDetection()
{
}

void CEdgeDetection::Roberts()
{/*
	//源图像数据区指针
	LPBYTE lpData = m_pDib->GetData();
	//图像每行像素所占的字节数
	int nLineByte = m_pDib->GetLineByte();
	//图像宽度
	int nWidth = m_pDib->GetWidth();
	//图像高度
	int nHeight = m_pDib->GetHeight();
	//新图像缓冲区的指针
	LPBYTE lpTemp = new BYTE[nLineByte * nHeight];
	//初始化新分配的内存，设定初始值为255
	memset(lpTemp, 255, nLineByte * nHeight);
	//ROBERTS算子
	int pixel[4];
	//由于使用2x2的模板，为防止越界，不处理最上边和最右边的边界像素
	for (size_t j = 0; j < nHeight-1; j++)
	{
		for (int i = 0; i < nWidth - 1; i++)
		{
			//生成roberts算子
			pixel[0] = lpData[j*nLineByte + i];
			pixel[1] = lpData[j*nLineByte + i+1];
			pixel[2] = lpData[(j+1)*nLineByte + i];
			pixel[3] = lpData[(j+1)*nLineByte + i+1];
			//处理当前像素
			lpTemp[j*nLineByte + i] = sqrt(double(pixel[0] - pixel[3]) * (pixel[0] - pixel[3])
				+ ( pixel))

		}
	}
	*/
}
