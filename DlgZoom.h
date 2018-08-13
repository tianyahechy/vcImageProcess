#pragma once


// CDlgZoom 对话框

class CDlgZoom : public CDialog
{
	DECLARE_DYNAMIC(CDlgZoom)

public:
	CDlgZoom(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgZoom();

// 对话框数据
	enum { IDD = IDD_DLGZOOM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	float m_horZoom;
	float m_verZoom;
};
