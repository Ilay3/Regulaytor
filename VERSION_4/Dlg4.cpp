// Dlg4.cpp : implementation file
//

#include "stdafx.h"
#include "VERSION_4Dlg.h"
#include "Dlg4.h"


// CDlg4 dialog

IMPLEMENT_DYNAMIC(CDlg4, CDialog)

CDlg4::~CDlg4()
{
}

void CDlg4::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlg4, CDialog)
	ON_BN_CLICKED(IDC_PACKETATACK, &CDlg4::OnBnClickedPacketatack)
END_MESSAGE_MAP()


// CDlg4 message handlers

BOOL CDlg4::Create(UINT ID, CVERSION_4Dlg* pParentWnd)
{
	MAIN_WINDOW=pParentWnd;	//сохраним указатель на главное окно

	return CDialog::Create(ID, (CWnd*)pParentWnd);
}

/***************    OnBnClickedPacketatack()   ***************************
*		Ќажали на кнопку, по которпой нужно отправить на регул€тор
*	подр€д целую кучу пакетов и попытатьс€ задавить его
**************************************************************************/
void CDlg4::OnBnClickedPacketatack()
{
	MAIN_WINDOW->pUSART_Dlg->SetMode(1);
	MAIN_WINDOW->pUSART_Dlg->StartTransfer();
	MAIN_WINDOW->pUSART_Dlg->StartTransfer();
	MAIN_WINDOW->pUSART_Dlg->StartTransfer();
}
