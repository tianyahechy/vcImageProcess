#pragma once


// CDlgTran �Ի���

class CDlgTran : public CDialog
{
	DECLARE_DYNAMIC(CDlgTran)

public:
	CDlgTran(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgTran();

// �Ի�������
	enum { IDD = IDD_DLGTRAN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int m_horOff;
	int m_verOff;
};
