// DlgRot.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ImageProcess.h"
#include "DlgRot.h"
#include "afxdialogex.h"


// CDlgRot �Ի���

IMPLEMENT_DYNAMIC(CDlgRot, CDialog)

CDlgRot::CDlgRot(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRot::IDD, pParent)
	, m_rotAngle(0)
{

}

CDlgRot::~CDlgRot()
{
}

void CDlgRot::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_rotAngle);
}


BEGIN_MESSAGE_MAP(CDlgRot, CDialog)
END_MESSAGE_MAP()


// CDlgRot ��Ϣ�������
