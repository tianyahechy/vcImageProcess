
#pragma once
#define WIDTHBYTES(bits)	(((bits) + 31) / 32 * 4)
//数字图像平移
BOOL Translation(LPSTR lpSrcStartBits, //指向源DIB起始像素的指针
	long lWidth,	//DIB图像的宽度
	long lHeight,	//DIB图像的高度
	long lXOffset,	//X方向偏移量
	long lYOffset,	//Y方向偏移量
	long lLineBytes,//DIB图像的行字节数，为4的倍数 
	long lDstLineBytes//临时DIB图像的行字节数，为4的倍数
	)
{
	LPSTR lpSrcDIBBits;		//指向源像素的指针
	LPSTR lpDstDIBBits;		//指向临时图像对应像素的指针
	LPSTR lpDstStartBits;	//指向临时图像对应像素的指针
	HLOCAL hDstDIBBits = LocalAlloc(LHND, lWidth * lDstLineBytes); //分配临时内存给临时图像句柄
	//锁定内存
	lpDstStartBits = (char *)LocalLock(hDstDIBBits);
	//判断是否内存分配
	if (hDstDIBBits == NULL)  //分配内存失败
	{
		return FALSE;
	}
	for (size_t i = 0; i < lHeight; i++)  //行
	{
		for (int j = 0; j < lWidth; j++)
		{
			//指向新DIB第i行，第j个像素的指针
			lpDstDIBBits = (char *)lpDstStartBits + lLineBytes * (lHeight - 1 - i) + j;
			//判断是否在源图范围内
			if ((j - lYOffset >= 0) &&
				(j - lYOffset  < lWidth) &&
				(i - lXOffset >= 0) &&
				(i - lXOffset < lHeight)
				)
			{
				//指向源DIB第i行，第j个像素的指针
				lpSrcDIBBits = (char *)lpSrcStartBits + lLineBytes * (lHeight - 1 - (i - lXOffset)) + (j - lYOffset);
				//复制像素
				*lpDstDIBBits = *lpSrcDIBBits;
			}
			else
			{
				//源图中没有的像素，赋为255
				*((unsigned char *)lpDstDIBBits) = 255;
			}
		}
	}
	//复制图像
	memcpy(lpSrcStartBits, lpDstStartBits, lLineBytes * lHeight);
	//释放内存
	LocalUnlock(hDstDIBBits);
	LocalFree(hDstDIBBits);

	return TRUE;

}

//旋转
HGLOBAL Rotate(LPSTR lpSrcDib,		//指向源DIB的指针
	LPSTR lpSrcStartBits, //指向源DIB起始像素的指针
	long lWidth,	//源DIB图像的宽度
	long lHeight,	//源DIB图像的高度	
	long lLineBytes,//源DIB图像的行字节数，为4的倍数 
	WORD palSize,	//源DIB图像调色板大小
	long lDstWidth,	//目标图像宽度
	long lDstHeight,//目标图像高度
	long lDstLineBytes, //目标DIB图像行字节数（4的倍数）
	float fSina,	//旋转角正弦
	float fCosa		//旋转角余弦
	)
{
	
	//浮点参数
	float varFloat1 = (float)(-0.5 * (lDstWidth - 1) * fCosa - 0.5 * (lDstHeight - 1) * fSina + 0.5 * (lDstWidth - 1));
	float varFloat2 = (float)(-0.5 * (lDstWidth - 1) * fSina - 0.5 * (lDstHeight - 1) * fCosa + 0.5 * (lDstHeight - 1));
	//分配内存，以保存新DIB
	HGLOBAL hDIB = (HGLOBAL) ::GlobalAlloc(GHND, lDstLineBytes * lDstHeight + *(LPWORD)lpSrcDib + palSize);
	if (hDIB == NULL )
	{
		//判断是否是有效的dib对象
		return FALSE;
	}
	//锁定内存
	LPSTR lpDstDib = (char *) ::GlobalLock((HGLOBAL)hDIB);
	//复制dib信息头和调色板
	memcpy(lpDstDib, lpSrcDib, *(LPDWORD)lpSrcDib + palSize);
	//获取指针
	LPBITMAPINFOHEADER lpmi = (LPBITMAPINFOHEADER)lpDstDib;
	//更新DIB中图像的高度和宽度
	lpmi->biHeight = lDstHeight;
	lpmi->biWidth = lDstWidth;

	//计算像素起始位置，
	LPSTR lpDstStartBits = lpDstDib + *(LPWORD)lpDstDib + palSize;
	for (size_t i = 0; i < lDstHeight; i++)
	{
		for (size_t j = 0; j < lDstWidth; j++)
		{
			//指向新dib第i行第j个像素的指针
			LPSTR lpDstDIBBits = (char *)lpDstStartBits + lDstLineBytes * (lDstHeight - 1 - i) + j;
			//计算该像素在源DIB中的坐标
			long i1 = (long)(-((float)j) * fSina + ((float)i) * fCosa + varFloat2 + 0.5);
			long j1 = (long)(((float)j)  * fCosa + ((float)i) * fSina + varFloat1 + 0.5);
			//判断是否在范围内
			if ((j1 >= 0) && (j1 < lWidth) && (i1 >= 0) && (i1 < lHeight))
			{
				//指向源dib第i1行第j1个像素的指针
				LPSTR lpSrcDIBBits = (char*)lpSrcStartBits + lLineBytes * (lHeight - 1 - i1) + j1;
				//复制像素
				*lpDstDIBBits = *lpSrcDIBBits;
			}
			else
			{
				//源图中不存在的像素，赋值为255
				*((unsigned char*)lpDstDIBBits) = 255;
			}
		}

	}
	
	return hDIB;
}

