#pragma once

// CDlg3 dialog

class CVERSION_4Dlg;

class CDlg3 : public CDialog
{
	DECLARE_DYNAMIC(CDlg3)

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
	// аказатель на родителя (главное окно)
	CVERSION_4Dlg* MAIN_WINDOW;
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG *pMsg);

	//присвоим им переменные, чтобы корректно обновлять информацию из главного класса
	CEdit edit1;
	CEdit edit2;
	CEdit edit3;
	CEdit edit4;
	CEdit edit5;
	CEdit edit6;
	CEdit edit7;
	CEdit edit8;

	virtual ~CDlg3();

public:
	virtual BOOL Create(UINT ID, CVERSION_4Dlg* pParentWnd);
};
