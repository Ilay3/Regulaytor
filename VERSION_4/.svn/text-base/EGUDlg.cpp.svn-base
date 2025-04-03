// EGUDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VERSION_4.h"
#include "EGUDlg.h"
#include "ModeInputDlg.h"	


// CEGUDlg dialog

IMPLEMENT_DYNAMIC(CEGUDlg, CDialog)

//CEGUDlg::CEGUDlg(CWnd* pParent /*=NULL*/)
//	: CDialog(CEGUDlg::IDD, pParent)
//{

//}

CEGUDlg::CEGUDlg()
{
	pGLAVNIY_CLASS = NULL;
}

CEGUDlg::~CEGUDlg()
{
	pGLAVNIY_CLASS->m_VectEGU.clear();
}

void CEGUDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
 	DDX_Control(pDX, IDC_LIST, m_list);

	CString strFmt = _T("%.2f");
	CString str;
	if (!pDX->m_bSaveAndValidate)
	{
		if (pGLAVNIY_CLASS == (GLAVNIY_CLASS_Struct *)NULL) return;

		if (mode == 1) {
			str.Format(strFmt, pGLAVNIY_CLASS->var_Reyka);
			GetDlgItem(IDC_Y_EDIT)->SetWindowText(str);
		}
	}
}


BEGIN_MESSAGE_MAP(CEGUDlg, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK_H, OnBnClickedH)
	ON_BN_CLICKED(IDC_CHECK_K, OnBnClickedK)
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnBnClickedAdd)
	ON_BN_CLICKED(IDC_CLEAR_BUTTON, OnBnClickedClear)
	ON_COMMAND(ID_EGU_MODE, OnMenuMode)
END_MESSAGE_MAP()


// CEGUDlg message handlers

