
#pragma once
#define WIDTHBYTES(bits)	(((bits) + 31) / 32 * 4)
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

//����
HGLOBAL Zoom(LPSTR lpSrcDib,			//ָ��ԴDIB��ָ��
	LPSTR lpSrcStartBits,				//ָ��ԴDIB����ʼ���ص�ָ��
	long lWidth,						//ԴDIBͼ����
	long lHeight,						//ԴDIBͼ��߶�
	long lLineBytes,					//ԴDIBͼ���ֽڿ�ȣ�4�ı���)
	WORD palSize,						//ԴDIBͼ���ɫ���С
	long lDstWidth,						//Ŀ��ͼ����
	long lDstHeight,					//Ŀ��ͼ��߶�
	long lDstLineBytes,					//Ŀ��DIBͼ�����ֽ�����4�ı�����
	float fhorRatio,					//ˮƽ���ű���
	float fVerRatio						//��ֱ���ű���
	)
{
	LPSTR lpSrcDIBBits;		//ָ��Դ���ص�ָ��
	LPSTR lpDstDIBBits;		//ָ����ʱͼ���Ӧ���ص�ָ��
	LPSTR lpDstStartBits;	//ָ����ʱͼ���Ӧ���ص�ָ��
	//�����ڴ棬�Ա������ź��DIB
	HGLOBAL hDIB = (HGLOBAL)::GlobalAlloc(GHND, lDstLineBytes * lDstHeight + *(LPDWORD)lpSrcDib + palSize);
	if (hDIB == NULL)
	{
		return FALSE;
	}
	//�����ڴ�
	LPSTR lpdstDib = (char *)::GlobalLock((HGLOBAL)hDIB);
	//����DIB��Ϣͷ�͵�ɫ��
	memcpy(lpdstDib, lpSrcDib, *(LPDWORD)lpSrcDib + palSize);
	//�ҵ���DIB��������ʼλ��
	lpDstStartBits = lpdstDib + *(LPDWORD)lpdstDib + palSize;
	//��ȡָ��BITMAPINFO�ṹ��ָ��
	LPBITMAPINFOHEADER lpbmi = (LPBITMAPINFOHEADER)lpdstDib;
	//����DIBͼ���еĸ߶ȺͿ��
	lpbmi->biWidth = lDstWidth;
	lpbmi->biHeight = lDstHeight;
	long i1;
	long j1;
	for (size_t i = 0; i < lDstHeight; i++)  //��
	{
		for (int j = 0; j < lDstWidth; j++)
		{
			//ָ����DIB��i�У���j�����ص�ָ��
			lpDstDIBBits = (char *)lpDstStartBits + lDstLineBytes * (lDstHeight - 1 - i) + j;
			//�����������ԴDIB�е�����
			i1 = (long)(i / fVerRatio + 0.5);
			j1 = (long)(j / fhorRatio + 0.5);
			//�ж��Ƿ��ڷ�Χ��
			if( ( j1 >= 0 ) && ( j1 < lWidth) && ( i1 >= 0 ) && ( i1 < lHeight ))
			{
				//ָ��Դͼ���i1�е�j1�����ص�ָ��
				lpSrcDIBBits = (char *)lpSrcStartBits + lLineBytes * (lHeight - 1 - i1) + j1;
				//��������
				*lpDstDIBBits = *lpSrcDIBBits;
			}
			else
			{
				//Դͼ���в����ڵ����ظ�ֵΪ255
				*((unsigned char *)lpDstDIBBits) = 255;
			}

		}
	}

	return hDIB;

}

