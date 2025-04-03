#pragma once

#include "GlavniyClass.h"		//переменные состояния объекта


// CEGUDlg dialog

class CEGUDlg : public CDialog
{
	DECLARE_DYNAMIC(CEGUDlg)

public:
//	CEGUDlg(CWnd* pParent = NULL);   // standard constructor
	CEGUDlg();   // standard constructor
	virtual ~CEGUDlg();

// Dialog Data
//	enum { IDD = IDD_EGU_CTRL_DLG };

	GLAVNIY_CLASS_Struct *pGLAVNIY_CLASS;

	CListCtrl m_list;

	int flag;	// контроль введения H и K
	int mode;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	afx_msg void Clear();
	afx_msg void OnBnClickedH();
	afx_msg void OnBnClickedK();
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedClear();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMenuMode();

	bool CheckString (CString *strText);
};
