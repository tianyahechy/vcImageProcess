
#pragma once
//����ͼ��ƽ��
BOOL Translation(LPSTR lpSrcStartBits, //ָ��ԴDIB��ʼ���ص�ָ��
	long lWidth,	//DIBͼ��Ŀ��
	long lHeight,	//DIBͼ��ĸ߶�
	long lXOffset,	//X����ƫ����
	long lYOffset,	//Y����ƫ����
	long lLineBytes,//DIBͼ������ֽ�����Ϊ4�ı��� 
	long lDstLineBytes//��ʱDIBͼ������ֽ�����Ϊ4�ı���
	)
{
	LPSTR lpSrcDIBBits;		//ָ��Դ���ص�ָ��
	LPSTR lpDstDIBBits;		//ָ����ʱͼ���Ӧ���ص�ָ��
	LPSTR lpDstStartBits;	//ָ����ʱͼ���Ӧ���ص�ָ��
	HLOCAL hDstDIBBits = LocalAlloc(LHND, lWidth * lDstLineBytes); //������ʱ�ڴ����ʱͼ����
	//�����ڴ�
	lpDstStartBits = (char *)LocalLock(hDstDIBBits);
	//�ж��Ƿ��ڴ����
	if (hDstDIBBits == NULL)  //�����ڴ�ʧ��
	{
		return FALSE;
	}
	for (size_t i = 0; i < lHeight; i++)  //��
	{
		for (int j = 0; j < lWidth; j++)
		{
			//ָ����DIB��i�У���j�����ص�ָ��
			lpDstDIBBits = (char *)lpDstStartBits + lLineBytes * (lHeight - 1 - i) + j;
			//�ж��Ƿ���Դͼ��Χ��
			if ((j - lYOffset >= 0) &&
				(j - lYOffset  < lWidth) &&
				(i - lXOffset >= 0) &&
				(i - lXOffset < lHeight)
				)
			{
				//ָ��ԴDIB��i�У���j�����ص�ָ��
				lpSrcDIBBits = (char *)lpSrcStartBits + lLineBytes * (lHeight - 1 - (i - lXOffset)) + (j - lYOffset);
				//��������
				*lpDstDIBBits = *lpSrcDIBBits;
			}
			else
			{
				//Դͼ��û�е����أ���Ϊ255
				*((unsigned char *)lpDstDIBBits) = 255;
			}
		}
	}
	//����ͼ��
	memcpy(lpSrcStartBits, lpDstStartBits, lLineBytes * lHeight);
	//�ͷ��ڴ�
	LocalUnlock(hDstDIBBits);
	LocalFree(hDstDIBBits);

	return TRUE;

}
