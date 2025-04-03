/******************************************************************************************************
*******************************************************************************************************
**		��� ����� ���� �������� ������-��������
*******************************************************************************************************
********************************************************************************************************/

/********************** ���� ����������� ��������� ****************************************************
*		11.09.2006 - ������ ��� ����������� ����� �� �� ��������� ��������������� ���������� ���������
*******************************************************************************************************/

#pragma once
#include "afxwin.h"
#include <SerialPort\SerialPort.h>				//����� ������ � ���������������� ������
#include "ConfigDlg.h"

class CUSART_Dlg;

class CFlagsDlg : public CDialog
{
	DECLARE_DYNAMIC(CFlagsDlg)

public:
	CFlagsDlg(CWnd* pParent);   // standard constructor
	virtual ~CFlagsDlg();
	int b_flag;
	CUSART_Dlg* pUSART_Dlg;
// Dialog Data
	enum { IDD = IDD_FLAGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	afx_msg void OnConfigbutton();

	
public:
	virtual BOOL OnInitDialog();
	//��������� ���������� ���������
	bool			o_emulation1;
	bool			o_nepr1;				//���������� �� ����������
	CString			o_inMesFile1;			//���� � ����� ������� ���������� ���������
	bool			o_verify1;				//������������ �� ��� ���������
	bool			o_OFF1;					//���������� �� ��������� �� �����
	CString			o_outMesFile1;			//��� ����� �� �������� ������������ ���������
	bool			o_HexDisplayOutMessage1;//���������� �� ����������������� �������� � ���� ��������
	bool			o_HexDisplayInMessage1;	//���������� �� ����������������� ����� ��� ������
	bool			o_LogOutMessage1;		//����� �� ����������� ������������ ��������� � �����
	bool			o_LogInMessage1;		//����� �� ����������� ����������� ��������� � �����
	bool			o_LogTimeMessages1;		//����� �� ����������� ����� �������� � ������ ���������
	INT32U			o_Period1;				//������ �������� ��������� ��� ����������� ��������
	INT16U			o_EmulSpeed1;			//������ �������� �������� ��� ��������
	
	bool		o_DoHistoryFile1;			//����������� �� �� � ��������� �����
	//����� ��������� ���������� ���������

	//������� ��� ������� ��� ��������� ��������
	afx_msg void OnEmulatioN();
	afx_msg void OnVerify();
	afx_msg void OnInMesFile();
	afx_msg void OnNepr();
	afx_msg void OnOFF();
	afx_msg void OnEnChangeOutfile();
	afx_msg void OnBnClickedHexout();
	afx_msg void OnBnClickedHexin();
	afx_msg void OnBnClickedLogout();
	afx_msg void OnEnChangePeriod();

	// ������� � ��������� �����
	CStatic m_SCOM;
	
	
	
public:
	afx_msg void OnBnClickedLogin();
public:
	afx_msg void OnBnClickedLogtime();
	afx_msg void OnEnChangeEmulSpeed();
};
