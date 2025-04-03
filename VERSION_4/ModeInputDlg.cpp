// ModeInputDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VERSION_4.h"
#include "ModeInputDlg.h"


// CModeInputDlg dialog

IMPLEMENT_DYNAMIC(CModeInputDlg, CDialog)

CModeInputDlg::CModeInputDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CModeInputDlg::IDD, pParent)
{
	mode = 0;
}

CModeInputDlg::~CModeInputDlg()
{
}

void CModeInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CModeInputDlg, CDialog)
	ON_BN_CLICKED(IDC_MANUAL_BTN, &CModeInputDlg::OnBnClickedManualBtn)
	ON_BN_CLICKED(IDC_AVTO_BTN, &CModeInputDlg::OnBnClickedAvtoBtn)
END_MESSAGE_MAP()


// CModeInputDlg message handlers

void CModeInputDlg::OnCancel()
{
	//CDialog::OnCancel();
}

void CModeInputDlg::OnBnClickedManualBtn()
{
	mode = 0;
	OnOK();
}

void CModeInputDlg::OnBnClickedAvtoBtn()
{
	mode = 1;
	OnOK();
}
