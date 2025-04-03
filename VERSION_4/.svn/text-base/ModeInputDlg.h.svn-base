#pragma once


// CModeInputDlg dialog

class CModeInputDlg : public CDialog
{
	DECLARE_DYNAMIC(CModeInputDlg)

public:
	CModeInputDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CModeInputDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

	int mode;   // 0-ручной/1-авто

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void OnCancel();
public:
	afx_msg void OnBnClickedManualBtn();
	afx_msg void OnBnClickedAvtoBtn();
};
