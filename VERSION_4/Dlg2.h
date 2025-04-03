#pragma once

class CVERSION_4Dlg;

class CDlg2 : public CDialog
{
	DECLARE_DYNAMIC(CDlg2)

public:
	virtual ~CDlg2();

	CEdit m_Edit_1;	//частота вращения
	CEdit m_Edit_2;	//положение рейки
	CEdit m_Edit_3;	//температура масла
	CEdit m_Edit_4;	//обороты турбины
	CEdit m_Edit_5;	//давление наддува
	CEdit m_Edit_7;	//давление масла

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	// аказатель на родителя (главное окно)
	CVERSION_4Dlg* MAIN_WINDOW;
public:
	virtual BOOL PreTranslateMessage(MSG *pMsg);
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
public:
	virtual BOOL Create(UINT ID, CVERSION_4Dlg* pParentWnd);
};
