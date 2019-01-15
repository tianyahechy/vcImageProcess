#include "stdafx.h"
#include "Dib.h"
#include <atlbase.h>
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//DIB文件头标志(字符串"BM"),用于判断位图的标志宏
#define DIB_MAKER	((WORD) ( 'M' << 8) | 'B')

CDib::CDib()
{
	
	this->InitMembers();
	
}

//初始化成员变量
void CDib::InitMembers()
{
	m_hDib = NULL;
	lpbmihrd = NULL;
	lpdib = NULL;
	lpDIBBits = NULL;
	dwDIBSize = 0;
	lpRgbQuad = NULL;
}

void CDib::clearMemory()
{
	if( m_hDib != NULL)
		::GlobalFree(m_hDib);
	lpbmihrd = NULL;
	lpdib = NULL;
	lpDIBBits = NULL;
	dwDIBSize = 0;
	lpRgbQuad = NULL;
}

CDib::~CDib()
{
	//清理空间
	this->clearMemory();
}
//返回DIB中调色板的大小
WORD CDib::getPalSize(LPSTR lpdib)
{
	return (WORD)(getColorNum(lpdib) * sizeof(RGBQUAD));
}
//计算DIB像素的起始位置，并返回指向它的指针
LPSTR CDib::GetBits(LPSTR)
{
	return (lpdib + ((LPBITMAPINFOHEADER)lpdib)->biSize + this->getPalSize(lpdib));
}
//从文件加载位图
HGLOBAL CDib::loadFile(CFile& file)
{
	//获取文件大小
	DWORD dwFileSize = file.GetLength();
	//读取位图文件头
	int nCount = file.Read((LPSTR)&bmfHeader, sizeof(bmfHeader));
	if (nCount != sizeof(bmfHeader))
	{
		return NULL;
	}

	//判断此文件是否位图，0x4d42代表bmP
	if (bmfHeader.bfType != DIB_MAKER)
	{
		return NULL;
	}

	//分配DIB内存
	m_hDib = (HGLOBAL) ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, dwFileSize - sizeof(BITMAPFILEHEADER));
	//分配失败，返回NULL
	if ( m_hDib == 0 )
	{
		return NULL;
	}

	//给CDib类的成员变量赋值
	//锁定
	lpdib = (LPSTR) ::GlobalLock((HGLOBAL)m_hDib);
	lpbminfo = (BITMAPINFO*)lpdib;
	lpbmihrd = (BITMAPINFOHEADER*)lpdib;
	lpRgbQuad = (RGBQUAD*)(lpdib + lpbmihrd->biSize);
	int m_numberOfColors = this->getColorNum((LPSTR)lpbmihrd);
	if ( lpbmihrd->biClrUsed == 0)
	{
		lpbmihrd->biClrUsed = m_numberOfColors;
	}

	//计算颜色表长度
	DWORD colorTableSize = m_numberOfColors * sizeof(RGBQUAD);
	//数组指针
	lpDIBBits = lpdib + lpbmihrd->biSize + colorTableSize;
	//大小不一致
	if (file.Read(lpdib, dwFileSize - sizeof(BITMAPFILEHEADER)) != dwFileSize - sizeof(BITMAPFILEHEADER))
	{
		//解除锁定
		::GlobalUnlock((HGLOBAL)m_hDib);
		//释放内存
		::GlobalFree((HGLOBAL)m_hDib);
		return NULL;
	}
	//解除锁定
	::GlobalUnlock((HGLOBAL)m_hDib);
	//返回DIB句柄
	return m_hDib;
}

