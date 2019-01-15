
// ImageProcessView.cpp : CImageProcessView ���ʵ��
//

#include "stdafx.h"
#include <gdiplusinit.h>
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
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
	// ��׼��ӡ����
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

// CImageProcessView ����/����

CImageProcessView::CImageProcessView()
{
	// TODO:  �ڴ˴���ӹ������
	m_pDbImage = NULL;
	m_nDWTCurDepth = 0;
	_bLoadImage = false;
}

CImageProcessView::~CImageProcessView()
{
}

BOOL CImageProcessView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CView::PreCreateWindow(cs);
}

// CImageProcessView ����

void CImageProcessView::OnDraw(CDC* pDC)
{
	CImageProcessDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO:  �ڴ˴�Ϊ����������ӻ��ƴ���
	HGLOBAL hDIB = pDoc->GetHObject();
	//�ж�DIB�Ƿ�Ϊ��
	if ( hDIB != NULL )
	{
		LPSTR lpDibSection = (LPSTR) ::GlobalLock((HGLOBAL)hDIB);
		//��ȡDIB���
		int cxDIB = (int)pDoc->m_dib.getWidth(lpDibSection);
		//��ȡDIB�߶�
		int cyDIB = (int)pDoc->m_dib.getHeight(lpDibSection);
		::GlobalUnlock((HGLOBAL)hDIB);

		CRect rcDIB;
		rcDIB.top = rcDIB.left = 0;
		rcDIB.right = cxDIB;
		rcDIB.bottom = cyDIB;
		CRect rcDest = rcDIB;
		//���DIB��
		pDoc->m_dib.DrawDib(pDC->m_hDC, &rcDest, pDoc->GetHObject(), &rcDIB, pDoc->GetDocPal());
	}

}


// CImageProcessView ��ӡ


void CImageProcessView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CImageProcessView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void CImageProcessView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void CImageProcessView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  ��Ӵ�ӡ����е��������
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


// CImageProcessView ���

#ifdef _DEBUG
void CImageProcessView::AssertValid() const
{
	CView::AssertValid();
}

void CImageProcessView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CImageProcessDoc* CImageProcessView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CImageProcessDoc)));
	return (CImageProcessDoc*)m_pDocument;
}
#endif //_DEBUG


// CImageProcessView ��Ϣ�������


void CImageProcessView::OnTranslation()
{
	// TODO:  �ڴ���������������
	CImageProcessDoc * pDoc = GetDocument();
	//����DIB,ָ��Դͼ���ָ��
	LPSTR lpSrcDib = (LPSTR) ::GlobalLock((HGLOBAL)pDoc->GetHObject());
	//�ж��Ƿ���8λλͼ�������򷵻�
	if ( pDoc->m_dib.getColorNum(lpSrcDib) != 256 )
	{
		AfxMessageBox(_T("�Բ��𣬲���256λͼ"));
		::GlobalUnlock((HGLOBAL)pDoc->GetHObject());
		return;
	}
	//�ҵ�DIBͼ��������ʼλ�õ�ָ��
	LPSTR lpSrcStartBits = pDoc->m_dib.GetBits(lpSrcDib);
	//���ͼ��Ŀ��
	long lSrcWidth = pDoc->m_dib.getWidth(lpSrcDib);
	//���ͼ��ĸ߶�
	long lSrcHeight = pDoc->m_dib.getHeight(lpSrcDib);
	//����ͼ��ÿ�е��ֽ���
	long lSrcLineBytes = pDoc->m_dib.getReqByteWidth(lSrcWidth * 8);
	long lDstLineBytes = pDoc->m_dib.getReqByteWidth(lSrcWidth * 8);
	//������ͼ��ÿ�е��ֽ���
	CDlgTran tranPara;
	if (tranPara.DoModal() != IDOK)
	{
		return;
	}

	int temVer = tranPara.m_verOff;
	int temHor = tranPara.m_horOff;
	//ƽ��
	if (Translation(lpSrcStartBits, lSrcWidth,lSrcHeight,temVer,temHor,lSrcLineBytes, lDstLineBytes))
	{
		//���ñ��
		pDoc->SetModifiedFlag(TRUE);
		//������ͼ
		pDoc->UpdateAllViews(NULL);
		//�������
		::GlobalUnlock((HGLOBAL)pDoc->GetHObject());
	}
	else
	{
		AfxMessageBox(_T("�����ڴ�ʧ��!"));
	}
}

