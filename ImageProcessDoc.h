
// ImageProcessDoc.h : CImageProcessDoc ��Ľӿ�
//


#pragma once
#include "Dib.h"

extern CDib m_dib;
class CImageProcessDoc : public CDocument
{
protected: // �������л�����
	CImageProcessDoc();
	DECLARE_DYNCREATE(CImageProcessDoc)

// ����
public:
	CDib m_dib;	//����һ��dib����

protected:
	HGLOBAL m_hDIB;
	CPalette * m_palDIB;
	CSize m_sizeDoc;
// ����
public:
	//��ȡDib����ľ��
	HGLOBAL GetHObject() const
	{
		return m_hDIB;
	}
	//��ȡ��ɫ��ָ��
	CPalette * GetDocPal() const
	{
		return m_palDIB;
	}
	//��ȡ��ɫ���С
	CSize GetDocDimension() const
	{
		return m_sizeDoc;
	}
	//����dib����
	void updateObject(HGLOBAL hDIB);

public:
	//��ʼ��dib����
	void setDIB();
// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// ʵ��
public:
	virtual ~CImageProcessDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// ����Ϊ����������������������ݵ� Helper ����
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