//将旋转角度从度转换为弧度
float AngleToRadian(float angle)
{
	float radian = angle * 3.1415926 / 180.0;
	return radian;
}

//水平镜像
BOOL Mirror(LPSTR lpSrcStartBits, //指向源DIB起始像素的指针
	long lWidth,	//DIB图像的宽度
	long lHeight,	//DIB图像的高度
	long lLineBytes //DIB图像的行字节数，为4的倍数 
	)
{
	LPSTR lpSrcDIBBits;		//指向源像素的指针
	LPSTR lpDstDIBBits;		//指向临时图像对应像素的指针
	LPSTR lpBits;			//中间变量
	HLOCAL hDstDIBBits = LocalAlloc(LHND, lLineBytes); //分配临时内存给临时图像句柄
	//判断是否内存分配
	if (hDstDIBBits == NULL)  //分配内存失败
	{
		return FALSE;
	}
	//锁定内存
	lpDstDIBBits = (char *)LocalLock(hDstDIBBits);

	//水平镜像，针对图像每行进行操作
	for (size_t i = 0; i < lHeight; i++)  //行
	{
		//针对每行图像左半部分进行操作
		for (int j = 0; j < lWidth/2; j++)
		{	
			//指向源DIB倒数第i行，第j个像素的指针
			lpSrcDIBBits = (char *)lpSrcStartBits + lLineBytes * i + j;
			//指向新DIB第i行，第j个像素的指针
			lpBits = (char *)lpSrcStartBits + lLineBytes * (i + 1) - j;
			
			//交换
			*lpDstDIBBits = *lpBits;
			*lpBits = *lpSrcDIBBits;
			*lpSrcDIBBits = *lpDstDIBBits;
		}
	}

	//释放内存
	LocalUnlock(hDstDIBBits);
	LocalFree(hDstDIBBits);

	return TRUE;

}

//垂直镜像
BOOL Mirror2(LPSTR lpSrcStartBits, //指向源DIB起始像素的指针
	long lWidth,	//DIB图像的宽度
	long lHeight,	//DIB图像的高度
	long lLineBytes //DIB图像的行字节数，为4的倍数 
	)
{
	LPSTR lpSrcDIBBits;		//指向源像素的指针
	LPSTR lpDstDIBBits;		//指向临时图像对应像素的指针
	LPSTR lpBits;			//中间变量
	HLOCAL hDstDIBBits = LocalAlloc(LHND, lLineBytes); //分配临时内存给临时图像句柄
	//判断是否内存分配
	if (hDstDIBBits == NULL)  //分配内存失败
	{
		return FALSE;
	}
	//锁定内存
	lpDstDIBBits = (char *)LocalLock(hDstDIBBits);

	//水平镜像，针对图像每行进行操作
	for (size_t i = 0; i < lHeight/2; i++)  //行
	{
		//指向源DIB倒数第i行，第j个像素的指针
		lpSrcDIBBits = (char *)lpSrcStartBits + lLineBytes * i;
		//指向新DIB第i行，第j个像素的指针
		lpBits = (char *)lpSrcStartBits + lLineBytes * (lHeight-i+1);

		//交换
		memcpy(lpDstDIBBits, lpBits, lLineBytes);
		memcpy(lpBits, lpSrcDIBBits, lLineBytes);
		memcpy(lpSrcDIBBits, lpDstDIBBits, lLineBytes);
	
	}

	//释放内存
	LocalUnlock(hDstDIBBits);
	LocalFree(hDstDIBBits);

	return TRUE;

}