void CImageProcessView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	pView = this;
}

void CImageProcessView::OnRobertsedgedetection()
{/*
	// TODO:  �ڴ���������������
	CImageProcessDoc * pDoc = GetDocument();
	//����DIB,ָ��Դͼ���ָ��
	LPSTR lpSrcDib = (LPSTR) ::GlobalLock((HGLOBAL)pDoc->GetHObject());
	LPBYTE lpData = pDoc->m_dib.getBitCount
	//�ж��Ƿ���8λλͼ�������򷵻�
	if (pDoc->m_dib.getColorNum(lpSrcDib) != 256)
	{
		AfxMessageBox(_T("�Բ��𣬲���256λͼ"));
		::GlobalUnlock((HGLOBAL)pDoc->GetHObject());
		return;
	}
	//�ҵ�DIBͼ��������ʼλ�õ�ָ��
	LPSTR lpSrcStartBits = pDoc->m_dib.GetBits(lpSrcDib);
	//���ͼ��Ŀ��
	long nWidth = pDoc->m_dib.getWidth(lpSrcDib);
	//���ͼ��ĸ߶�
	long nHeight = pDoc->m_dib.getHeight(lpSrcDib);
	//����ͼ��ÿ�е��ֽ���
	long lSrcLineBytes = pDoc->m_dib.getReqByteWidth(lSrcWidth * 8);
	long lDstLineBytes = pDoc->m_dib.getReqByteWidth(lSrcWidth * 8);
	//������ͼ��ÿ�е��ֽ���
	CDlgTran tranPara;
	if (tranPara.DoModal() != IDOK)
	{
		return;
	}

	int temVer = tranPara.m_verOff;
	int temHor = tranPara.m_horOff;
	//ƽ��
	if (Translation(lpSrcStartBits, lSrcWidth, lSrcHeight, temVer, temHor, lSrcLineBytes, lDstLineBytes))
	{
		//���ñ��
		pDoc->SetModifiedFlag(TRUE);
		//������ͼ
		pDoc->UpdateAllViews(NULL);
		//�������
		::GlobalUnlock((HGLOBAL)pDoc->GetHObject());
	}
	else
	{
		AfxMessageBox(_T("�����ڴ�ʧ��!"));
	}
	*/

}


