#pragma once
#include "afx.h"

//调色板版本
#define PalVersion	0x300

class CDib :
	public CObject
{
public:
	CDib();
	virtual ~CDib();
	BOOL DrawDib(HDC, LPRECT, HGLOBAL, LPRECT, CPalette*);	//显示位图
	BOOL ConstructPalette(HGLOBAL, CPalette*);				//构造逻辑调色板
	LPSTR GetBits(LPSTR);									//获取位图数据的入口地址
	DWORD getWidth(LPSTR);									//获得位图宽度
	DWORD getHeight(LPSTR);									//获得位图高度
	WORD  getPalSize(LPSTR);								//取得调色板的大小
	WORD  getColorNum(LPSTR);								//获取位图颜色数
	WORD  getBitCount();									//获取单个像素所占位数
	HGLOBAL copyObject(HGLOBAL);							//用于复制位图对象
	BOOL saveFile(HGLOBAL, CFile&);							//将位图保存到文件
	HGLOBAL loadFile(CFile& file);							//从文件加载位图
	//转换位图字节为4的倍数以进行图像处理
	int getReqByteWidth(int);
	long  getRectWidth(LPRECT);								//获取区域的宽度
	long  getRectHeight(LPRECT);							//获取区域的高度
    // 获取位图数据
	LPBYTE GetData();
public:
	void clearMemory();
	void InitMembers();

public:

private:
	LPBITMAPINFO		lpbminfo;			//位图信息指针
	LPBITMAPINFOHEADER	lpbmihrd;			//位图信息头指针
	BITMAPFILEHEADER	bmfHeader;			//位图文件头指针，需动态分配和释放
	LPSTR				lpdib;				//位图指针，包含除位图文件头的所有内容，需动态分配和释放
	LPSTR				lpDIBBits;			//位图数据指针
	DWORD				dwDIBSize;			//DIB大小
	HGLOBAL				m_hDib;				//DIB对象的句柄
	RGBQUAD	*			lpRgbQuad;			//位图颜色表指针
	
};

