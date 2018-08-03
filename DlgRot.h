#pragma once


// CDlgRot 对话框

class CDlgRot : public CDialog
{
	DECLARE_DYNAMIC(CDlgRot)

public:
	CDlgRot(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgRot();

// 对话框数据
	enum { IDD = IDD_DLGROT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	double m_rotAngle;
};
