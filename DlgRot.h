#pragma once


// CDlgRot �Ի���

class CDlgRot : public CDialog
{
	DECLARE_DYNAMIC(CDlgRot)

public:
	CDlgRot(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgRot();

// �Ի�������
	enum { IDD = IDD_DLGROT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	double m_rotAngle;
};