BOOL CEGUDlg::OnInitDialog()
{
	CString str;


	CDialog::OnInitDialog();

	str = L"0.00";
	GetDlgItem(IDC_Y_EDIT)->SetWindowText(str);
	GetDlgItem(IDC_H_EDIT)->SetWindowText(str);
	GetDlgItem(IDC_K_EDIT)->SetWindowText(str);

	m_list.ModifyStyle(0, LVS_REPORT|LVS_SINGLESEL);
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_list.InsertColumn(0, L"№", LVCFMT_LEFT, 40); 
	m_list.InsertColumn(1, L"Х", LVCFMT_CENTER, 80); 
	m_list.InsertColumn(2, L"Y", LVCFMT_CENTER, 80); 

	((CEdit *)GetDlgItem(IDC_Y_EDIT))->SetReadOnly(TRUE);

	flag = 0;
	mode = 0;

	SetTimer(0, 100, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CEGUDlg::Clear()
{
	CString str;


	pGLAVNIY_CLASS->m_VectEGU.clear();

	m_list.DeleteAllItems();

	str = L"0.00";
	GetDlgItem(IDC_Y_EDIT)->SetWindowText(str);
	GetDlgItem(IDC_H_EDIT)->SetWindowText(str);
	GetDlgItem(IDC_K_EDIT)->SetWindowText(str);

	// ответ
	str = L"-";
	GetDlgItem(IDC_STEPNO)->SetWindowText(str);

	GetDlgItem(IDC_LINEAR)->SetWindowText(str);

	GetDlgItem(IDC_DISTANCE)->SetWindowText(str);
}


void CEGUDlg::OnBnClickedH()
{
	CString str;
	float x;

	if (((CButton *)(GetDlgItem(IDC_CHECK_H)))->GetCheck() == BST_CHECKED) {
		GetDlgItem(IDC_H_EDIT)->GetWindowText(str);
		if (CheckString(&str) == false)
			return;

		x = (float)_wtof(str.GetBuffer());
		str.ReleaseBuffer();

		pGLAVNIY_CLASS->fH = x;
		((CEdit *)GetDlgItem(IDC_H_EDIT))->SetReadOnly(TRUE);

		GetDlgItem(IDC_CHECK_H)->SetWindowText(L"Изменить");

		flag ++; 
	} else {
		pGLAVNIY_CLASS->fH = 0.0f;
		((CEdit *)GetDlgItem(IDC_H_EDIT))->SetReadOnly(FALSE);

		GetDlgItem(IDC_CHECK_H)->SetWindowText(L"Сохранить");
		flag --; 
	}
}

void CEGUDlg::OnBnClickedK()
{
	CString str;
	float x;

	if (((CButton *)(GetDlgItem(IDC_CHECK_K)))->GetCheck() == BST_CHECKED) {
		GetDlgItem(IDC_K_EDIT)->GetWindowText(str);
		if (CheckString(&str) == false)
			return;

		x = (float)_wtof(str.GetBuffer());
		str.ReleaseBuffer();

		pGLAVNIY_CLASS->fK = x;
		((CEdit *)GetDlgItem(IDC_K_EDIT))->SetReadOnly(TRUE);

		GetDlgItem(IDC_CHECK_K)->SetWindowText(L"Изменить");

		flag ++; 
	} else {
		pGLAVNIY_CLASS->fK = 0.0f;
		((CEdit *)GetDlgItem(IDC_K_EDIT))->SetReadOnly(FALSE);

		GetDlgItem(IDC_CHECK_K)->SetWindowText(L"Сохранить");
		flag --; 
	}
}

void CEGUDlg::OnBnClickedAdd()
{
	if (flag != 2) return;

	EGU_STRUCT  temp;
	CString str;
	float y;
	int size;
	int ix;

	size = pGLAVNIY_CLASS->m_VectEGU.size();

		GetDlgItem(IDC_Y_EDIT)->GetWindowText(str);
		if (CheckString(&str) == false)
			return;

		y = (float)_wtof(str.GetBuffer());
		str.ReleaseBuffer();

		temp.fY = y;
//	temp.fY = pGLAVNIY_CLASS->var_Reyka;

	if (size == 0) {
		temp.fX = 0;
	} else {
		temp.fX = pGLAVNIY_CLASS->m_VectEGU[size-1].fX + pGLAVNIY_CLASS->fdXe;
	}

	pGLAVNIY_CLASS->m_VectEGU.push_back(temp);

	pGLAVNIY_CLASS->CalcEGU();

	// Добавим новый элемент
	str.Format(L"%d", size+1);
	m_list.InsertItem(size, str);
	CRect rect;
	m_list.GetItemRect(0, &rect, LVIR_BOUNDS);
	m_list.Scroll(CSize(0, rect.Height()*size));

	// так как уже добавили, то повторим запорос размера
	size = pGLAVNIY_CLASS->m_VectEGU.size();

	// перепишем все элементы
	for (ix=0; ix<size; ix++) {
		str.Format(L"%.3f",	pGLAVNIY_CLASS->m_VectEGU[ix].fX);
		m_list.SetItemText(ix, 1, str);

		str.Format(L"%.3f",	pGLAVNIY_CLASS->m_VectEGU[ix].fY);
		m_list.SetItemText(ix, 2, str);
	}

	if (pGLAVNIY_CLASS->Fix_w < 0) return;

	// ответ
	str.Format(L"%d", pGLAVNIY_CLASS->Fix_w);
	GetDlgItem(IDC_STEPNO)->SetWindowText(str);

	str.Format(L"%.3f", pGLAVNIY_CLASS->fYw);
	GetDlgItem(IDC_LINEAR)->SetWindowText(str);

	str.Format(L"%.3f", pGLAVNIY_CLASS->fL);
	GetDlgItem(IDC_DISTANCE)->SetWindowText(str);
}

void CEGUDlg::OnBnClickedClear()
{
	Clear();
}

void CEGUDlg::OnTimer(UINT_PTR nIDEvent)
{
	UpdateData(FALSE);
	CDialog::OnTimer(nIDEvent);
}

bool CEGUDlg::CheckString (CString *strText)
{
	TCHAR  ch;
	int    len;


	strText->Replace(',','.');
	len = strText->GetLength();
	for (int i=0; i<len; i++)
	{
		ch = strText->GetAt(i);
		if (i == 0 && (ch == '-' || ch == '+'))
		{
			continue;
		}

		if (ch != '.')
		{
			if (ch > '9' || ch < '0') {
				AfxMessageBox(L"Ошибка ввода!");
				return false;
			}
		}
	}
	return true;
}

void CEGUDlg::OnMenuMode()
{
	CModeInputDlg dlg;
	dlg.DoModal();

	mode = dlg.mode;
	if (mode == 0) 	((CEdit *)GetDlgItem(IDC_Y_EDIT))->SetReadOnly(FALSE);
	else ((CEdit *)GetDlgItem(IDC_Y_EDIT))->SetReadOnly(TRUE);
}

