#pragma once

class CVERSION_4Dlg;

class CDlg2 : public CDialog
{
	DECLARE_DYNAMIC(CDlg2)

public:
	virtual ~CDlg2();

	CEdit m_Edit_1;	//������� ��������
	CEdit m_Edit_2;	//��������� �����
	CEdit m_Edit_3;	//����������� �����
	CEdit m_Edit_4;	//������� �������
	CEdit m_Edit_5;	//�������� �������
	CEdit m_Edit_7;	//�������� �����

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	// ��������� �� �������� (������� ����)
	CVERSION_4Dlg* MAIN_WINDOW;
public:
	virtual BOOL PreTranslateMessage(MSG *pMsg);
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
public:
	virtual BOOL Create(UINT ID, CVERSION_4Dlg* pParentWnd);
};
