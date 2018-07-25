
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
END_MESSAGE_MAP()

// CImageProcessView 构造/析构

CImageProcessView::CImageProcessView()
{
	// TODO:  在此处添加构造代码
	m_pDbImage = NULL;
	m_nDWTCurDepth = 0;

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
