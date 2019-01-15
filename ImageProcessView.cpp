
// ImageProcessView.cpp : CImageProcessView 类的实现
//

#include "stdafx.h"
#include <gdiplusinit.h>
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "ImageProcess.h"
#endif
#include "function.h"
#include "DlgTran.h"
#include "DlgRot.h"
#include "DlgZoom.h"
//#include "Dlggeo.h"

#include "ImageProcessDoc.h"
#include "ImageProcessView.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CImageProcessView * pView;
// CImageProcessView

IMPLEMENT_DYNCREATE(CImageProcessView, CView)

BEGIN_MESSAGE_MAP(CImageProcessView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CImageProcessView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_Translation, &CImageProcessView::OnTranslation)
	ON_COMMAND(ID_RobertsEdgeDetection, &CImageProcessView::OnRobertsedgedetection)
	ON_COMMAND(ID_Rotation, &CImageProcessView::OnRotation)
	ON_COMMAND(ID_Mirror, &CImageProcessView::OnMirror)
	ON_COMMAND(ID_Mirror2, &CImageProcessView::OnMirror2)
	ON_COMMAND(ID_Transpose, &CImageProcessView::OnTranspose)
	ON_COMMAND(ID_32785, &CImageProcessView::OnZoom)
	ON_COMMAND(ID_GeomRota, &CImageProcessView::OnGeomrota)
	ON_COMMAND(ID_grayTransform_Linear, &CImageProcessView::OngraytransformLinear)
	ON_COMMAND(ID_OPEN2, &CImageProcessView::OnOpen2)
END_MESSAGE_MAP()

// CImageProcessView 构造/析构

CImageProcessView::CImageProcessView()
{
	// TODO:  在此处添加构造代码
	m_pDbImage = NULL;
	m_nDWTCurDepth = 0;
	_bLoadImage = false;
}

CImageProcessView::~CImageProcessView()
{
}

BOOL CImageProcessView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CImageProcessView 绘制

void CImageProcessView::OnDraw(CDC* pDC)
{
	CImageProcessDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO:  在此处为本机数据添加绘制代码
	HGLOBAL hDIB = pDoc->GetHObject();
	//判断DIB是否为空
	if ( hDIB != NULL )
	{
		LPSTR lpDibSection = (LPSTR) ::GlobalLock((HGLOBAL)hDIB);
		//获取DIB宽度
		int cxDIB = (int)pDoc->m_dib.getWidth(lpDibSection);
		//获取DIB高度
		int cyDIB = (int)pDoc->m_dib.getHeight(lpDibSection);
		::GlobalUnlock((HGLOBAL)hDIB);

		CRect rcDIB;
		rcDIB.top = rcDIB.left = 0;
		rcDIB.right = cxDIB;
		rcDIB.bottom = cyDIB;
		CRect rcDest = rcDIB;
		//输出DIB，
		pDoc->m_dib.DrawDib(pDC->m_hDC, &rcDest, pDoc->GetHObject(), &rcDIB, pDoc->GetDocPal());
	}

}


// CImageProcessView 打印


void CImageProcessView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CImageProcessView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CImageProcessView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加额外的打印前进行的初始化过程
}

void CImageProcessView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加打印后进行的清理过程
}

void CImageProcessView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CImageProcessView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CImageProcessView 诊断

#ifdef _DEBUG
void CImageProcessView::AssertValid() const
{
	CView::AssertValid();
}

void CImageProcessView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CImageProcessDoc* CImageProcessView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CImageProcessDoc)));
	return (CImageProcessDoc*)m_pDocument;
}
#endif //_DEBUG


// CImageProcessView 消息处理程序


void CImageProcessView::OnTranslation()
{
	// TODO:  在此添加命令处理程序代码
	CImageProcessDoc * pDoc = GetDocument();
	//锁定DIB,指向源图像的指针
	LPSTR lpSrcDib = (LPSTR) ::GlobalLock((HGLOBAL)pDoc->GetHObject());
	//判断是否是8位位图，不是则返回
	if ( pDoc->m_dib.getColorNum(lpSrcDib) != 256 )
	{
		AfxMessageBox(_T("对不起，不是256位图"));
		::GlobalUnlock((HGLOBAL)pDoc->GetHObject());
		return;
	}
	//找到DIB图像像素起始位置的指针
	LPSTR lpSrcStartBits = pDoc->m_dib.GetBits(lpSrcDib);
	//获得图像的宽度
	long lSrcWidth = pDoc->m_dib.getWidth(lpSrcDib);
	//获得图像的高度
	long lSrcHeight = pDoc->m_dib.getHeight(lpSrcDib);
	//计算图像每行的字节数
	long lSrcLineBytes = pDoc->m_dib.getReqByteWidth(lSrcWidth * 8);
	long lDstLineBytes = pDoc->m_dib.getReqByteWidth(lSrcWidth * 8);
	//计算新图像每行的字节数
	CDlgTran tranPara;
	if (tranPara.DoModal() != IDOK)
	{
		return;
	}

	int temVer = tranPara.m_verOff;
	int temHor = tranPara.m_horOff;
	//平移
	if (Translation(lpSrcStartBits, lSrcWidth,lSrcHeight,temVer,temHor,lSrcLineBytes, lDstLineBytes))
	{
		//设置标记
		pDoc->SetModifiedFlag(TRUE);
		//更新视图
		pDoc->UpdateAllViews(NULL);
		//解除锁定
		::GlobalUnlock((HGLOBAL)pDoc->GetHObject());
	}
	else
	{
		AfxMessageBox(_T("分配内存失败!"));
	}
}

void CImageProcessView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	pView = this;
}

void CImageProcessView::OnRobertsedgedetection()
{/*
	// TODO:  在此添加命令处理程序代码
	CImageProcessDoc * pDoc = GetDocument();
	//锁定DIB,指向源图像的指针
	LPSTR lpSrcDib = (LPSTR) ::GlobalLock((HGLOBAL)pDoc->GetHObject());
	LPBYTE lpData = pDoc->m_dib.getBitCount
	//判断是否是8位位图，不是则返回
	if (pDoc->m_dib.getColorNum(lpSrcDib) != 256)
	{
		AfxMessageBox(_T("对不起，不是256位图"));
		::GlobalUnlock((HGLOBAL)pDoc->GetHObject());
		return;
	}
	//找到DIB图像像素起始位置的指针
	LPSTR lpSrcStartBits = pDoc->m_dib.GetBits(lpSrcDib);
	//获得图像的宽度
	long nWidth = pDoc->m_dib.getWidth(lpSrcDib);
	//获得图像的高度
	long nHeight = pDoc->m_dib.getHeight(lpSrcDib);
	//计算图像每行的字节数
	long lSrcLineBytes = pDoc->m_dib.getReqByteWidth(lSrcWidth * 8);
	long lDstLineBytes = pDoc->m_dib.getReqByteWidth(lSrcWidth * 8);
	//计算新图像每行的字节数
	CDlgTran tranPara;
	if (tranPara.DoModal() != IDOK)
	{
		return;
	}

	int temVer = tranPara.m_verOff;
	int temHor = tranPara.m_horOff;
	//平移
	if (Translation(lpSrcStartBits, lSrcWidth, lSrcHeight, temVer, temHor, lSrcLineBytes, lDstLineBytes))
	{
		//设置标记
		pDoc->SetModifiedFlag(TRUE);
		//更新视图
		pDoc->UpdateAllViews(NULL);
		//解除锁定
		::GlobalUnlock((HGLOBAL)pDoc->GetHObject());
	}
	else
	{
		AfxMessageBox(_T("分配内存失败!"));
	}
	*/

}


void CImageProcessView::OnRotation()
{
	// TODO:  在此添加命令处理程序代码// TODO:  在此添加命令处理程序代码
	CImageProcessDoc * pDoc = GetDocument();
	//锁定DIB,指向源图像的指针
	LPSTR lpSrcDib = (LPSTR) ::GlobalLock((HGLOBAL)pDoc->GetHObject());
	//判断是否是8位位图，不是则返回
	if (pDoc->m_dib.getColorNum(lpSrcDib) != 256)
	{
		AfxMessageBox(_T("对不起，不是256位图"));
		::GlobalUnlock((HGLOBAL)pDoc->GetHObject());
		return;
	}
	//找到DIB图像像素起始位置的指针
	LPSTR lpSrcStartBits = pDoc->m_dib.GetBits(lpSrcDib);
	//获得图像的宽度
	long lSrcWidth = pDoc->m_dib.getWidth(lpSrcDib);
	//获得图像的高度
	long lSrcHeight = pDoc->m_dib.getHeight(lpSrcDib);
	//计算图像每行的字节数
	long lSrcLineBytes = pDoc->m_dib.getReqByteWidth(lSrcWidth * 8);

	//计算新图像每行的字节数
	CDlgRot rotPara;
	if (rotPara.DoModal() != IDOK)
	{
		return;
	}
	DWORD palSize = pDoc->m_dib.getPalSize(lpSrcDib);
	//将旋转角度从度转换为弧度
	float fRotationAngle = (float)AngleToRadian(rotPara.m_rotAngle);
	//计算旋转角度的正余弦
	float fSina = (float)sin((double)fRotationAngle);
	float fCosa = (float)cos((double)fRotationAngle);

	//计算旋转前4个角的坐标（以图像中心为坐标系原点）
	float fSrcX1 = (float)(-(lSrcWidth - 1) / 2);
	float fSrcY1 = (float)((lSrcHeight - 1) / 2);
	float fSrcX2 = (float)((lSrcWidth - 1) / 2);
	float fSrcY2 = (float)((lSrcHeight - 1) / 2);
	float fSrcX3 = (float)(-(lSrcWidth - 1) / 2);
	float fSrcY3 = (float)(-(lSrcHeight - 1) / 2);
	float fSrcX4 = (float)((lSrcWidth - 1) / 2);
	float fSrcY4 = (float)(-(lSrcHeight - 1) / 2);

	//计算新图4个角的坐标
	float fDstX1 = fCosa * fSrcX1 + fSina * fSrcY1;
	float fDstY1 = -fSina * fSrcX1 + fCosa * fSrcY1;
	float fDstX2 = fCosa * fSrcX2 + fSina * fSrcY2;
	float fDstY2 = -fSina * fSrcX2 + fCosa * fSrcY2;
	float fDstX3 = fCosa * fSrcX3 + fSina * fSrcY3;
	float fDstY3 = -fSina * fSrcX3 + fCosa * fSrcY3;
	float fDstX4 = fCosa * fSrcX4 + fSina * fSrcY4;
	float fDstY4 = -fSina * fSrcX4 + fCosa * fSrcY4;

	//计算旋转后的图像实际宽度
	long lDstWidth = (long)(max(fabs(fDstX4 - fDstX1), fabs(fDstX3 - fDstX2)) + 0.5);
	//计算新图像每行的字节数
	long lDstLineBytes = pDoc->m_dib.getReqByteWidth(lDstWidth * 8);
	//计算旋转后的图像高度
	long lDstHeight = (long)(max(fabs(fDstY4 - fDstY1), fabs(fDstY3 - fDstY2)) + 0.5);
	//创建新dib,旋转
	HGLOBAL hDstDIB = (HGLOBAL)Rotate(lpSrcDib,lpSrcStartBits, lSrcWidth, lSrcHeight, 
		lSrcLineBytes, palSize, lDstWidth, lDstHeight, lDstLineBytes, fSina, fCosa);
	if (hDstDIB != NULL)
	{
		//替换dib，同时释放旧dib对象
		pDoc->updateObject(hDstDIB);
		//更新dib大小和调色板
		pDoc->setDIB();
		//设置修改标记
		pDoc->SetModifiedFlag(TRUE);
		//更新视图
		pDoc->UpdateAllViews(NULL);
		//解除锁定
		::GlobalUnlock((HGLOBAL)pDoc->GetHObject());
	}
	else
	{
		AfxMessageBox(_T("分配内存失败!"));
	}
}


