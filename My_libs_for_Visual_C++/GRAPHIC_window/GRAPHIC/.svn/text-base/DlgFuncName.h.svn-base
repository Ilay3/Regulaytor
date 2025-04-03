#pragma once
#include "resource.h"

class CDlgFuncName : public CDialog
{
	DECLARE_DYNAMIC(CDlgFuncName)

public:
	CDlgFuncName(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgFuncName();

// Dialog Data
	enum { IDD = IDD_FUNC_NAME_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
public:
	CString m_FuncName;		//по большому счету используется как темповая переменная
	DECLARE_MESSAGE_MAP()
public:
	virtual INT_PTR DoModal(CString OldName);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

protected:
	virtual void OnOK();
};
