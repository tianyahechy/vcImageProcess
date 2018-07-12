// DlgTran.cpp : 实现文件
//

#include "stdafx.h"
#include "ImageProcess.h"
#include "DlgTran.h"
#include "afxdialogex.h"


// CDlgTran 对话框

IMPLEMENT_DYNAMIC(CDlgTran, CDialog)

CDlgTran::CDlgTran(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTran::IDD, pParent)
	, m_horOff(0)
	, m_verOff(0)
{

}

CDlgTran::~CDlgTran()
{
}

void CDlgTran::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT2, m_horOff);
	DDX_Text(pDX, IDC_EDIT3, m_verOff);
}


BEGIN_MESSAGE_MAP(CDlgTran, CDialog)
END_MESSAGE_MAP()


// CDlgTran 消息处理程序
