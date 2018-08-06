
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

//��ת
HGLOBAL Rotate(LPSTR lpSrcDib,		//ָ��ԴDIB��ָ��
	LPSTR lpSrcStartBits, //ָ��ԴDIB��ʼ���ص�ָ��
	long lWidth,	//ԴDIBͼ��Ŀ��
	long lHeight,	//ԴDIBͼ��ĸ߶�	
	long lLineBytes,//ԴDIBͼ������ֽ�����Ϊ4�ı��� 
	WORD palSize,	//ԴDIBͼ���ɫ���С
	long lDstWidth,	//Ŀ��ͼ����
	long lDstHeight,//Ŀ��ͼ��߶�
	long lDstLineBytes, //Ŀ��DIBͼ�����ֽ�����4�ı�����
	float fSina,	//��ת������
	float fCosa		//��ת������
	)
{
	
	//�������
	float varFloat1 = (float)(-0.5 * (lDstWidth - 1) * fCosa - 0.5 * (lDstHeight - 1) * fSina + 0.5 * (lDstWidth - 1));
	float varFloat2 = (float)(-0.5 * (lDstWidth - 1) * fSina - 0.5 * (lDstHeight - 1) * fCosa + 0.5 * (lDstHeight - 1));
	//�����ڴ棬�Ա�����DIB
	HGLOBAL hDIB = (HGLOBAL) ::GlobalAlloc(GHND, lDstLineBytes * lDstHeight + *(LPWORD)lpSrcDib + palSize);
	if (hDIB == NULL )
	{
		//�ж��Ƿ�����Ч��dib����
		return FALSE;
	}
	//�����ڴ�
	LPSTR lpDstDib = (char *) ::GlobalLock((HGLOBAL)hDIB);
	//����dib��Ϣͷ�͵�ɫ��
	memcpy(lpDstDib, lpSrcDib, *(LPDWORD)lpSrcDib + palSize);
	//��ȡָ��
	LPBITMAPINFOHEADER lpmi = (LPBITMAPINFOHEADER)lpDstDib;
	//����DIB��ͼ��ĸ߶ȺͿ��
	lpmi->biHeight = lDstHeight;
	lpmi->biWidth = lDstWidth;

	//����������ʼλ�ã�
	LPSTR lpDstStartBits = lpDstDib + *(LPWORD)lpDstDib + palSize;
	for (size_t i = 0; i < lDstHeight; i++)
	{
		for (size_t j = 0; j < lDstWidth; j++)
		{
			//ָ����dib��i�е�j�����ص�ָ��
			LPSTR lpDstDIBBits = (char *)lpDstStartBits + lDstLineBytes * (lDstHeight - 1 - i) + j;
			//�����������ԴDIB�е�����
			long i1 = (long)(-((float)j) * fSina + ((float)i) * fCosa + varFloat2 + 0.5);
			long j1 = (long)(((float)j)  * fCosa + ((float)i) * fSina + varFloat1 + 0.5);
			//�ж��Ƿ��ڷ�Χ��
			if ((j1 >= 0) && (j1 < lWidth) && (i1 >= 0) && (i1 < lHeight))
			{
				//ָ��Դdib��i1�е�j1�����ص�ָ��
				LPSTR lpSrcDIBBits = (char*)lpSrcStartBits + lLineBytes * (lHeight - 1 - i1) + j1;
				//��������
				*lpDstDIBBits = *lpSrcDIBBits;
			}
			else
			{
				//Դͼ�в����ڵ����أ���ֵΪ255
				*((unsigned char*)lpDstDIBBits) = 255;
			}
		}

	}
	
	return hDIB;
}

//����ת�ǶȴӶ�ת��Ϊ����
float AngleToRadian(float angle)
{
	float radian = angle * 3.1415926 / 180.0;
	return radian;
}

