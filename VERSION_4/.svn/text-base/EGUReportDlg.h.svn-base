#pragma once
#include "LIMGraphCtrl.h"
#include "GlavniyClass.h"		//переменные состояния объекта

// CEGUReportDlg dialog

class CEGUReportDlg : public CDialog
{
	DECLARE_DYNAMIC(CEGUReportDlg)

public:
//	CEGUReportDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEGUReportDlg();

// Dialog Data
//	enum { IDD = IDD_EGU_REPORT_DLG };

	GLAVNIY_CLASS_Struct *pGLAVNIY_CLASS;
	CLIMGraphCtrl  m_pLIMGraph;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	void Update();
};
