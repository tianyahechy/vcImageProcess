// DlgZoom.cpp : 实现文件
//

#include "stdafx.h"
#include "ImageProcess.h"
#include "DlgZoom.h"
#include "afxdialogex.h"


// CDlgZoom 对话框

IMPLEMENT_DYNAMIC(CDlgZoom, CDialog)

CDlgZoom::CDlgZoom(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgZoom::IDD, pParent)
	, m_horZoom(0)
	, m_verZoom(0)
{

}

CDlgZoom::~CDlgZoom()
{
}

void CDlgZoom::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_horZoom);
	DDX_Text(pDX, IDC_EDIT2, m_verZoom);
}


BEGIN_MESSAGE_MAP(CDlgZoom, CDialog)
END_MESSAGE_MAP()


// CDlgZoom 消息处理程序
