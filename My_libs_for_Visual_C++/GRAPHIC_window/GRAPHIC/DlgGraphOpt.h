#pragma once
#include "afxwin.h"
#include "resource.h"

//������������� ����� �������� ����
class CGRAPH_Dlg;

class CDlgGraphOpt : public CDialog
{
	DECLARE_DYNAMIC(CDlgGraphOpt)

public:
	CDlgGraphOpt(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgGraphOpt();

	// ��������� �� �������� (������� ���� �������)
	CGRAPH_Dlg* GRAPH_WINDOW;

// Dialog Data
	enum { IDD = IDD_GRAPH_OPT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(UINT ID, CGRAPH_Dlg* pParentWnd);
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
public:
	virtual BOOL OnInitDialog();
public:
	// �������� ����� �������
	CComboBox m_ColorScheme;
	// ��� �������
	CEdit m_GraphName;
public:
	afx_msg void OnCbnSelendokColorScheme();
public:
	afx_msg void OnEnChangeGraphName();
public:
	// �������� ���������� ����� �� �������������� ���
	short m_HorTickCount;
	// �������� ���������� ������ �� ������������ ���
	short m_VerTickCount;
	CSpinButtonCtrl m_HorTicksSpin;	//���� �� ������������
	CSpinButtonCtrl m_VerTicksSpin;	//���� �� ������������
	afx_msg void OnDeltaPosHorTickCount(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaPosVerTickCount(NMHDR *pNMHDR, LRESULT *pResult);
	CEdit m_edtX_Title;
	afx_msg void OnEnChangeXTitle();
};
