// EnterNLDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VERSION_4.h"
#include "EnterNLDlg.h"
#include "GlavniyClass.h"
#include "ModeInputDlg.h"	


// CEnterNLDlg dialog

IMPLEMENT_DYNAMIC(CEnterNLDlg, CDialog)

//CEnterNLDlg::CEnterNLDlg(CWnd* pParent /*=NULL*/)
//	: CDialog(CEnterNLDlg::IDD, pParent)
//{

//}
CEnterNLDlg::CEnterNLDlg()
{
	pGLAVNIY_CLASS = NULL;
}

CEnterNLDlg::~CEnterNLDlg()
{
	pGLAVNIY_CLASS->m_VectNonLinear.clear();
}

void CEnterNLDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
 	DDX_Control(pDX, IDC_NL_LIST, m_list);


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


BEGIN_MESSAGE_MAP(CEnterNLDlg, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK_NUMBER, OnBnClickedNumber)
	ON_BN_CLICKED(IDC_CHECKXSTART, OnBnClickedXStart)
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnBnClickedAdd)
	ON_COMMAND(ID_NL_MENU_OPEN, &CEnterNLDlg::OnNlMenuOpen)
	ON_COMMAND(ID_SAVE_NL_MENU, &CEnterNLDlg::OnNlMenuSaveTxt)
	ON_COMMAND(ID_SAVE_RAW_NL_MENU, &CEnterNLDlg::OnNlMenuSaveRaw)
	ON_COMMAND(ID_NL_MENU_CLEAR, &CEnterNLDlg::OnNlMenuClear)
	ON_COMMAND(ID_EXIT_NL_MENU, &CEnterNLDlg::OnNlMenuExit)
	ON_COMMAND(ID_NL_MENU_GRAPH, &CEnterNLDlg::OnNlMenuGraph)
	ON_COMMAND(ID_NL_MENU_MODE, &CEnterNLDlg::OnNlMenuMode)
END_MESSAGE_MAP()


// CEnterNLDlg message handlers

