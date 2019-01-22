
// ImageProcessView.h : CImageProcessView ��Ľӿ�
//

#pragma once
#include "ImageProcessDoc.h"
#include "Dib2.h"
#include "HistogramDib.h"

class CImageProcessView : public CView
{
protected: // �������л�����
	CImageProcessView();
	DECLARE_DYNCREATE(CImageProcessView)

// ����
public:
	CImageProcessDoc* GetDocument() const;

// ����
public:
	CDib2 theDIB;
	bool _bLoadImage;
// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ʵ��
public:
	virtual ~CImageProcessView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	double * m_pDbImage;
	int m_nDWTCurDepth;

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTranslation();
	afx_msg void OnRobertsedgedetection();
	afx_msg void OnRotation();
	afx_msg void OnMirror();
	afx_msg void OnMirror2();
	afx_msg void OnTranspose();
	afx_msg void OnZoom();
	afx_msg void OnGeomrota();
	afx_msg void OngraytransformLinear();
	afx_msg void OnOpen2();
	afx_msg void Onseglineartransform();
	afx_msg void Onnonlineartransform();
	afx_msg void OnHistogramEqulization();
	afx_msg void OnHistogramMatch();
	afx_msg void OnAverageSmooth();
};

#ifndef _DEBUG  // ImageProcessView.cpp �еĵ��԰汾
inline CImageProcessDoc* CImageProcessView::GetDocument() const
   { return reinterpret_cast<CImageProcessDoc*>(m_pDocument); }
#endif