//数字图像转置
BOOL Transpose(
	LPSTR lpSrcDib,			//指向源DIB的指针
	LPSTR lpSrcStartBits, //指向源DIB起始像素的指针
	long lWidth,	//DIB图像的宽度
	long lHeight,	//DIB图像的高度
	long lLineBytes,//DIB图像的行字节数，为4的倍数 
	long lDstLineBytes//临时DIB图像的行字节数，为4的倍数
	)
{
	LPSTR lpSrcDIBBits;		//指向源像素的指针
	LPSTR lpDstDIBBits;		//指向临时图像对应像素的指针
	LPSTR lpDstStartBits;	//指向临时图像对应像素的指针
	LPBITMAPINFOHEADER lpbmi = ( LPBITMAPINFOHEADER) lpSrcDib;//指向BITMAPINFOHEADER结构的指针
	HLOCAL hDstDIBBits = LocalAlloc(LHND, lWidth * lDstLineBytes); //分配临时内存给临时图像句柄	
	//判断是否内存分配
	if (hDstDIBBits == NULL)  //分配内存失败
	{
		return FALSE;
	}
	//锁定内存
	lpDstStartBits = (char *)LocalLock(hDstDIBBits);

	for (size_t i = 0; i < lHeight; i++)  //行
	{
		for (int j = 0; j < lWidth; j++)
		{
			//指向源DIB第i行，第j个像素的指针
			lpSrcDIBBits = (char *)lpSrcStartBits + lLineBytes * (lHeight - 1 - i ) + j;
			//指向新DIB第i行，第j个像素的指针
			lpDstDIBBits = (char *)lpDstStartBits + lDstLineBytes * (lWidth - 1 - j) + i;
			//复制像素
			*lpDstDIBBits = *lpSrcDIBBits;
	
		}
	}
	//复制图像
	memcpy(lpSrcStartBits, lpDstStartBits, lWidth * lDstLineBytes );
	lpbmi->biWidth = lHeight;
	lpbmi->biHeight = lWidth;
	//释放内存
	LocalUnlock(hDstDIBBits);
	LocalFree(hDstDIBBits);

	return TRUE;

}

//缩放
HGLOBAL Zoom(LPSTR lpSrcDib,			//指向源DIB的指针
	LPSTR lpSrcStartBits,				//指向源DIB的起始像素的指针
	long lWidth,						//源DIB图像宽度
	long lHeight,						//源DIB图像高度
	long lLineBytes,					//源DIB图像字节宽度（4的倍数)
	WORD palSize,						//源DIB图像调色板大小
	long lDstWidth,						//目标图像宽度
	long lDstHeight,					//目标图像高度
	long lDstLineBytes,					//目标DIB图像行字节数（4的倍数）
	float fhorRatio,					//水平缩放比率
	float fVerRatio						//垂直缩放比率
	)
{
	LPSTR lpSrcDIBBits;		//指向源像素的指针
	LPSTR lpDstDIBBits;		//指向临时图像对应像素的指针
	LPSTR lpDstStartBits;	//指向临时图像对应像素的指针
	//分配内存，以保存缩放后的DIB
	HGLOBAL hDIB = (HGLOBAL)::GlobalAlloc(GHND, lDstLineBytes * lDstHeight + *(LPDWORD)lpSrcDib + palSize);
	if (hDIB == NULL)
	{
		return FALSE;
	}
	//锁定内存
	LPSTR lpdstDib = (char *)::GlobalLock((HGLOBAL)hDIB);
	//复制DIB信息头和调色板
	memcpy(lpdstDib, lpSrcDib, *(LPDWORD)lpSrcDib + palSize);
	//找到新DIB的像素起始位置
	lpDstStartBits = lpdstDib + *(LPDWORD)lpdstDib + palSize;
	//获取指向BITMAPINFO结构的指针
	LPBITMAPINFOHEADER lpbmi = (LPBITMAPINFOHEADER)lpdstDib;
	//更新DIB图像中的高度和宽度
	lpbmi->biWidth = lDstWidth;
	lpbmi->biHeight = lDstHeight;
	long i1;
	long j1;
	for (size_t i = 0; i < lDstHeight; i++)  //行
	{
		for (int j = 0; j < lDstWidth; j++)
		{
			//指向新DIB第i行，第j个像素的指针
			lpDstDIBBits = (char *)lpDstStartBits + lDstLineBytes * (lDstHeight - 1 - i) + j;
			//计算该像素在源DIB中的坐标
			i1 = (long)(i / fVerRatio + 0.5);
			j1 = (long)(j / fhorRatio + 0.5);
			//判断是否在范围内
			if( ( j1 >= 0 ) && ( j1 < lWidth) && ( i1 >= 0 ) && ( i1 < lHeight ))
			{
				//指向源图像第i1行第j1个像素的指针
				lpSrcDIBBits = (char *)lpSrcStartBits + lLineBytes * (lHeight - 1 - i1) + j1;
				//复制像素
				*lpDstDIBBits = *lpSrcDIBBits;
			}
			else
			{
				//源图像中不存在的像素赋值为255
				*((unsigned char *)lpDstDIBBits) = 255;
			}

		}
	}

	return hDIB;

}