BOOL CEnterNLDlg::OnInitDialog()
{
	CString str;


	CDialog::OnInitDialog();

	mode = 0;

	m_ReportDlg.pGLAVNIY_CLASS = pGLAVNIY_CLASS;
	m_ReportDlg.Create(IDD_EGU_REPORT_DLG, this);
	m_ReportDlg.CenterWindow();

	((CEdit *)GetDlgItem(IDC_Y_EDIT))->SetReadOnly(FALSE);
	str = L"0.00";
	GetDlgItem(IDC_Y_EDIT)->SetWindowText(str);

	GetDlgItem(IDC_NUM_SENSOR_EDIT)->SetWindowText(pGLAVNIY_CLASS->strNumberSensor);

	str.Format(_T("%.2f"), pGLAVNIY_CLASS->fXStart);
	GetDlgItem(IDC_XSTART_EDIT)->SetWindowText(str);
	
	m_list.ModifyStyle(0, LVS_REPORT|LVS_SINGLESEL);
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_list.InsertColumn(0, L"�", LVCFMT_LEFT, 30); 
	m_list.InsertColumn(1, L"�", LVCFMT_CENTER, 50); 
	m_list.InsertColumn(2, L"Y", LVCFMT_CENTER, 50); 
	m_list.InsertColumn(3, L"h", LVCFMT_CENTER, 50); 
	m_list.InsertColumn(4, L"gamma", LVCFMT_CENTER, 80); 

	TCHAR dir[_MAX_PATH];
	GetCurrentDirectory(_MAX_PATH, dir);
	m_sDir.Format(_T("%s\\���������� ��"), dir);
	CreateDirectory(m_sDir, NULL);

	flag = 0;

	SetTimer(0, 100, NULL);
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CEnterNLDlg::OnBnClickedNumber()
{
	CString str;
	if (((CButton *)(GetDlgItem(IDC_CHECK_NUMBER)))->GetCheck() == BST_CHECKED) {
		GetDlgItem(IDC_NUM_SENSOR_EDIT)->GetWindowText(str);
		pGLAVNIY_CLASS->strNumberSensor = str;
		((CEdit *)GetDlgItem(IDC_NUM_SENSOR_EDIT))->SetReadOnly(TRUE);
		GetDlgItem(IDC_SERIALNO)->SetWindowText(str);

		GetDlgItem(IDC_CHECK_NUMBER)->SetWindowText(L"��������");

		flag ++; 
	} else {
		pGLAVNIY_CLASS->strNumberSensor = _T("�");
		((CEdit *)GetDlgItem(IDC_NUM_SENSOR_EDIT))->SetReadOnly(FALSE);

		GetDlgItem(IDC_CHECK_NUMBER)->SetWindowText(L"���������");

		flag --; 
	}
	UpdateData(FALSE);
}


void CEnterNLDlg::OnBnClickedXStart()
{
	CString str;
	float x;

	if (((CButton *)(GetDlgItem(IDC_CHECKXSTART)))->GetCheck() == BST_CHECKED) {
		GetDlgItem(IDC_XSTART_EDIT)->GetWindowText(str);
		if (CheckString(&str) == false)
			return;

		x = (float)_wtof(str.GetBuffer());
		str.ReleaseBuffer();

		pGLAVNIY_CLASS->fXStart = x;
		((CEdit *)GetDlgItem(IDC_XSTART_EDIT))->SetReadOnly(TRUE);

		GetDlgItem(IDC_CHECKXSTART)->SetWindowText(L"��������");

		flag ++; 
	} else {
		pGLAVNIY_CLASS->fXStart = 0.0f;
		((CEdit *)GetDlgItem(IDC_XSTART_EDIT))->SetReadOnly(FALSE);

		GetDlgItem(IDC_CHECKXSTART)->SetWindowText(L"���������");

		flag --; 
	}
	UpdateData(FALSE);
}

void CEnterNLDlg::OnBnClickedAdd()
{
	NONLINEAR_STRUCT  temp;
	CString str;
	float y;
	int size;
	int ix;

	if (flag != 2) return;

	size = pGLAVNIY_CLASS->m_VectNonLinear.size();

	if (mode == 0) {
		GetDlgItem(IDC_Y_EDIT)->GetWindowText(str);
		if (CheckString(&str) == false)
			return;

		y = (float)_wtof(str.GetBuffer());
		str.ReleaseBuffer();

		temp.fY = y;
	} else {
		temp.fY = pGLAVNIY_CLASS->var_Reyka;
	}

	if (size == 0) {
		temp.fX = pGLAVNIY_CLASS->fXStart;
	} else {
		temp.fX = pGLAVNIY_CLASS->m_VectNonLinear[size-1].fX + pGLAVNIY_CLASS->fdX;
	}

	pGLAVNIY_CLASS->m_VectNonLinear.push_back(temp);

	// ������� ����� �������
	str.Format(L"%d", size+1);
	m_list.InsertItem(size, str);
	CRect rect;
	m_list.GetItemRect(0, &rect, LVIR_BOUNDS);
	m_list.Scroll(CSize(0, rect.Height()*size));

	// ������ �������������
	pGLAVNIY_CLASS->CalcLinear();

	// ��� ��� ��� ��������, �� �������� ������� �������
	size = pGLAVNIY_CLASS->m_VectNonLinear.size();

	// ��������� ��� ��������
	for (ix=0; ix<size; ix++) {
		str.Format(L"%.3f",	pGLAVNIY_CLASS->m_VectNonLinear[ix].fX);
		m_list.SetItemText(ix, 1, str);

		str.Format(L"%.3f",	pGLAVNIY_CLASS->m_VectNonLinear[ix].fY);
		m_list.SetItemText(ix, 2, str);

		str.Format(L"%.3f",	pGLAVNIY_CLASS->m_VectNonLinear[ix].fh);
		m_list.SetItemText(ix, 3, str);

		str.Format(L"%.3f",	pGLAVNIY_CLASS->m_VectNonLinear[ix].fgammai);
		m_list.SetItemText(ix, 4, str);
	}

	m_list.RedrawItems(0, size-1);

	// �����
	str.Format(L"%.3f ��", pGLAVNIY_CLASS->fhmaxp);
	GetDlgItem(IDC_OTKLP)->SetWindowText(str);

	str.Format(L"%.3f ��", pGLAVNIY_CLASS->fhmaxm);
	GetDlgItem(IDC_OTKLM)->SetWindowText(str);

	str.Format(L"%.3f ��", pGLAVNIY_CLASS->fhmax);
	GetDlgItem(IDC_RAZMAX)->SetWindowText(str);

	str.Format(L"%.3f %%", pGLAVNIY_CLASS->fgamma);
	GetDlgItem(IDC_NELINEYNOST)->SetWindowText(str);

	m_ReportDlg.Update();
}


void CEnterNLDlg::Clear()
{
	CString str;


	pGLAVNIY_CLASS->m_VectNonLinear.clear();

	m_list.DeleteAllItems();

	// �����
	str = L"-";
	GetDlgItem(IDC_OTKLP)->SetWindowText(str);

	GetDlgItem(IDC_OTKLM)->SetWindowText(str);

	GetDlgItem(IDC_RAZMAX)->SetWindowText(str);

	GetDlgItem(IDC_NELINEYNOST)->SetWindowText(str);

	GetDlgItem(IDC_SERIALNO)->SetWindowText(str);

	m_ReportDlg.Update();
}

void CEnterNLDlg::OnTimer(UINT_PTR nIDEvent)
{
	UpdateData(FALSE);
	CDialog::OnTimer(nIDEvent);
}


void CEnterNLDlg::OnNlMenuOpen()
{
	Clear();

	CString m_sFile(m_sDir + _T("\\")); 

	NONLINEAR_STRUCT  temp;
	CFile m_File;
	int size;
	int lenstr;
	int ix;
	CString str;
	WCHAR *buf;


	static TCHAR BASED_CODE szFilter[] = _T("Raw Files (*.bin)|*.bin||");

	SetCurrentDirectory(m_sDir);

	CFileDialog dlg(TRUE, _T("*.bin"), 0, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilter);						
	if (dlg.DoModal() == IDOK)
	{
		m_sFile = dlg.GetPathName();

		if (m_File.Open(m_sFile, CFile::modeReadWrite, NULL)) {
			
			m_File.Read((void *)&size, sizeof(int));
			
			for (ix = 0; ix < size; ix++)
			{
				m_File.Read((void *)&temp, sizeof(NONLINEAR_STRUCT));
				pGLAVNIY_CLASS->m_VectNonLinear.push_back(temp);
			}

			m_File.Read((void *)&lenstr, sizeof(int));

			buf = new WCHAR[lenstr/sizeof(WCHAR)];
			m_File.Read((void *)buf, lenstr);
			pGLAVNIY_CLASS->strNumberSensor.SetString(buf, lenstr/sizeof(WCHAR));

			m_File.Read((void *)&pGLAVNIY_CLASS->fXStart, sizeof(float));

			m_File.Close();

			// ������ �������������
			pGLAVNIY_CLASS->CalcLinear();

			// ��������� ��� ��������
			for (ix=0; ix<size; ix++) {
				str.Format(L"%d", ix+1);
				m_list.InsertItem(ix, str);

				str.Format(L"%.3f",	pGLAVNIY_CLASS->m_VectNonLinear[ix].fX);
				m_list.SetItemText(ix, 1, str);

				str.Format(L"%.3f",	pGLAVNIY_CLASS->m_VectNonLinear[ix].fY);
				m_list.SetItemText(ix, 2, str);

				str.Format(L"%.3f",	pGLAVNIY_CLASS->m_VectNonLinear[ix].fh);
				m_list.SetItemText(ix, 3, str);

				str.Format(L"%.3f",	pGLAVNIY_CLASS->m_VectNonLinear[ix].fgammai);
				m_list.SetItemText(ix, 4, str);
			}

			m_list.RedrawItems(0, size-1);

			// �����
			str.Format(L"%.3f ��", pGLAVNIY_CLASS->fhmaxp);
			GetDlgItem(IDC_OTKLP)->SetWindowText(str);

			str.Format(L"%.3f ��", pGLAVNIY_CLASS->fhmaxm);
			GetDlgItem(IDC_OTKLM)->SetWindowText(str);

			str.Format(L"%.3f ��", pGLAVNIY_CLASS->fhmax);
			GetDlgItem(IDC_RAZMAX)->SetWindowText(str);

			str.Format(L"%.3f ��", pGLAVNIY_CLASS->fgamma);
			GetDlgItem(IDC_NELINEYNOST)->SetWindowText(str);

			GetDlgItem(IDC_SERIALNO)->SetWindowText(pGLAVNIY_CLASS->strNumberSensor);

			m_ReportDlg.Update();
		}
	}
}

void CEnterNLDlg::OnNlMenuSaveTxt()
{
	CString str; 

	CStdioFile m_stdFile;

	CString m_sFile(m_sDir + _T("\\") + pGLAVNIY_CLASS->strNumberSensor + _T(".txt")); 

	static TCHAR BASED_CODE szFilter[] = _T("Txt Files (*.txt)|*.txt|All Files (*.*)|*.*||");

	CFileDialog dlg(FALSE, _T("*.txt"), m_sFile, OFN_HIDEREADONLY, szFilter);						
	if (dlg.DoModal() == IDOK)
	{
		m_sFile = dlg.GetPathName();

		if (m_stdFile.Open(m_sFile, CFile::modeCreate | CFile::modeWrite, NULL)) {
			m_stdFile.WriteString(pGLAVNIY_CLASS->time_date);
			m_stdFile.WriteString(_T("\n"));
			m_stdFile.WriteString(_T("����� ��: "));
			m_stdFile.WriteString(pGLAVNIY_CLASS->strNumberSensor);
			m_stdFile.WriteString(_T("\n"));

			m_stdFile.WriteString(_T("\n"));
			str.Format(_T("������������ + ����������: %.3f ��\n"), pGLAVNIY_CLASS->fhmaxp);
			m_stdFile.WriteString(str);
			str.Format(_T("������������ - ����������: %.3f ��\n"), pGLAVNIY_CLASS->fhmaxm);
			m_stdFile.WriteString(str);
			str.Format(_T("������: %.3f ��\n"), pGLAVNIY_CLASS->fhmax);
			m_stdFile.WriteString(str);
			str.Format(_T("������������: %.3f %%\n"), pGLAVNIY_CLASS->fgamma);
			m_stdFile.WriteString(str);
			m_stdFile.WriteString(_T("\n"));

			str.Format(_T("��������� �������� X: %.3f ��\n"), pGLAVNIY_CLASS->fXStart);
			m_stdFile.WriteString(str);
			m_stdFile.WriteString(_T("\n"));

			str.Format(_T("���, ��\t��� ��, ��\t����������, ��\t����������, %%\n"), pGLAVNIY_CLASS->fXStart);
			m_stdFile.WriteString(str);
			for (int i=0; i<pGLAVNIY_CLASS->m_VectNonLinear.size(); i++) {
				str.Format(_T("%d\t%.3f\t\t%.3f\t\t%.3f\n"), i+(int)pGLAVNIY_CLASS->fXStart,
					pGLAVNIY_CLASS->m_VectNonLinear[i].fY,
					pGLAVNIY_CLASS->m_VectNonLinear[i].fh,
					pGLAVNIY_CLASS->m_VectNonLinear[i].fgammai);
				m_stdFile.WriteString(str);
			}

			m_stdFile.Close();
		}
	}
}

void CEnterNLDlg::OnNlMenuSaveRaw()
{
	CString m_sFile(m_sDir + _T("\\") + pGLAVNIY_CLASS->strNumberSensor + _T(".bin")); 

	NONLINEAR_STRUCT  temp;
	CFile m_File;
	int size;
	int ix;
	int lenstr;
	WCHAR *buf;

	static TCHAR BASED_CODE szFilter[] = _T("Raw Files (*.bin)|*.bin||");

	CFileDialog dlg(FALSE, _T("*.bin"), m_sFile, OFN_HIDEREADONLY, szFilter);						
	if (dlg.DoModal() == IDOK)
	{
		m_sFile = dlg.GetPathName();
		size = pGLAVNIY_CLASS->m_VectNonLinear.size();
		if (size == 0) return;

		if (m_File.Open(m_sFile, CFile::modeCreate | CFile::modeWrite, NULL)) {
			m_File.Write((void *)&size, sizeof(int));
			
			for (ix = 0; ix< size; ix++)
			{
				temp = pGLAVNIY_CLASS->m_VectNonLinear[ix];
				m_File.Write((void *)&temp, sizeof(NONLINEAR_STRUCT));
			}

			lenstr = pGLAVNIY_CLASS->strNumberSensor.GetLength()*sizeof(WCHAR);
			buf = pGLAVNIY_CLASS->strNumberSensor.GetBuffer();
			m_File.Write((void *)&lenstr, sizeof(int));
			m_File.Write((void *)buf, lenstr);
			pGLAVNIY_CLASS->strNumberSensor.ReleaseBuffer();

			m_File.Write((void *)&pGLAVNIY_CLASS->fXStart, sizeof(float));

			m_File.Close();
		}
	}

}

void CEnterNLDlg::OnNlMenuClear()
{
	Clear();
}

void CEnterNLDlg::OnNlMenuExit()
{
	Clear();
	OnOK();
}

void CEnterNLDlg::OnNlMenuGraph()
{
	m_ReportDlg.Update();
	m_ReportDlg.ShowWindow(SW_SHOW);
}

void CEnterNLDlg::OnNlMenuPrint()
{
}

void CEnterNLDlg::OnNlMenuMode()
{
	CModeInputDlg dlg;
	dlg.DoModal();

	mode = dlg.mode;
	if (mode == 0) 	((CEdit *)GetDlgItem(IDC_Y_EDIT))->SetReadOnly(FALSE);
	else ((CEdit *)GetDlgItem(IDC_Y_EDIT))->SetReadOnly(TRUE);
}

bool CEnterNLDlg::CheckString (CString *strText)
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
				AfxMessageBox(L"������ �����!");
				return false;
			}
		}
	}
	return true;
}
