#pragma once


// CDlgTran 对话框

class CDlgTran : public CDialog
{
	DECLARE_DYNAMIC(CDlgTran)

public:
	CDlgTran(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgTran();

// 对话框数据
	enum { IDD = IDD_DLGTRAN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_horOff;
	int m_verOff;
};