void CImageProcessView::OnRotation()
{
	// TODO:  �ڴ���������������// TODO:  �ڴ���������������
	CImageProcessDoc * pDoc = GetDocument();
	//����DIB,ָ��Դͼ���ָ��
	LPSTR lpSrcDib = (LPSTR) ::GlobalLock((HGLOBAL)pDoc->GetHObject());
	//�ж��Ƿ���8λλͼ�������򷵻�
	if (pDoc->m_dib.getColorNum(lpSrcDib) != 256)
	{
		AfxMessageBox(_T("�Բ��𣬲���256λͼ"));
		::GlobalUnlock((HGLOBAL)pDoc->GetHObject());
		return;
	}
	//�ҵ�DIBͼ��������ʼλ�õ�ָ��
	LPSTR lpSrcStartBits = pDoc->m_dib.GetBits(lpSrcDib);
	//���ͼ��Ŀ��
	long lSrcWidth = pDoc->m_dib.getWidth(lpSrcDib);
	//���ͼ��ĸ߶�
	long lSrcHeight = pDoc->m_dib.getHeight(lpSrcDib);
	//����ͼ��ÿ�е��ֽ���
	long lSrcLineBytes = pDoc->m_dib.getReqByteWidth(lSrcWidth * 8);

	//������ͼ��ÿ�е��ֽ���
	CDlgRot rotPara;
	if (rotPara.DoModal() != IDOK)
	{
		return;
	}
	DWORD palSize = pDoc->m_dib.getPalSize(lpSrcDib);
	//����ת�ǶȴӶ�ת��Ϊ����
	float fRotationAngle = (float)AngleToRadian(rotPara.m_rotAngle);
	//������ת�Ƕȵ�������
	float fSina = (float)sin((double)fRotationAngle);
	float fCosa = (float)cos((double)fRotationAngle);

	//������תǰ4���ǵ����꣨��ͼ������Ϊ����ϵԭ�㣩
	float fSrcX1 = (float)(-(lSrcWidth - 1) / 2);
	float fSrcY1 = (float)((lSrcHeight - 1) / 2);
	float fSrcX2 = (float)((lSrcWidth - 1) / 2);
	float fSrcY2 = (float)((lSrcHeight - 1) / 2);
	float fSrcX3 = (float)(-(lSrcWidth - 1) / 2);
	float fSrcY3 = (float)(-(lSrcHeight - 1) / 2);
	float fSrcX4 = (float)((lSrcWidth - 1) / 2);
	float fSrcY4 = (float)(-(lSrcHeight - 1) / 2);

	//������ͼ4���ǵ�����
	float fDstX1 = fCosa * fSrcX1 + fSina * fSrcY1;
	float fDstY1 = -fSina * fSrcX1 + fCosa * fSrcY1;
	float fDstX2 = fCosa * fSrcX2 + fSina * fSrcY2;
	float fDstY2 = -fSina * fSrcX2 + fCosa * fSrcY2;
	float fDstX3 = fCosa * fSrcX3 + fSina * fSrcY3;
	float fDstY3 = -fSina * fSrcX3 + fCosa * fSrcY3;
	float fDstX4 = fCosa * fSrcX4 + fSina * fSrcY4;
	float fDstY4 = -fSina * fSrcX4 + fCosa * fSrcY4;

	//������ת���ͼ��ʵ�ʿ��
	long lDstWidth = (long)(max(fabs(fDstX4 - fDstX1), fabs(fDstX3 - fDstX2)) + 0.5);
	//������ͼ��ÿ�е��ֽ���
	long lDstLineBytes = pDoc->m_dib.getReqByteWidth(lDstWidth * 8);
	//������ת���ͼ��߶�
	long lDstHeight = (long)(max(fabs(fDstY4 - fDstY1), fabs(fDstY3 - fDstY2)) + 0.5);
	//������dib,��ת
	HGLOBAL hDstDIB = (HGLOBAL)Rotate(lpSrcDib,lpSrcStartBits, lSrcWidth, lSrcHeight, 
		lSrcLineBytes, palSize, lDstWidth, lDstHeight, lDstLineBytes, fSina, fCosa);
	if (hDstDIB != NULL)
	{
		//�滻dib��ͬʱ�ͷž�dib����
		pDoc->updateObject(hDstDIB);
		//����dib��С�͵�ɫ��
		pDoc->setDIB();
		//�����޸ı��
		pDoc->SetModifiedFlag(TRUE);
		//������ͼ
		pDoc->UpdateAllViews(NULL);
		//�������
		::GlobalUnlock((HGLOBAL)pDoc->GetHObject());
	}
	else
	{
		AfxMessageBox(_T("�����ڴ�ʧ��!"));
	}
}


void CImageProcessView::OnMirror()
{
	// TODO:  �ڴ���������������
	CImageProcessDoc * pDoc = GetDocument();
	//����DIB,ָ��Դͼ���ָ��
	LPSTR lpSrcDib = (LPSTR) ::GlobalLock((HGLOBAL)pDoc->GetHObject());
	//�ж��Ƿ���8λλͼ�������򷵻�
	if (pDoc->m_dib.getColorNum(lpSrcDib) != 256)
	{
		AfxMessageBox(_T("�Բ��𣬲���256λͼ"));
		::GlobalUnlock((HGLOBAL)pDoc->GetHObject());
		return;
	}
	//�ҵ�DIBͼ��������ʼλ�õ�ָ��
	LPSTR lpSrcStartBits = pDoc->m_dib.GetBits(lpSrcDib);
	//���ͼ��Ŀ��
	long lSrcWidth = pDoc->m_dib.getWidth(lpSrcDib);
	//���ͼ��ĸ߶�
	long lSrcHeight = pDoc->m_dib.getHeight(lpSrcDib);
	//����ͼ��ÿ�е��ֽ���
	long lSrcLineBytes = pDoc->m_dib.getReqByteWidth(lSrcWidth * 8);

	//ƽ��
	if (Mirror(lpSrcStartBits, lSrcWidth, lSrcHeight, lSrcLineBytes))
	{
		//���ñ��
		pDoc->SetModifiedFlag(TRUE);
		//������ͼ
		pDoc->UpdateAllViews(NULL);
		//�������
		::GlobalUnlock((HGLOBAL)pDoc->GetHObject());
	}
	else
	{
		AfxMessageBox(_T("�����ڴ�ʧ��!"));
	}
}


void CImageProcessView::OnMirror2()
{
	// TODO:  �ڴ���������������
	CImageProcessDoc * pDoc = GetDocument();
	//����DIB,ָ��Դͼ���ָ��
	LPSTR lpSrcDib = (LPSTR) ::GlobalLock((HGLOBAL)pDoc->GetHObject());
	//�ж��Ƿ���8λλͼ�������򷵻�
	if (pDoc->m_dib.getColorNum(lpSrcDib) != 256)
	{
		AfxMessageBox(_T("�Բ��𣬲���256λͼ"));
		::GlobalUnlock((HGLOBAL)pDoc->GetHObject());
		return;
	}
	//�ҵ�DIBͼ��������ʼλ�õ�ָ��
	LPSTR lpSrcStartBits = pDoc->m_dib.GetBits(lpSrcDib);
	//���ͼ��Ŀ��
	long lSrcWidth = pDoc->m_dib.getWidth(lpSrcDib);
	//���ͼ��ĸ߶�
	long lSrcHeight = pDoc->m_dib.getHeight(lpSrcDib);
	//����ͼ��ÿ�е��ֽ���
	long lSrcLineBytes = pDoc->m_dib.getReqByteWidth(lSrcWidth * 8);

	//ƽ��
	if (Mirror2(lpSrcStartBits, lSrcWidth, lSrcHeight, lSrcLineBytes))
	{
		//���ñ��
		pDoc->SetModifiedFlag(TRUE);
		//������ͼ
		pDoc->UpdateAllViews(NULL);
		//�������
		::GlobalUnlock((HGLOBAL)pDoc->GetHObject());
	}
	else
	{
		AfxMessageBox(_T("�����ڴ�ʧ��!"));
	}
}


