#pragma once
#include <types_visual_c++.h>		//���� � ����� ������

#include "Resource.h"
#include "afxwin.h"
class CConfigDlg : public CDialog
{
public:
	CConfigDlg(CWnd* pParent = NULL);   // standard constructor
	CConfigDlg(CWnd* pParent, DCB dcb, INT8U PortNum);

	enum { IDD = IDD_CONFIGDIALOG };
	CString	m_strBaudRate;
	CString	m_strDataBits;
	char	m_Parity;
	CString	m_strStopBits;
	BOOL	m_CommBreakDetected;
	BOOL	m_CommCTSDetected;
	BOOL	m_CommDSRDetected;
	BOOL	m_CommERRDetected;
	BOOL	m_CommRingDetected;
	BOOL	m_CommRLSDDetected;
	BOOL	m_CommRxchar;
	BOOL	m_CommRxcharFlag;
	BOOL	m_CommTXEmpty;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:

	//����� ���������� ���������� � ���������� ����� �� ������ ������ ����
	DCB		m_dcb;

	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
public:

	//���� ��� ������ ���� ����� ������� ����� �������� ����������������� �����
	INT8U m_PortNum;
	// ����� ����������������� �����
	CComboBox m_ComNumList;
};
