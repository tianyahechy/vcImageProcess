#include "stdafx.h"
#include "HistogramDib.h"

CHistogramDib::CHistogramDib(CDib2 * dib)
{
	theDIB = dib;
}

CHistogramDib::~CHistogramDib()
{
}
//ֱ��ͼͳ��
void CHistogramDib::Histogram_Statistic(float* probability)
{
	//�ҵ�Դͼ�����ʼλ��
	LPBYTE lpDIBBits = theDIB->GetData();
	//���ͼ��Ŀ��
	long lWidth = theDIB->GetWidth();
	//���ͼ��ĸ߶�
	long lHeight = theDIB->GetHeight();
	//�Ҷȼ���������ʼ��
	int gray[256];
	memset(gray, 0, sizeof(gray));
	//���ɨ��ͼ���е����ص㣬���лҶȼ���ͳ��
	for (int j = 0; j < lHeight; j++)
	{
		for (int i = 0; i < lWidth; i++)
		{
			//ָ��Դͼ������j�е�i�����ص�ָ��
			LPBYTE lpSrc = (LPBYTE)lpDIBBits + lWidth * j + i;
			//Դ����ֵ
			BYTE pixel = (BYTE)*lpSrc;
			//�Ҷ�ͳ�Ƽ���
			gray[pixel]++;

		}
	}

	//����Ҷȸ����ܶ�
	for (int i = 0; i < 256; i++)
	{
		probability[i] = gray[i] / (lHeight * lWidth * 1.0);
	}
}