
// ImageProcessDoc.cpp : CImageProcessDoc ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "ImageProcess.h"
#endif

#include "ImageProcessDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CImageProcessDoc

IMPLEMENT_DYNCREATE(CImageProcessDoc, CDocument)

BEGIN_MESSAGE_MAP(CImageProcessDoc, CDocument)
END_MESSAGE_MAP()


// CImageProcessDoc ����/����

CImageProcessDoc::CImageProcessDoc()
{
	// TODO:  �ڴ����һ���Թ������
	m_hDIB = NULL;
	m_palDIB = NULL;
	m_sizeDoc = CSize(1, 1);

}

CImageProcessDoc::~CImageProcessDoc()
{
	//�ж��Ƿ���DIB����
	if ( m_hDIB != NULL )
	{
		::GlobalFree((HGLOBAL)m_hDIB);
	}
	//�жϵ�ɫ���Ƿ����
	if ( m_palDIB != NULL )
	{
		delete m_palDIB;
	}
}

BOOL CImageProcessDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO:  �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	return TRUE;
}

// CImageProcessDoc ���л�

void CImageProcessDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO:  �ڴ���Ӵ洢����
	}
	else
	{
		// TODO:  �ڴ���Ӽ��ش���
	}
}

#ifdef SHARED_HANDLERS

// ����ͼ��֧��
void CImageProcessDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// �޸Ĵ˴����Ի����ĵ�����
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// ������������֧��
void CImageProcessDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// ���ĵ����������������ݡ�
	// ���ݲ���Ӧ�ɡ�;���ָ�

	// ����:     strSearchContent = _T("point;rectangle;circle;ole object;")��
	SetSearchContent(strSearchContent);
}

void CImageProcessDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CImageProcessDoc ���

#ifdef _DEBUG
void CImageProcessDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CImageProcessDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CImageProcessDoc ����

BOOL CImageProcessDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	CFile file;
	//���ļ�
	if ( !file.Open(lpszPathName, CFile::modeRead | CFile::shareDenyWrite ) )
	{
		return FALSE;
	}
	DeleteContents();
	//��ȡͼ��
	m_hDIB = m_dib.loadFile(file);
	if (m_hDIB == NULL)
	{
		return FALSE;
	}
	//��ʼ��DIB
	this->setDIB();
	//�ж϶�ȡ�ļ��Ƿ�ɹ�
	if ( m_hDIB == NULL )
	{
		AfxMessageBox(_T("��ȡͼ��ʱ����"));
		return FALSE;

	}
	SetPathName(lpszPathName);
	SetModifiedFlag(FALSE);
	return TRUE;

}

BOOL CImageProcessDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	CFile file;
	//���ļ�
	if (!file.Open(lpszPathName, CFile::modeCreate | CFile::modeReadWrite |CFile::shareExclusive))
	{
		return FALSE;
	}
	//����ͼ��
	BOOL bSuccess = m_dib.saveFile(m_hDIB, file);
	//�ر��ļ�
	file.Close();
	SetModifiedFlag(FALSE);
	if ( !bSuccess )
	{
		AfxMessageBox(_T("����BMPͼ��ʱ����"));
	}
	return bSuccess;

}

void CImageProcessDoc::updateObject(HGLOBAL hDIB)
{
	if ( m_hDIB != NULL)
	{
		::GlobalFree((HGLOBAL)m_hDIB);
	}
	m_hDIB = hDIB;
}
void CImageProcessDoc::setDIB()
{
	LPSTR lpdib = (LPSTR) ::GlobalLock((HGLOBAL)m_hDIB);
	//�ж�ͼ���Ƿ����
	if ( m_dib.getWidth(lpdib) > INT_MAX ||
		m_dib.getHeight(lpdib) > INT_MAX )
	{
		//�������ͷ�DIB����
		::GlobalUnlock((HGLOBAL)m_hDIB);
		::GlobalFree((HGLOBAL)m_hDIB);
		//����DIBΪ��
		m_hDIB = NULL;
		AfxMessageBox(_T("��ʼ��ʧ��"));
		return;
	}
	//�����ĵ���С
	m_sizeDoc = CSize((int)m_dib.getWidth(lpdib), (int)m_dib.getHeight(lpdib));
	::GlobalUnlock((HGLOBAL)m_hDIB);
	//�����µ�ɫ��
	m_palDIB = new CPalette;
	//�ж��Ƿ񴴽��ɹ�
	if ( m_palDIB == NULL )
	{
		::GlobalFree((HGLOBAL)m_hDIB);
		m_hDIB = NULL;
		return;
	}
	if ( m_dib.ConstructPalette(m_hDIB, m_palDIB) == NULL)
	{
		delete m_palDIB;
		m_palDIB = NULL;
		return;
	}
}
