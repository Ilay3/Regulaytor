#pragma once

#include "GlavniyClass.h"		//переменные состояния объекта
#include "EnterNLDlg.h"	
#include "EGUReportDlg.h"	

// CNonLinearDlg dialog
class CNonLinearDlg : public CDialog
{
	DECLARE_DYNAMIC(CNonLinearDlg)

public:
//	CNonLinearDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNonLinearDlg();

// Dialog Data
//	enum { IDD = IDD_NONLINEAR_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:

	GLAVNIY_CLASS_Struct *pGLAVNIY_CLASS;


	CEnterNLDlg m_EnterDlg;
	CEGUReportDlg m_ReportDlg;


	CString m_sDir; 

	afx_msg void OnBnClickedInsertNLBtn();
	afx_msg void OnBnClickedResultNLBtn();
	afx_msg void OnBnClickedSaveNLBtn();
	afx_msg void OnBnClickedPrintNLBtn();
	virtual BOOL OnInitDialog();

	void Save();
	void Print();
};