//ˮƽ����
BOOL Mirror(LPSTR lpSrcStartBits, //ָ��ԴDIB��ʼ���ص�ָ��
	long lWidth,	//DIBͼ��Ŀ��
	long lHeight,	//DIBͼ��ĸ߶�
	long lLineBytes //DIBͼ������ֽ�����Ϊ4�ı��� 
	)
{
	LPSTR lpSrcDIBBits;		//ָ��Դ���ص�ָ��
	LPSTR lpDstDIBBits;		//ָ����ʱͼ���Ӧ���ص�ָ��
	LPSTR lpBits;			//�м����
	HLOCAL hDstDIBBits = LocalAlloc(LHND, lLineBytes); //������ʱ�ڴ����ʱͼ����
	//�ж��Ƿ��ڴ����
	if (hDstDIBBits == NULL)  //�����ڴ�ʧ��
	{
		return FALSE;
	}
	//�����ڴ�
	lpDstDIBBits = (char *)LocalLock(hDstDIBBits);

	//ˮƽ�������ͼ��ÿ�н��в���
	for (size_t i = 0; i < lHeight; i++)  //��
	{
		//���ÿ��ͼ����벿�ֽ��в���
		for (int j = 0; j < lWidth/2; j++)
		{	
			//ָ��ԴDIB������i�У���j�����ص�ָ��
			lpSrcDIBBits = (char *)lpSrcStartBits + lLineBytes * i + j;
			//ָ����DIB��i�У���j�����ص�ָ��
			lpBits = (char *)lpSrcStartBits + lLineBytes * (i + 1) - j;
			
			//����
			*lpDstDIBBits = *lpBits;
			*lpBits = *lpSrcDIBBits;
			*lpSrcDIBBits = *lpDstDIBBits;
		}
	}

	//�ͷ��ڴ�
	LocalUnlock(hDstDIBBits);
	LocalFree(hDstDIBBits);

	return TRUE;

}

//��ֱ����
BOOL Mirror2(LPSTR lpSrcStartBits, //ָ��ԴDIB��ʼ���ص�ָ��
	long lWidth,	//DIBͼ��Ŀ��
	long lHeight,	//DIBͼ��ĸ߶�
	long lLineBytes //DIBͼ������ֽ�����Ϊ4�ı��� 
	)
{
	LPSTR lpSrcDIBBits;		//ָ��Դ���ص�ָ��
	LPSTR lpDstDIBBits;		//ָ����ʱͼ���Ӧ���ص�ָ��
	LPSTR lpBits;			//�м����
	HLOCAL hDstDIBBits = LocalAlloc(LHND, lLineBytes); //������ʱ�ڴ����ʱͼ����
	//�ж��Ƿ��ڴ����
	if (hDstDIBBits == NULL)  //�����ڴ�ʧ��
	{
		return FALSE;
	}
	//�����ڴ�
	lpDstDIBBits = (char *)LocalLock(hDstDIBBits);

	//ˮƽ�������ͼ��ÿ�н��в���
	for (size_t i = 0; i < lHeight/2; i++)  //��
	{
		//ָ��ԴDIB������i�У���j�����ص�ָ��
		lpSrcDIBBits = (char *)lpSrcStartBits + lLineBytes * i;
		//ָ����DIB��i�У���j�����ص�ָ��
		lpBits = (char *)lpSrcStartBits + lLineBytes * (lHeight-i+1);

		//����
		memcpy(lpDstDIBBits, lpBits, lLineBytes);
		memcpy(lpBits, lpSrcDIBBits, lLineBytes);
		memcpy(lpSrcDIBBits, lpDstDIBBits, lLineBytes);
	
	}

	//�ͷ��ڴ�
	LocalUnlock(hDstDIBBits);
	LocalFree(hDstDIBBits);

	return TRUE;

}

//����ͼ��ת��
BOOL Transpose(
	LPSTR lpSrcDib,			//ָ��ԴDIB��ָ��
	LPSTR lpSrcStartBits, //ָ��ԴDIB��ʼ���ص�ָ��
	long lWidth,	//DIBͼ��Ŀ��
	long lHeight,	//DIBͼ��ĸ߶�
	long lLineBytes,//DIBͼ������ֽ�����Ϊ4�ı��� 
	long lDstLineBytes//��ʱDIBͼ������ֽ�����Ϊ4�ı���
	)
{
	LPSTR lpSrcDIBBits;		//ָ��Դ���ص�ָ��
	LPSTR lpDstDIBBits;		//ָ����ʱͼ���Ӧ���ص�ָ��
	LPSTR lpDstStartBits;	//ָ����ʱͼ���Ӧ���ص�ָ��
	LPBITMAPINFOHEADER lpbmi = ( LPBITMAPINFOHEADER) lpSrcDib;//ָ��BITMAPINFOHEADER�ṹ��ָ��
	HLOCAL hDstDIBBits = LocalAlloc(LHND, lWidth * lDstLineBytes); //������ʱ�ڴ����ʱͼ����	
	//�ж��Ƿ��ڴ����
	if (hDstDIBBits == NULL)  //�����ڴ�ʧ��
	{
		return FALSE;
	}
	//�����ڴ�
	lpDstStartBits = (char *)LocalLock(hDstDIBBits);

	for (size_t i = 0; i < lHeight; i++)  //��
	{
		for (int j = 0; j < lWidth; j++)
		{
			//ָ��ԴDIB��i�У���j�����ص�ָ��
			lpSrcDIBBits = (char *)lpSrcStartBits + lLineBytes * (lHeight - 1 - i ) + j;
			//ָ����DIB��i�У���j�����ص�ָ��
			lpDstDIBBits = (char *)lpDstStartBits + lDstLineBytes * (lWidth - 1 - j) + i;
			//��������
			*lpDstDIBBits = *lpSrcDIBBits;
	
		}
	}
	//����ͼ��
	memcpy(lpSrcStartBits, lpDstStartBits, lWidth * lDstLineBytes );
	lpbmi->biWidth = lHeight;
	lpbmi->biHeight = lWidth;
	//�ͷ��ڴ�
	LocalUnlock(hDstDIBBits);
	LocalFree(hDstDIBBits);

	return TRUE;

}