//利用双线性插值算法估算像素值
unsigned char Interpolation(
	LPSTR lpDIBBits,		//指向源DIB图像指针
	LONG lWidth,			//源图像宽度（像素数）
	LONG lHeight,			//源图像高度（像素数)
	FLOAT x,				//插值元素的x坐标
	FLOAT y					//插值元素的y坐标
	)
{
	//4个最邻近像素的坐标(i1,j1)，(i2,j1),(i1,j2),(i2,j2)
	LONG i1 = (LONG)x;
	LONG i2 = i1 + 1;
	LONG j1 = (LONG)y;
	LONG j2 = j1 + 1;
	//图像每行字节数
	long lLineBytes = WIDTHBYTES(lWidth * 8);
	//定义一个值，当像素坐标相差小于该值时，认为坐标相同
	float EXP = (FLOAT) 0.0001;
	//根据不同情况分别处理
	//如果计算的点不在源图像范围内，直接返回255
	if ((x < 0) || (x > lWidth - 1) || (y < 0) || ( y>lHeight - 1) )
	{
		return 255;
	}
	else
	{
		if (fabs(x - lWidth + 1) <= EXP)
		{
			//要计算的点在图像右边缘上
			if (fabs(y - lHeight + 1) <= EXP)
			{
				//要计算的点正好是图像最右下角那个像素，直接返回该点像素值
				unsigned char f1 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j1) + i1);
				return f1;
			}
			else
			{
				//在图像右边缘上且不是最后一点，直接一次插值即可.
				unsigned char f1 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j1) + i1);
				unsigned char f2 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j1) + i2);
				return ((unsigned char)(f1 + (y - j1) * (f2 - f1)));
			}

		}
		else if (fabs(y - lHeight + 1) <= EXP )
		{
			//在图像下边缘上且不是最后一点，直接一次插值即可.
			unsigned char f1 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j1) + i1);
			unsigned char f2 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j2) + i1);
			return ((unsigned char)(f1 + (x - i1) * (f2 - f1)));

		}
		else
		{
			//计算4个最邻近像素值
			unsigned char f1 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j1) + i1);
			unsigned char f2 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j1) + i2);
			unsigned char f3 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j2) + i1);
			unsigned char f4 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j2) + i2);
			//插值1
			unsigned char f12 = ((unsigned char)(f1 + (x - i1) * (f2 - f1)));
			//插值2
			unsigned char f34 = ((unsigned char)(f3 + (x - i1) * (f4 - f3)));
			//插值3
			return ((unsigned char)(f12 + (y - j1) * (f34 - f12)));
		}
	}
}
//双线性插值实现图像旋转
HGLOBAL Rotate_dib2(LPSTR lpSrcDib,		//指向源DIB的指针
	float fRotateAngle,		//旋转的角度
	LPSTR lpSrcStartBits, //指向源DIB起始像素的指针
	long lWidth,	//源DIB图像的宽度
	long lHeight,	//源DIB图像的高度	
	WORD palSize	//源DIB图像调色板大小
	)
{
	//找到源DIB图像像素起始位置
	LPSTR lpDIBBits = lpSrcStartBits;
	//旋转角度的正弦和余弦
	float fSina = (float)sin((double)fRotateAngle);
	float fCosa = (float)cos((double)fRotateAngle);
	//计算图像四个角的坐标（以图像中心为坐标系原点)
	float fSrcX1 = (float)(-(lWidth - 1) / 2);
	float fSrcY1 = (float)((lHeight - 1) / 2);
	float fSrcX2 = (float)((lWidth - 1) / 2);
	float fSrcY2 = (float)((lHeight - 1) / 2);
	float fSrcX3 = (float)(-(lWidth - 1) / 2);
	float fSrcY3 = (float)(-(lHeight - 1) / 2);
	float fSrcX4 = (float)((lWidth - 1) / 2);
	float fSrcY4 = (float)(-(lHeight - 1) / 2);
	//计算新图像4个角度的坐标（以图像中心为坐标系原点）
	float fDstX1 = fCosa * fSrcX1 + fSina * fSrcY1;
	float fDstY1 = -fSina * fSrcX1 + fCosa * fSrcY1;
	float fDstX2 = fCosa * fSrcX2 + fSina * fSrcY2;
	float fDstY2 = -fSina * fSrcX2 + fCosa * fSrcY2;
	float fDstX3 = fCosa * fSrcX3 + fSina * fSrcY3;
	float fDstY3 = -fSina * fSrcX3 + fCosa * fSrcY3;
	float fDstX4 = fCosa * fSrcX4 + fSina * fSrcY4;
	float fDstY4 = -fSina * fSrcX4 + fCosa * fSrcY4;
	//计算旋转后的图像实际宽度
	LONG lNewWidth = (LONG)(max(abs(fDstX4 - fDstX1), fabs(fDstX3 - fDstX2)) + 0.5);
	LONG lNewLineBytes = WIDTHBYTES(lNewWidth * 8);
	//计算旋转后的图像实际高度
	LONG lNewHeight = (LONG)(max(abs(fDstY4 - fDstY1), fabs(fDstY3 - fDstY2)) + 0.5);

	//浮点参数
	float f1 = (float)(-0.5 * (lNewWidth - 1) * fCosa - 0.5 * (lNewHeight - 1) * fSina + 0.5 * (lNewWidth - 1));
	float f2 = (float)(-0.5 * (lNewWidth - 1) * fSina - 0.5 * (lNewHeight - 1) * fCosa + 0.5 * (lNewHeight - 1));
	//分配内存，以保存新DIB
	HGLOBAL hDIB = (HGLOBAL) ::GlobalAlloc(GHND, lNewLineBytes * lNewHeight + *(LPWORD)lpSrcDib + palSize);
	if (hDIB == NULL)
	{
		//判断是否是有效的dib对象
		return FALSE;
	}
	//锁定内存
	LPSTR lpNewDIB = (char *) ::GlobalLock((HGLOBAL)hDIB);
	//复制dib信息头和调色板
	memcpy(lpNewDIB, lpSrcDib, *(LPDWORD)lpSrcDib + palSize);
	//找到新DIB像素起始位置
	LPSTR lpNewDIBBits = lpNewDIB + *(LPDWORD)lpSrcDib + palSize;
	//获取指针
	LPBITMAPINFOHEADER lpbmi = (LPBITMAPINFOHEADER)lpNewDIB;
	LPBITMAPCOREHEADER lpbmc = (LPBITMAPCOREHEADER)lpNewDIB;
	//更新DIB中图像的高度和宽度
	lpbmi->biHeight = lNewHeight;
	lpbmi->biWidth = lNewWidth;

	for (size_t i = 0; i < lNewHeight; i++)
	{
		for (size_t j = 0; j < lNewWidth; j++)
		{
			//指向新dib第i行第j个像素的指针
			LPSTR lpDst = (char *)lpNewDIBBits + lNewLineBytes * (lNewHeight - 1 - i) + j;
			//计算该像素在源DIB中的坐标
			long i0 = (long)(-((float)j) * fSina + ((float)i) * fCosa + f2 );
			long j0 = (long)(((float)j)  * fCosa + ((float)i) * fSina + f1 );
	
			//利用双线性插值算法来估算像素值
			*lpDst = Interpolation(lpDIBBits, lWidth, lHeight, j0, i0);
		
	
		}

	}

	return hDIB;
}
