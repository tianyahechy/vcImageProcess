
// ImageProcessDoc.h : CImageProcessDoc 类的接口
//


#pragma once
#include "Dib.h"

extern CDib m_dib;
class CImageProcessDoc : public CDocument
{
protected: // 仅从序列化创建
	CImageProcessDoc();
	DECLARE_DYNCREATE(CImageProcessDoc)

// 特性
public:
	CDib m_dib;	//声明一个dib对象

protected:
	HGLOBAL m_hDIB;
	CPalette * m_palDIB;
	CSize m_sizeDoc;
// 操作
public:
	//获取Dib对象的句柄
	HGLOBAL GetHObject() const
	{
		return m_hDIB;
	}
	//获取调色板指针
	CPalette * GetDocPal() const
	{
		return m_palDIB;
	}
	//获取调色板大小
	CSize GetDocDimension() const
	{
		return m_sizeDoc;
	}
	//更新dib对象
	void updateObject(HGLOBAL hDIB);

public:
	//初始化dib对象
	void setDIB();
// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 实现
public:
	virtual ~CImageProcessDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 用于为搜索处理程序设置搜索内容的 Helper 函数
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