void CImageProcessView::OnTranspose()
{
	// TODO:  �ڴ���������������
	CImageProcessDoc * pDoc = GetDocument();
	//����DIB,ָ��Դͼ���ָ��
	LPSTR lpSrcDib = (LPSTR) ::GlobalLock((HGLOBAL)pDoc->GetHObject());
	//�ж��Ƿ���8λλͼ�������򷵻�
	if (pDoc->m_dib.getColorNum(lpSrcDib) != 256)
	{
		AfxMessageBox(_T("�Բ��𣬲���256λͼ"));
		::GlobalUnlock((HGLOBAL)pDoc->GetHObject());
		return;
	}
	//�ҵ�DIBͼ��������ʼλ�õ�ָ��
	LPSTR lpSrcStartBits = pDoc->m_dib.GetBits(lpSrcDib);
	//���ͼ��Ŀ��
	long lSrcWidth = pDoc->m_dib.getWidth(lpSrcDib);
	//���ͼ��ĸ߶�
	long lSrcHeight = pDoc->m_dib.getHeight(lpSrcDib);
	//����ͼ��ÿ�е��ֽ���
	long lSrcLineBytes = pDoc->m_dib.getReqByteWidth(lSrcWidth * 8);
	//������ͼ��ÿ�е��ֽ���
	long lDstLineBytes = pDoc->m_dib.getReqByteWidth(lSrcHeight * 8);

	//ת��
	if (Transpose(lpSrcDib,lpSrcStartBits, lSrcWidth, lSrcHeight, lSrcLineBytes,lDstLineBytes))
	{
		//����dibͼ���С�͵�ɫ��
		pDoc->setDIB();
		//���ñ��
		pDoc->SetModifiedFlag(TRUE);
		//������ͼ
		pDoc->UpdateAllViews(NULL);
		//�������
		::GlobalUnlock((HGLOBAL)pDoc->GetHObject());
	}
	else
	{
		AfxMessageBox(_T("�����ڴ�ʧ��!"));
	}
}

void CImageProcessView::OnZoom()
{
	// TODO:  �ڴ���������������
	CImageProcessDoc * pDoc = GetDocument();
	//����DIB,ָ��Դͼ���ָ��
	LPSTR lpSrcDib = (LPSTR) ::GlobalLock((HGLOBAL)pDoc->GetHObject());
	//�ж��Ƿ���8λλͼ�������򷵻�
	if (pDoc->m_dib.getColorNum(lpSrcDib) != 256)
	{
		AfxMessageBox(_T("�Բ��𣬲���256λͼ"));
		::GlobalUnlock((HGLOBAL)pDoc->GetHObject());
		return;
	}
	//�ҵ�DIBͼ��������ʼλ�õ�ָ��
	LPSTR lpSrcStartBits = pDoc->m_dib.GetBits(lpSrcDib);
	//���ͼ��Ŀ��
	long lSrcWidth = pDoc->m_dib.getWidth(lpSrcDib);
	//���ͼ��ĸ߶�
	long lSrcHeight = pDoc->m_dib.getHeight(lpSrcDib);
	//����ͼ��ÿ�е��ֽ���
	long lSrcLineBytes = pDoc->m_dib.getReqByteWidth(lSrcWidth * 8);
	DWORD palSize = pDoc->m_dib.getPalSize(lpSrcDib);
	//������ͼ��ÿ�е��ֽ���
	CDlgZoom zoomPara;
	if (zoomPara.DoModal() != IDOK)
	{
		return;
	}

	float fX = zoomPara.m_horZoom;
	float fY = zoomPara.m_verZoom;

	//�������ź��ͼ��ʵ�ʿ�ȣ�
	long lDstWidth = (long)(lSrcWidth * fX + 0.5);
	//ת����ͼ��Ӧ�е��ֽ�����Ϊ4�ı���
	long lDstLineBytes = pDoc->m_dib.getReqByteWidth(lDstWidth * 8);
	//�������ź��ͼ��߶�
	long lDstHeight = (long)(lSrcHeight * fY + 0.5);
	//������DIB
	HGLOBAL hDstDIB = (HGLOBAL)Zoom(lpSrcDib, lpSrcStartBits, lSrcWidth, lSrcHeight, lSrcLineBytes, 
		palSize, lDstWidth, lDstHeight, lDstLineBytes, fX, fY);
	//ƽ��
	if (hDstDIB != NULL)
	{
		//�滻DIB,ͬʱ�ͷž�DIB
		pDoc->updateObject(hDstDIB);
		//����DIB��С�͵�ɫ��
		pDoc->setDIB();
		//���ñ��
		pDoc->SetModifiedFlag(TRUE);
		//������ͼ
		pDoc->UpdateAllViews(NULL);
		//�������
		::GlobalUnlock((HGLOBAL)pDoc->GetHObject());
	}
	else
	{
		AfxMessageBox(_T("�����ڴ�ʧ��!"));
	}
}


