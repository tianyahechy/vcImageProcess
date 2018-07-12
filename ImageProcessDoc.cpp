
// ImageProcessDoc.cpp : CImageProcessDoc 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
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


// CImageProcessDoc 构造/析构

CImageProcessDoc::CImageProcessDoc()
{
	// TODO:  在此添加一次性构造代码
	m_hDIB = NULL;
	m_palDIB = NULL;
	m_sizeDoc = CSize(1, 1);

}

CImageProcessDoc::~CImageProcessDoc()
{
	//判断是否有DIB对象
	if ( m_hDIB != NULL )
	{
		::GlobalFree((HGLOBAL)m_hDIB);
	}
	//判断调色板是否存在
	if ( m_palDIB != NULL )
	{
		delete m_palDIB;
	}
}

BOOL CImageProcessDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO:  在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}

// CImageProcessDoc 序列化

void CImageProcessDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO:  在此添加存储代码
	}
	else
	{
		// TODO:  在此添加加载代码
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CImageProcessDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
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

// 搜索处理程序的支持
void CImageProcessDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
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

// CImageProcessDoc 诊断

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


// CImageProcessDoc 命令

BOOL CImageProcessDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	CFile file;
	//打开文件
	if ( !file.Open(lpszPathName, CFile::modeRead | CFile::shareDenyWrite ) )
	{
		return FALSE;
	}
	DeleteContents();
	//读取图像
	m_hDIB = m_dib.loadFile(file);
	if (m_hDIB == NULL)
	{
		return FALSE;
	}
	//初始化DIB
	this->setDIB();
	//判断读取文件是否成功
	if ( m_hDIB == NULL )
	{
		AfxMessageBox(_T("读取图像时出错"));
		return FALSE;

	}
	SetPathName(lpszPathName);
	SetModifiedFlag(FALSE);
	return TRUE;

}

BOOL CImageProcessDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	CFile file;
	//打开文件
	if (!file.Open(lpszPathName, CFile::modeCreate | CFile::modeReadWrite |CFile::shareExclusive))
	{
		return FALSE;
	}
	//保存图像
	BOOL bSuccess = m_dib.saveFile(m_hDIB, file);
	//关闭文件
	file.Close();
	SetModifiedFlag(FALSE);
	if ( !bSuccess )
	{
		AfxMessageBox(_T("保存BMP图像时出错"));
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
	//判断图像是否过大
	if ( m_dib.getWidth(lpdib) > INT_MAX ||
		m_dib.getHeight(lpdib) > INT_MAX )
	{
		//解锁并释放DIB对象
		::GlobalUnlock((HGLOBAL)m_hDIB);
		::GlobalFree((HGLOBAL)m_hDIB);
		//设置DIB为空
		m_hDIB = NULL;
		AfxMessageBox(_T("初始化失败"));
		return;
	}
	//设置文档大小
	m_sizeDoc = CSize((int)m_dib.getWidth(lpdib), (int)m_dib.getHeight(lpdib));
	::GlobalUnlock((HGLOBAL)m_hDIB);
	//创建新调色板
	m_palDIB = new CPalette;
	//判断是否创建成功
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
