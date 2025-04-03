#pragma once
#include "resource.h"

//�������������� ����� ����� �������� ���� �������, ����� �� �������� � ������������ �������
class CGRAPH_Dlg;

class CDlgCodomain : public CDialog
{
	DECLARE_DYNAMIC(CDlgCodomain)

public:
	CDlgCodomain(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgCodomain();

// Dialog Data
	enum { IDD = IDD_CODOMAIN_DLG };

	// ������������� ��� ������
	void CreateMAIN(UINT str, CGRAPH_Dlg * parent);
	
	// ��������� �� �������� (������� ���� �������)
	CGRAPH_Dlg* GRAPH_WINDOW;

	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//���������� ��������� �������� �������
	CEdit CUpLimit;
	//���������� ��������� �������� �������
	CEdit CDownLimit;
	//���������� ��������� ������������� �������
	CEdit CVisibleTime;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeUplimit();
	afx_msg void OnEnChangeDownlimit();
	afx_msg void OnEnChangeVisibleTime();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