void CImageProcessView::OnGeomrota()
{/*
	// TODO:  �ڴ���������������
	CImageProcessDoc * pDoc = GetDocument();
	//����DIB,ָ��Դͼ���ָ��
	LPSTR lpSrcDib = (LPSTR) ::GlobalLock((HGLOBAL)pDoc->GetHObject());
	//�ж��Ƿ���8λλͼ�������򷵻�
	if (pDoc->m_dib.getColorNum(lpSrcDib) != 256)
	{
		AfxMessageBox(_T("�Բ��𣬲���256λͼ"));
		::GlobalUnlock((HGLOBAL)pDoc->GetHObject());
		return;
	}
	//�ҵ�DIBͼ��������ʼλ�õ�ָ��
	LPSTR lpSrcStartBits = pDoc->m_dib.GetBits(lpSrcDib);
	//���ͼ��Ŀ��
	long lSrcWidth = pDoc->m_dib.getWidth(lpSrcDib);
	//���ͼ��ĸ߶�
	long lSrcHeight = pDoc->m_dib.getHeight(lpSrcDib);
	//����ͼ��ÿ�е��ֽ���
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
	//ƽ��
	if (hNewDIB != NULL)
	{
		pDoc->updateObject(hNewDIB);
		pDoc->setDIB();
		//���ñ��
		pDoc->SetModifiedFlag(TRUE);
		//������ͼ
		pDoc->UpdateAllViews(NULL);
	}
	else
	{
		AfxMessageBox(_T("�����ڴ�ʧ��!"));
	}
	//�������
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
	//�ҵ�Դͼ�����ʼλ��
	LPBYTE lpDIBBits = theDIB.GetData();
	//���ͼ��Ŀ��
	long lWidth = theDIB.GetWidth();
	//���ͼ��ĸ߶�
	long lHeight = theDIB.GetHeight();
	//��ʱ�����ڴ����ͼ��
	HLOCAL hNewDIBBits = LocalAlloc(LHND, lWidth * lHeight);
	if (hNewDIBBits == NULL)
	{
		return;
	}
	//ָ�򻺴�IDBͼ���ָ��
	LPBYTE lpNewDIBBits = (LPBYTE)LocalLock(hNewDIBBits);

	//��ʼ���·����ڴ棬�趨��ʼֵΪ0
	LPBYTE lpDst = (LPBYTE)lpNewDIBBits;
	memset(lpDst, (BYTE)0, lWidth * lHeight);
	float gMin = 100.0;
	float gMax = 200.0;
	//���ɨ��ͼ���е����ص㣬���лҶ����Ա任
	for (int j  = 0;  j < lHeight; j++)
	{
		for (int i = 0; i < lWidth; i++)
		{
			//ָ��Դͼ������j�е�i�����ص�ָ��
			LPBYTE lpSrc = (LPBYTE)lpDIBBits + lWidth * j + i;
			//ָ��Ŀ��ͼ������j�е�i�����ص�ָ��
			LPBYTE lpDst = (LPBYTE)lpNewDIBBits + lWidth * j + i;
			//Դ����ֵ
			BYTE pixel = (BYTE)*lpSrc;
			*lpDst = (BYTE)(((float)(gMax - gMin) / 255) * pixel + gMin + 0.5);
		}
	}
	//���Ʊ任���ͼ��
	memcpy(lpDIBBits, lpNewDIBBits, lWidth * lHeight);
	//�ͷ��ڴ�
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
	// TODO:  �ڴ���������������
	CString fileName;
	CFileDialog dlg(TRUE, _T("BMP"), _T("*.BMP"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("λͼ�ļ�(*.BMP)|*.BMP|"));
	if (IDOK == dlg.DoModal())
	{
		fileName.Format("%s", dlg.GetPathName());
	}
	theDIB.LoadFromFile(fileName);
	_bLoadImage = true;
	//��ʾͼ��
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
