// DlgFuncName.cpp : implementation file
//

#include "stdafx.h"
#include "DlgFuncName.h"


// CDlgFuncName dialog

IMPLEMENT_DYNAMIC(CDlgFuncName, CDialog)

CDlgFuncName::CDlgFuncName(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFuncName::IDD, pParent)
{

}

CDlgFuncName::~CDlgFuncName()
{
}

void CDlgFuncName::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgFuncName, CDialog)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CDlgFuncName message handlers

INT_PTR CDlgFuncName::DoModal(CString OldName)
{
	m_FuncName=OldName;
	return CDialog::DoModal();
}

void CDlgFuncName::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	((CEdit*)GetDlgItem(IDC_FUNCNAME))->SetLimitText(255);	//защита
	//прорисую старое имя
	GetDlgItem(IDC_FUNCNAME)->SetWindowText(m_FuncName);
	//сразу всё выделю
	((CEdit*)GetDlgItem(IDC_FUNCNAME))->SetFocus();
	((CEdit*)GetDlgItem(IDC_FUNCNAME))->SetSel(0,m_FuncName.GetLength(),TRUE);
}
void CDlgFuncName::OnOK()
{
	//сохраню имя функции, т.к. эдитик разрушится
	GetDlgItem(IDC_FUNCNAME)->GetWindowText(m_FuncName);
	if(m_FuncName.GetLength()==0)
	{
		AfxMessageBox(_T("Не будьте такими жадными, введите хотя бы один символ"));
	}

	CDialog::OnOK();
}
