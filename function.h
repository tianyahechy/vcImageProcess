
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