void CImageProcessView::OnMirror()
{
	// TODO:  在此添加命令处理程序代码
	CImageProcessDoc * pDoc = GetDocument();
	//锁定DIB,指向源图像的指针
	LPSTR lpSrcDib = (LPSTR) ::GlobalLock((HGLOBAL)pDoc->GetHObject());
	//判断是否是8位位图，不是则返回
	if (pDoc->m_dib.getColorNum(lpSrcDib) != 256)
	{
		AfxMessageBox(_T("对不起，不是256位图"));
		::GlobalUnlock((HGLOBAL)pDoc->GetHObject());
		return;
	}
	//找到DIB图像像素起始位置的指针
	LPSTR lpSrcStartBits = pDoc->m_dib.GetBits(lpSrcDib);
	//获得图像的宽度
	long lSrcWidth = pDoc->m_dib.getWidth(lpSrcDib);
	//获得图像的高度
	long lSrcHeight = pDoc->m_dib.getHeight(lpSrcDib);
	//计算图像每行的字节数
	long lSrcLineBytes = pDoc->m_dib.getReqByteWidth(lSrcWidth * 8);

	//平移
	if (Mirror(lpSrcStartBits, lSrcWidth, lSrcHeight, lSrcLineBytes))
	{
		//设置标记
		pDoc->SetModifiedFlag(TRUE);
		//更新视图
		pDoc->UpdateAllViews(NULL);
		//解除锁定
		::GlobalUnlock((HGLOBAL)pDoc->GetHObject());
	}
	else
	{
		AfxMessageBox(_T("分配内存失败!"));
	}
}


void CImageProcessView::OnMirror2()
{
	// TODO:  在此添加命令处理程序代码
	CImageProcessDoc * pDoc = GetDocument();
	//锁定DIB,指向源图像的指针
	LPSTR lpSrcDib = (LPSTR) ::GlobalLock((HGLOBAL)pDoc->GetHObject());
	//判断是否是8位位图，不是则返回
	if (pDoc->m_dib.getColorNum(lpSrcDib) != 256)
	{
		AfxMessageBox(_T("对不起，不是256位图"));
		::GlobalUnlock((HGLOBAL)pDoc->GetHObject());
		return;
	}
	//找到DIB图像像素起始位置的指针
	LPSTR lpSrcStartBits = pDoc->m_dib.GetBits(lpSrcDib);
	//获得图像的宽度
	long lSrcWidth = pDoc->m_dib.getWidth(lpSrcDib);
	//获得图像的高度
	long lSrcHeight = pDoc->m_dib.getHeight(lpSrcDib);
	//计算图像每行的字节数
	long lSrcLineBytes = pDoc->m_dib.getReqByteWidth(lSrcWidth * 8);

	//平移
	if (Mirror2(lpSrcStartBits, lSrcWidth, lSrcHeight, lSrcLineBytes))
	{
		//设置标记
		pDoc->SetModifiedFlag(TRUE);
		//更新视图
		pDoc->UpdateAllViews(NULL);
		//解除锁定
		::GlobalUnlock((HGLOBAL)pDoc->GetHObject());
	}
	else
	{
		AfxMessageBox(_T("分配内存失败!"));
	}
}


void CImageProcessView::OnTranspose()
{
	// TODO:  在此添加命令处理程序代码
	CImageProcessDoc * pDoc = GetDocument();
	//锁定DIB,指向源图像的指针
	LPSTR lpSrcDib = (LPSTR) ::GlobalLock((HGLOBAL)pDoc->GetHObject());
	//判断是否是8位位图，不是则返回
	if (pDoc->m_dib.getColorNum(lpSrcDib) != 256)
	{
		AfxMessageBox(_T("对不起，不是256位图"));
		::GlobalUnlock((HGLOBAL)pDoc->GetHObject());
		return;
	}
	//找到DIB图像像素起始位置的指针
	LPSTR lpSrcStartBits = pDoc->m_dib.GetBits(lpSrcDib);
	//获得图像的宽度
	long lSrcWidth = pDoc->m_dib.getWidth(lpSrcDib);
	//获得图像的高度
	long lSrcHeight = pDoc->m_dib.getHeight(lpSrcDib);
	//计算图像每行的字节数
	long lSrcLineBytes = pDoc->m_dib.getReqByteWidth(lSrcWidth * 8);
	//计算新图像每行的字节数
	long lDstLineBytes = pDoc->m_dib.getReqByteWidth(lSrcHeight * 8);

	//转置
	if (Transpose(lpSrcDib,lpSrcStartBits, lSrcWidth, lSrcHeight, lSrcLineBytes,lDstLineBytes))
	{
		//更新dib图像大小和调色板
		pDoc->setDIB();
		//设置标记
		pDoc->SetModifiedFlag(TRUE);
		//更新视图
		pDoc->UpdateAllViews(NULL);
		//解除锁定
		::GlobalUnlock((HGLOBAL)pDoc->GetHObject());
	}
	else
	{
		AfxMessageBox(_T("分配内存失败!"));
	}
}

