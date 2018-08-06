
#pragma once
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

//双线性插值实现图像旋转
HGLOBAL Rotate(LPSTR lpSrcDib,		//指向源DIB的指针
	float fRotateAngle,		//旋转的角度
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
	if (hDIB == NULL)
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