//=======================================================
// 函数功能： 获取位图数据
// 输入参数： 无
// 返回值：   LPBYTE 位图数据指针
//=======================================================
LPBYTE CDib::GetData()
{
	return (LPBYTE)lpDIBBits;
}
//将位图保存到文件
BOOL CDib::saveFile(HGLOBAL hDib, CFile& file)
{
	//如果DIB为空，返回FALSE
	if ( hDib == NULL )
	{
		return FALSE;
	}

	//读取BITMAPINFO结构，并锁定
	lpbmihrd = (LPBITMAPINFOHEADER) ::GlobalLock((HGLOBAL)hDib);
	if ( lpbmihrd == NULL)
	{
		return FALSE;
	}
	//填充文件头
	bmfHeader.bfType = DIB_MAKER;
	//文件头大小+颜色表大小
	dwDIBSize = *(LPDWORD)lpbmihrd + getPalSize((LPSTR)lpbmihrd);
	//像素的大小,大小为width * height
	DWORD dwBmBitsSize = this->getReqByteWidth((lpbmihrd->biWidth) * ((DWORD)lpbmihrd->biBitCount)) *lpbmihrd->biHeight;
	//计算后DIB每行字节数为4的倍数时的大小
	dwDIBSize += dwBmBitsSize;
	//更新biSizeImage
	lpbmihrd->biSizeImage = dwBmBitsSize;
	//文件大小
	bmfHeader.bfSize = dwDIBSize + sizeof(BITMAPFILEHEADER);
	bmfHeader.bfReserved1 = 0;
	bmfHeader.bfReserved2 = 0;
	//计算偏移量
	bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + lpbmihrd->biSize + getPalSize((LPSTR)lpbmihrd);
	TRY
	{
		//写文件头
		file.Write(&bmfHeader, sizeof(BITMAPFILEHEADER));
		//写DIB头和像素
		file.Write(lpbmihrd, dwDIBSize);
	}
	CATCH(CFileException, e)
	{
		//解除锁定
		::GlobalUnlock((HGLOBAL)hDib);
		//抛出异常
		THROW_LAST();
	}
	END_CATCH
		//解除锁定
		::GlobalUnlock((HGLOBAL)hDib);
	return TRUE;

}   

//获取位图高度
DWORD CDib::getHeight(LPSTR lpdib)
{
	return ((LPBITMAPINFOHEADER)lpdib)->biHeight;
}

//获得位图的宽度
DWORD CDib::getWidth(LPSTR lpdib)
{
	return ((LPBITMAPINFOHEADER)lpdib)->biWidth;
}

//获取单个像素所占位数
WORD CDib::getBitCount()
{
	return ((LPBITMAPINFOHEADER)lpdib)->biBitCount;
}

//获取需要的行字节数，应为4的倍数
int CDib::getReqByteWidth(int bits)
{
	int getBytes = (bits + 31) / 32 * 4;
	return getBytes;
}

long CDib::getRectWidth(LPRECT lpRect)
{
	long nWidth = lpRect->right - lpRect->left;
	return nWidth;
}

long CDib::getRectHeight(LPRECT lpRect)
{
	long nHeight = lpRect->bottom - lpRect->top;
	return nHeight;
}

//获取位图颜色数
WORD CDib::getColorNum(LPSTR lpdib)
{
	long dwClrUsed = ((LPBITMAPINFOHEADER)lpdib)->biClrUsed;
	if (dwClrUsed != 0)
	{
		return (WORD)dwClrUsed;
	}
	//读取biBitCount值
	WORD wBitCount = ((LPBITMAPINFOHEADER)lpdib)->biBitCount;
	
	switch (wBitCount)
	{
	case 1:
		return 2;	

	case 4:
		return 16;

	case 8:
		return 256;

	default:
		return 0;
	}

}

