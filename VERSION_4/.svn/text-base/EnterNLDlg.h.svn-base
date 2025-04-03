#pragma once

// CEnterNLDlg dialog
#include "GlavniyClass.h"		//переменные состояния объекта
#include "EGUReportDlg.h"	

class CEnterNLDlg : public CDialog
{
	DECLARE_DYNAMIC(CEnterNLDlg)

public:
//	CEnterNLDlg(CWnd* pParent = NULL);   // standard constructor
	CEnterNLDlg();   // standard constructor
	virtual ~CEnterNLDlg();

// Dialog Data
//	enum { IDD = IDD_INSERT_NL_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:

	CEGUReportDlg m_ReportDlg;

	GLAVNIY_CLASS_Struct *pGLAVNIY_CLASS;

	int mode;
	int flag;	// контроль введения № и dX

	CListCtrl m_list;
	CString m_sDir; 

	afx_msg void Clear();

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedNumber();
	afx_msg void OnBnClickedXStart();
	afx_msg void OnBnClickedAdd();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnNlMenuOpen();
	afx_msg void OnNlMenuSaveTxt();
	afx_msg void OnNlMenuSaveRaw();
	afx_msg void OnNlMenuClear();
	afx_msg void OnNlMenuExit();
	afx_msg void OnNlMenuGraph();
	afx_msg void OnNlMenuPrint();
	afx_msg void OnNlMenuMode();

	bool CheckString (CString *strText);

};