void CImageProcessView::OnZoom()
{
	// TODO:  在此添加命令处理程序代码
	CImageProcessDoc * pDoc = GetDocument();
	//锁定DIB,指向源图像的指针
	LPSTR lpSrcDib = (LPSTR) ::GlobalLock((HGLOBAL)pDoc->GetHObject());
	//判断是否是8位位图，不是则返回
	if (pDoc->m_dib.getColorNum(lpSrcDib) != 256)
	{
		AfxMessageBox(_T("对不起，不是256位图"));
		::GlobalUnlock((HGLOBAL)pDoc->GetHObject());
		return;
	}
	//找到DIB图像像素起始位置的指针
	LPSTR lpSrcStartBits = pDoc->m_dib.GetBits(lpSrcDib);
	//获得图像的宽度
	long lSrcWidth = pDoc->m_dib.getWidth(lpSrcDib);
	//获得图像的高度
	long lSrcHeight = pDoc->m_dib.getHeight(lpSrcDib);
	//计算图像每行的字节数
	long lSrcLineBytes = pDoc->m_dib.getReqByteWidth(lSrcWidth * 8);
	DWORD palSize = pDoc->m_dib.getPalSize(lpSrcDib);
	//计算新图像每行的字节数
	CDlgZoom zoomPara;
	if (zoomPara.DoModal() != IDOK)
	{
		return;
	}

	float fX = zoomPara.m_horZoom;
	float fY = zoomPara.m_verZoom;

	//计算缩放后的图像实际宽度，
	long lDstWidth = (long)(lSrcWidth * fX + 0.5);
	//转换后图像应有的字节数，为4的倍数
	long lDstLineBytes = pDoc->m_dib.getReqByteWidth(lDstWidth * 8);
	//计算缩放后的图像高度
	long lDstHeight = (long)(lSrcHeight * fY + 0.5);
	//创建新DIB
	HGLOBAL hDstDIB = (HGLOBAL)Zoom(lpSrcDib, lpSrcStartBits, lSrcWidth, lSrcHeight, lSrcLineBytes, 
		palSize, lDstWidth, lDstHeight, lDstLineBytes, fX, fY);
	//平移
	if (hDstDIB != NULL)
	{
		//替换DIB,同时释放旧DIB
		pDoc->updateObject(hDstDIB);
		//更新DIB大小和调色板
		pDoc->setDIB();
		//设置标记
		pDoc->SetModifiedFlag(TRUE);
		//更新视图
		pDoc->UpdateAllViews(NULL);
		//解除锁定
		::GlobalUnlock((HGLOBAL)pDoc->GetHObject());
	}
	else
	{
		AfxMessageBox(_T("分配内存失败!"));
	}
}