//根据颜色表生成调色板
BOOL CDib::ConstructPalette( HGLOBAL hDIB,		//DIB对象的句柄
	CPalette * pPal								//调色板指针
	)
{
	//创建结果
	BOOL bSuccess = FALSE;
	//判断是否是有效的DIB对象
	if (hDIB == NULL)
	{
		return FALSE;
	}
	//锁定DIB
	lpdib = (LPSTR) ::GlobalLock((HGLOBAL)hDIB);
	lpbminfo = (LPBITMAPINFO)lpdib;
	//获取DIB中颜色表的颜色数目
	long wNumColors = this->getColorNum(lpdib);
	//如果颜色表长度为0，则不生成逻辑调色板
	if (wNumColors == 0)
	{
		return FALSE;
	}
	
	//分配为逻辑调色板内存
	//逻辑调色板句柄
	HANDLE hLogPal = ::GlobalAlloc(GHND, sizeof(LOGPALETTE) + sizeof(PALETTEENTRY) * wNumColors);
	//如果失败则退出
	if ( hLogPal == 0 )
	{
		//解除锁定
		::GlobalUnlock((HGLOBAL)hDIB);
		return FALSE;
	}
	LPLOGPALETTE lpLogPalette = (LPLOGPALETTE) ::GlobalLock((HGLOBAL)hLogPal);
	
	//生成逻辑调色板
	//设置调色板版本号
	lpLogPalette->palVersion = PalVersion;
	//设置颜色数目
	lpLogPalette->palNumEntries = (WORD)wNumColors;
	//读取调色板
	for (size_t i = 0; i < (int)wNumColors; i++)
	{
		lpLogPalette->palPalEntry[i].peRed = lpbminfo->bmiColors[i].rgbRed;
		lpLogPalette->palPalEntry[i].peGreen = lpbminfo->bmiColors[i].rgbGreen;
		lpLogPalette->palPalEntry[i].peBlue = lpbminfo->bmiColors[i].rgbBlue;
		lpLogPalette->palPalEntry[i].peFlags = 0; //保留位
		
	}
	//创建逻辑调色板
	bSuccess = pPal->CreatePalette(lpLogPalette);

	//解除锁定
	::GlobalUnlock((HGLOBAL)hLogPal);
	//释放逻辑调色板
	::GlobalFree((HGLOBAL)hLogPal);	
	//解除锁定
	::GlobalUnlock((HGLOBAL)hDIB);
	return bSuccess;
}

//显示位图
BOOL CDib::DrawDib( HDC		hDC,			//输出设备DC
					LPRECT	lpDCRect,		//绘制矩形区域
					HGLOBAL	hDIB,			//DIB对象的句柄
					LPRECT	lpDIBRect,		//DIB的输出区域
					CPalette* pPal			//调色板指针
	)
{
	//重画成功标志
	BOOL	bSuccess = FALSE;
	//旧的调色板句柄
	HPALETTE hOldPalette = NULL;
	//判断是否是有效的DIB对象,不是则返回
	if ( hDIB == NULL)
	{
		return FALSE;
	}
	//锁定DIB
	lpdib = (LPSTR) ::GlobalLock((HGLOBAL)hDIB);
	//找到DIB图像像素起始位置
	lpDIBBits = GetBits(lpdib);
	//如果位图有调色板,则选进设备环境中
	if (pPal != NULL)
	{
		HPALETTE hPal = (HPALETTE)pPal->m_hObject;
		hOldPalette = SelectPalette(hDC, hPal, TRUE);
	}
	//设置显示模式
	::SetStretchBltMode(hDC,COLORONCOLOR);

	//将位图在pDC所指向的设备上进行显示
	bSuccess = ::StretchDIBits(hDC,			//设备环境句柄
		lpDCRect->left,						//目标X坐标
		lpDCRect->top,						//目标Y坐标
		this->getRectWidth(lpDCRect),		//目标宽度
		this->getRectHeight(lpDCRect),		//目标高度
		lpDIBRect->left,					//源X坐标 
		lpDIBRect->top,						//源Y坐标
		this->getRectWidth(lpDIBRect),		//源宽度
		this->getRectHeight(lpDIBRect),		//源高度
		lpDIBBits,							//指向dib像素的指针
		(LPBITMAPINFO) lpdib,				//指向位图信息结构的指针
		DIB_RGB_COLORS,						//使用的颜色数目
		SRCCOPY);
	::GlobalUnlock(hDIB);					//解除锁定
	//恢复系统调色板
	if ( hOldPalette != NULL )
	{
		::SelectPalette(hDC, hOldPalette, TRUE);
	}
	return bSuccess;
}

//复制指定的内存区域
HGLOBAL CDib::copyObject(HGLOBAL hGlob)
{
	if ( hGlob == NULL)
	{
		return NULL;
	}

	//获取指定内存区域大小
	DWORD dwLen = ::GlobalSize((HGLOBAL)hGlob);
	//分配新内存空间
	HGLOBAL hTemp = ::GlobalAlloc(GHND, dwLen);
	//判断分配是否成功
	if ( hTemp != NULL )
	{
		//锁定
		void * lpTemp = ::GlobalLock((HGLOBAL)hTemp);
		void * lp = ::GlobalLock((HGLOBAL)hGlob);
		//复制
		memcpy(lpTemp, lp, dwLen);
		//解除锁定
		::GlobalUnlock(hTemp);
		::GlobalUnlock(hGlob);
	}
	return hTemp;
}