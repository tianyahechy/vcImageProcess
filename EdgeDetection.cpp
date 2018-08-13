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
	//Դͼ��������ָ��
	LPBYTE lpData = m_pDib->GetData();
	//ͼ��ÿ��������ռ���ֽ���
	int nLineByte = m_pDib->GetLineByte();
	//ͼ����
	int nWidth = m_pDib->GetWidth();
	//ͼ��߶�
	int nHeight = m_pDib->GetHeight();
	//��ͼ�񻺳�����ָ��
	LPBYTE lpTemp = new BYTE[nLineByte * nHeight];
	//��ʼ���·�����ڴ棬�趨��ʼֵΪ255
	memset(lpTemp, 255, nLineByte * nHeight);
	//ROBERTS����
	int pixel[4];
	//����ʹ��2x2��ģ�壬Ϊ��ֹԽ�磬���������ϱߺ����ұߵı߽�����
	for (size_t j = 0; j < nHeight-1; j++)
	{
		for (int i = 0; i < nWidth - 1; i++)
		{
			//����roberts����
			pixel[0] = lpData[j*nLineByte + i];
			pixel[1] = lpData[j*nLineByte + i+1];
			pixel[2] = lpData[(j+1)*nLineByte + i];
			pixel[3] = lpData[(j+1)*nLineByte + i+1];
			//����ǰ����
			lpTemp[j*nLineByte + i] = sqrt(double(pixel[0] - pixel[3]) * (pixel[0] - pixel[3])
				+ ( pixel))

		}
	}
	*/
}