//����˫���Բ�ֵ�㷨��������ֵ
unsigned char Interpolation(
	LPSTR lpDIBBits,		//ָ��ԴDIBͼ��ָ��
	LONG lWidth,			//Դͼ���ȣ���������
	LONG lHeight,			//Դͼ��߶ȣ�������)
	FLOAT x,				//��ֵԪ�ص�x����
	FLOAT y					//��ֵԪ�ص�y����
	)
{
	//4�����ڽ����ص�����(i1,j1)��(i2,j1),(i1,j2),(i2,j2)
	LONG i1 = (LONG)x;
	LONG i2 = i1 + 1;
	LONG j1 = (LONG)y;
	LONG j2 = j1 + 1;
	//ͼ��ÿ���ֽ���
	long lLineBytes = WIDTHBYTES(lWidth * 8);
	//����һ��ֵ���������������С�ڸ�ֵʱ����Ϊ������ͬ
	float EXP = (FLOAT) 0.0001;
	//���ݲ�ͬ����ֱ���
	//�������ĵ㲻��Դͼ��Χ�ڣ�ֱ�ӷ���255
	if ((x < 0) || (x > lWidth - 1) || (y < 0) || ( y>lHeight - 1) )
	{
		return 255;
	}
	else
	{
		if (fabs(x - lWidth + 1) <= EXP)
		{
			//Ҫ����ĵ���ͼ���ұ�Ե��
			if (fabs(y - lHeight + 1) <= EXP)
			{
				//Ҫ����ĵ�������ͼ�������½��Ǹ����أ�ֱ�ӷ��ظõ�����ֵ
				unsigned char f1 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j1) + i1);
				return f1;
			}
			else
			{
				//��ͼ���ұ�Ե���Ҳ������һ�㣬ֱ��һ�β�ֵ����.
				unsigned char f1 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j1) + i1);
				unsigned char f2 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j1) + i2);
				return ((unsigned char)(f1 + (y - j1) * (f2 - f1)));
			}

		}
		else if (fabs(y - lHeight + 1) <= EXP )
		{
			//��ͼ���±�Ե���Ҳ������һ�㣬ֱ��һ�β�ֵ����.
			unsigned char f1 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j1) + i1);
			unsigned char f2 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j2) + i1);
			return ((unsigned char)(f1 + (x - i1) * (f2 - f1)));

		}
		else
		{
			//����4�����ڽ�����ֵ
			unsigned char f1 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j1) + i1);
			unsigned char f2 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j1) + i2);
			unsigned char f3 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j2) + i1);
			unsigned char f4 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j2) + i2);
			//��ֵ1
			unsigned char f12 = ((unsigned char)(f1 + (x - i1) * (f2 - f1)));
			//��ֵ2
			unsigned char f34 = ((unsigned char)(f3 + (x - i1) * (f4 - f3)));
			//��ֵ3
			return ((unsigned char)(f12 + (y - j1) * (f34 - f12)));
		}
	}
}
//˫���Բ�ֵʵ��ͼ����ת
HGLOBAL Rotate_dib2(LPSTR lpSrcDib,		//ָ��ԴDIB��ָ��
	float fRotateAngle,		//��ת�ĽǶ�
	LPSTR lpSrcStartBits, //ָ��ԴDIB��ʼ���ص�ָ��
	long lWidth,	//ԴDIBͼ��Ŀ��
	long lHeight,	//ԴDIBͼ��ĸ߶�	
	WORD palSize	//ԴDIBͼ���ɫ���С
	)
{
	//�ҵ�ԴDIBͼ��������ʼλ��
	LPSTR lpDIBBits = lpSrcStartBits;
	//��ת�Ƕȵ����Һ�����
	float fSina = (float)sin((double)fRotateAngle);
	float fCosa = (float)cos((double)fRotateAngle);
	//����ͼ���ĸ��ǵ����꣨��ͼ������Ϊ����ϵԭ��)
	float fSrcX1 = (float)(-(lWidth - 1) / 2);
	float fSrcY1 = (float)((lHeight - 1) / 2);
	float fSrcX2 = (float)((lWidth - 1) / 2);
	float fSrcY2 = (float)((lHeight - 1) / 2);
	float fSrcX3 = (float)(-(lWidth - 1) / 2);
	float fSrcY3 = (float)(-(lHeight - 1) / 2);
	float fSrcX4 = (float)((lWidth - 1) / 2);
	float fSrcY4 = (float)(-(lHeight - 1) / 2);
	//������ͼ��4���Ƕȵ����꣨��ͼ������Ϊ����ϵԭ�㣩
	float fDstX1 = fCosa * fSrcX1 + fSina * fSrcY1;
	float fDstY1 = -fSina * fSrcX1 + fCosa * fSrcY1;
	float fDstX2 = fCosa * fSrcX2 + fSina * fSrcY2;
	float fDstY2 = -fSina * fSrcX2 + fCosa * fSrcY2;
	float fDstX3 = fCosa * fSrcX3 + fSina * fSrcY3;
	float fDstY3 = -fSina * fSrcX3 + fCosa * fSrcY3;
	float fDstX4 = fCosa * fSrcX4 + fSina * fSrcY4;
	float fDstY4 = -fSina * fSrcX4 + fCosa * fSrcY4;
	//������ת���ͼ��ʵ�ʿ��
	LONG lNewWidth = (LONG)(max(abs(fDstX4 - fDstX1), fabs(fDstX3 - fDstX2)) + 0.5);
	LONG lNewLineBytes = WIDTHBYTES(lNewWidth * 8);
	//������ת���ͼ��ʵ�ʸ߶�
	LONG lNewHeight = (LONG)(max(abs(fDstY4 - fDstY1), fabs(fDstY3 - fDstY2)) + 0.5);

	//�������
	float f1 = (float)(-0.5 * (lNewWidth - 1) * fCosa - 0.5 * (lNewHeight - 1) * fSina + 0.5 * (lNewWidth - 1));
	float f2 = (float)(-0.5 * (lNewWidth - 1) * fSina - 0.5 * (lNewHeight - 1) * fCosa + 0.5 * (lNewHeight - 1));
	//�����ڴ棬�Ա�����DIB
	HGLOBAL hDIB = (HGLOBAL) ::GlobalAlloc(GHND, lNewLineBytes * lNewHeight + *(LPWORD)lpSrcDib + palSize);
	if (hDIB == NULL)
	{
		//�ж��Ƿ�����Ч��dib����
		return FALSE;
	}
	//�����ڴ�
	LPSTR lpNewDIB = (char *) ::GlobalLock((HGLOBAL)hDIB);
	//����dib��Ϣͷ�͵�ɫ��
	memcpy(lpNewDIB, lpSrcDib, *(LPDWORD)lpSrcDib + palSize);
	//�ҵ���DIB������ʼλ��
	LPSTR lpNewDIBBits = lpNewDIB + *(LPDWORD)lpSrcDib + palSize;
	//��ȡָ��
	LPBITMAPINFOHEADER lpbmi = (LPBITMAPINFOHEADER)lpNewDIB;
	LPBITMAPCOREHEADER lpbmc = (LPBITMAPCOREHEADER)lpNewDIB;
	//����DIB��ͼ��ĸ߶ȺͿ��
	lpbmi->biHeight = lNewHeight;
	lpbmi->biWidth = lNewWidth;

	for (size_t i = 0; i < lNewHeight; i++)
	{
		for (size_t j = 0; j < lNewWidth; j++)
		{
			//ָ����dib��i�е�j�����ص�ָ��
			LPSTR lpDst = (char *)lpNewDIBBits + lNewLineBytes * (lNewHeight - 1 - i) + j;
			//�����������ԴDIB�е�����
			long i0 = (long)(-((float)j) * fSina + ((float)i) * fCosa + f2 );
			long j0 = (long)(((float)j)  * fCosa + ((float)i) * fSina + f1 );
	
			//����˫���Բ�ֵ�㷨����������ֵ
			*lpDst = Interpolation(lpDIBBits, lWidth, lHeight, j0, i0);
		
	
		}

	}

	return hDIB;
}