//˫���Բ�ֵʵ��ͼ����ת
HGLOBAL Rotate(LPSTR lpSrcDib,		//ָ��ԴDIB��ָ��
	float fRotateAngle,		//��ת�ĽǶ�
	LPSTR lpSrcStartBits, //ָ��ԴDIB��ʼ���ص�ָ��
	long lWidth,	//ԴDIBͼ��Ŀ��
	long lHeight,	//ԴDIBͼ��ĸ߶�	
	long lLineBytes,//ԴDIBͼ������ֽ�����Ϊ4�ı��� 
	WORD palSize,	//ԴDIBͼ���ɫ���С
	long lDstWidth,	//Ŀ��ͼ����
	long lDstHeight,//Ŀ��ͼ��߶�
	long lDstLineBytes, //Ŀ��DIBͼ�����ֽ�����4�ı�����
	float fSina,	//��ת������
	float fCosa		//��ת������
	)
{

	//�������
	float varFloat1 = (float)(-0.5 * (lDstWidth - 1) * fCosa - 0.5 * (lDstHeight - 1) * fSina + 0.5 * (lDstWidth - 1));
	float varFloat2 = (float)(-0.5 * (lDstWidth - 1) * fSina - 0.5 * (lDstHeight - 1) * fCosa + 0.5 * (lDstHeight - 1));
	//�����ڴ棬�Ա�����DIB
	HGLOBAL hDIB = (HGLOBAL) ::GlobalAlloc(GHND, lDstLineBytes * lDstHeight + *(LPWORD)lpSrcDib + palSize);
	if (hDIB == NULL)
	{
		//�ж��Ƿ�����Ч��dib����
		return FALSE;
	}
	//�����ڴ�
	LPSTR lpDstDib = (char *) ::GlobalLock((HGLOBAL)hDIB);
	//����dib��Ϣͷ�͵�ɫ��
	memcpy(lpDstDib, lpSrcDib, *(LPDWORD)lpSrcDib + palSize);
	//��ȡָ��
	LPBITMAPINFOHEADER lpmi = (LPBITMAPINFOHEADER)lpDstDib;
	//����DIB��ͼ��ĸ߶ȺͿ��
	lpmi->biHeight = lDstHeight;
	lpmi->biWidth = lDstWidth;

	//����������ʼλ�ã�
	LPSTR lpDstStartBits = lpDstDib + *(LPWORD)lpDstDib + palSize;
	for (size_t i = 0; i < lDstHeight; i++)
	{
		for (size_t j = 0; j < lDstWidth; j++)
		{
			//ָ����dib��i�е�j�����ص�ָ��
			LPSTR lpDstDIBBits = (char *)lpDstStartBits + lDstLineBytes * (lDstHeight - 1 - i) + j;
			//�����������ԴDIB�е�����
			long i1 = (long)(-((float)j) * fSina + ((float)i) * fCosa + varFloat2 + 0.5);
			long j1 = (long)(((float)j)  * fCosa + ((float)i) * fSina + varFloat1 + 0.5);
			//�ж��Ƿ��ڷ�Χ��
			if ((j1 >= 0) && (j1 < lWidth) && (i1 >= 0) && (i1 < lHeight))
			{
				//ָ��Դdib��i1�е�j1�����ص�ָ��
				LPSTR lpSrcDIBBits = (char*)lpSrcStartBits + lLineBytes * (lHeight - 1 - i1) + j1;
				//��������
				*lpDstDIBBits = *lpSrcDIBBits;
			}
			else
			{
				//Դͼ�в����ڵ����أ���ֵΪ255
				*((unsigned char*)lpDstDIBBits) = 255;
			}
		}

	}

	return hDIB;
}