void CImageProcessView::OnGeomrota()
{/*
	// TODO:  在此添加命令处理程序代码
	CImageProcessDoc * pDoc = GetDocument();
	//锁定DIB,指向源图像的指针
	LPSTR lpSrcDib = (LPSTR) ::GlobalLock((HGLOBAL)pDoc->GetHObject());
	//判断是否是8位位图，不是则返回
	if (pDoc->m_dib.getColorNum(lpSrcDib) != 256)
	{
		AfxMessageBox(_T("对不起，不是256位图"));
		::GlobalUnlock((HGLOBAL)pDoc->GetHObject());
		return;
	}
	//找到DIB图像像素起始位置的指针
	LPSTR lpSrcStartBits = pDoc->m_dib.GetBits(lpSrcDib);
	//获得图像的宽度
	long lSrcWidth = pDoc->m_dib.getWidth(lpSrcDib);
	//获得图像的高度
	long lSrcHeight = pDoc->m_dib.getHeight(lpSrcDib);
	//计算图像每行的字节数
	long lSrcLineBytes = pDoc->m_dib.getReqByteWidth(lSrcWidth * 8);
	
	CDlggeo RotPara;
	if (RotPara.DoModal() != IDOK)
	{
		return;
	}

	float fRotationAngle = (float)AngleToRadian(RotPara.m_rotAngle);
	delete RotPara;

	DWORD palSize = pDoc->m_dib.getPalSize(lpSrcDib); 
	BeginWaitCursor();
	HGLOBAL hNewDIB = (HGLOBAL)Rotate_dib2(lpSrcDib, fRotationAngle, lpSrcStartBits, lSrcWidth, lSrcHeight, palSize);
	//平移
	if (hNewDIB != NULL)
	{
		pDoc->updateObject(hNewDIB);
		pDoc->setDIB();
		//设置标记
		pDoc->SetModifiedFlag(TRUE);
		//更新视图
		pDoc->UpdateAllViews(NULL);
	}
	else
	{
		AfxMessageBox(_T("分配内存失败!"));
	}
	//解除锁定
	::GlobalUnlock((HGLOBAL)pDoc->GetHObject());
	EndWaitCursor();
	*/
}


void CImageProcessView::OngraytransformLinear()
{
	if ( !_bLoadImage )
	{
		AfxMessageBox("no image");
		return;
	}
	//找到源图像的起始位置
	LPBYTE lpDIBBits = theDIB.GetData();
	//获得图像的宽度
	long lWidth = theDIB.GetWidth();
	//获得图像的高度
	long lHeight = theDIB.GetHeight();
	//暂时分配内存给新图像
	HLOCAL hNewDIBBits = LocalAlloc(LHND, lWidth * lHeight);
	if (hNewDIBBits == NULL)
	{
		return;
	}
	//指向缓存IDB图像的指针
	LPBYTE lpNewDIBBits = (LPBYTE)LocalLock(hNewDIBBits);

	//初始化新分配内存，设定初始值为0
	LPBYTE lpDst = (LPBYTE)lpNewDIBBits;
	memset(lpDst, (BYTE)0, lWidth * lHeight);
	float gMin = 100.0;
	float gMax = 200.0;
	//逐个扫描图像中的像素点，进行灰度线性变换
	for (int j  = 0;  j < lHeight; j++)
	{
		for (int i = 0; i < lWidth; i++)
		{
			//指向源图像倒数第j行第i个像素的指针
			LPBYTE lpSrc = (LPBYTE)lpDIBBits + lWidth * j + i;
			//指向目标图像倒数第j行第i个像素的指针
			LPBYTE lpDst = (LPBYTE)lpNewDIBBits + lWidth * j + i;
			//源像素值
			BYTE pixel = (BYTE)*lpSrc;
			*lpDst = (BYTE)(((float)(gMax - gMin) / 255) * pixel + gMin + 0.5);
		}
	}
	//复制变换后的图像
	memcpy(lpDIBBits, lpNewDIBBits, lWidth * lHeight);
	//释放内存
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);
	
	CDC* pDC = GetDC();
	CPoint pt;
	pt.x = 10;
	pt.y = 10;
	theDIB.Draw(pDC, pt, CSize(lWidth, lHeight));

}


void CImageProcessView::OnOpen2()
{
	// TODO:  在此添加命令处理程序代码
	CString fileName;
	CFileDialog dlg(TRUE, _T("BMP"), _T("*.BMP"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("位图文件(*.BMP)|*.BMP|"));
	if (IDOK == dlg.DoModal())
	{
		fileName.Format("%s", dlg.GetPathName());
	}
	theDIB.LoadFromFile(fileName);
	_bLoadImage = true;
	//显示图像
	CPoint pt;
	pt.x = 0;
	pt.y = 0;
	LONG lWidth = theDIB.GetWidth();
	LONG lHeight = theDIB.GetHeight();
	CSize theSize;
	theSize.cx = lWidth;
	theSize.cy = lHeight;
	CDC* pDC = GetDC();
	theDIB.Draw(pDC, pt, theSize);
}
