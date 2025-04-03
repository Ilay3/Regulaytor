#pragma once
#pragma comment(lib,"USART.lib")
#include <USART_window\USART\USART_Dlg.h>			//���� � ����� ��� �����
#pragma comment(lib,"GRAPHIC.lib")
#include "GRAPHIC_window\GRAPHIC\GRAPH_Dlg.h"		//���� � ����� ��� ������ ��������
#include <ERRORS\ERRORS.h>							//����� ����������� ������
#include <ProtocolOptions\ProtocolOptions.h>		//��� ��������� �����
#include <LogFile\LogFile.h>						//������ ���������
#include <SummCollection\SummCollection.h>			//������� ������������ ����������� �����
#include <ColorStatic\ColorStatic.h>				//������� �������

#include "Dlg1.h"
#include "Dlg2.h"
#include "Dlg3.h"
#include "Dlg4.h"
#include "Dlg5.h"
#include "EnterNLDlg.h"	
#include "EGUDlg.h"
#include "GlavniyClass.h"		//���������� ��������� �������
#include "my_functions.h"		//��������� �������������� �������
#include "Resource.h"			//�������� ��������������� ��������



// CVERSION_4Dlg dialog
class CVERSION_4Dlg : public CDialog
{
// Construction
public:
	CVERSION_4Dlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_VERSION_4_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	//���� �������
	CMenu menu;

	/****************************************************
	*	���� ����� �� RS232 ��� RS485
	*****************************************************/
public:
	CUSART_Dlg *pUSART_Dlg;	
	//�������� ���� ��� ����� �� USART
	afx_msg void ShowpUSART_Dlg(void);

	/***************************************************
	*	���� ��� ������ ��������
	****************************************************/
public:
	CGRAPH_Dlg *pGRAPH_Dlg;
	/****************************************************
	*	��������� � ������� �� �������� ��������� ��������
	*****************************************************/
	CGraphicOptions GraphicOptions;
protected:
	//�������, ������� ����� ���������������� ������
	bool InitGraph();
	//�������� ������ � ��������
	afx_msg void ShowGraph();

	/****************************************************
	*	���������� � ��������
	*****************************************************/
public:
	ERROR_Class m_Error;
	afx_msg LRESULT ShowError(WPARAM w, LPARAM l);
	// ������ ������ ��������� ���������
	//++++ CColorEdit m_Error_window;

	/****************************************************
	*	���������� �������� ������
	*****************************************************/
	GLAVNIY_CLASS_Struct GLAVNIY_CLASS;
	
	/****************************************************
	*	���������� ����� ����������� ���������
	****************************************************/
	LOG_FILE_Class LogFile;
	//���������� ������������ ������ ������� � ����������
	afx_msg void ShowLogFile();

	/****************************************************
	*	����������, � ������� �������� ��������� �������
	*	������-��������
	*****************************************************/
	POTOCOL_OPTIONS_Struct ProtocolOptions;

	/******************************************************
	*	��������� ������� � ���������
	*******************************************************/
	CDlg1 Dlg1;
	void ShowDlg1(void);	//���� �������������� �������
	
	CDlg2 Dlg2;
	void ShowDlg2(void);	//������ ��������� ���������
	
	CDlg3 Dlg3;
	void ShowDlg3(void);	//������ �������

	CDlg4 Dlg4;
	void ShowDlg4(void);	//������ �������������� ��������

	CDlg5 Dlg5;
	void ShowDlg5(void);	//��������� ������� �� ��������, COM ����

	CEnterNLDlg m_EnterDlg;

	CEGUDlg EGUDlg;

	//�������� ���� � ���������
	void ShowAboutBox(void);

	//�� �������� ��������� ��������� ���������� ��� �������
	virtual BOOL PreTranslateMessage(MSG *pMsg);
	//�������, �������� ����
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	//������� � ��������� �����
	CColorStatic m_LinkGood;
	void LINK_NOCONNECT()	
	{m_LinkGood.SetWindowText(_T("��� �����"));
	m_LinkGood.SetTextColor(RGB(255,0,0));}
	void LINK_PORTERRROR()	
	{m_LinkGood.SetWindowText(_T("������ �����"));
	m_LinkGood.SetTextColor(RGB(255,0,0));}
	void LINK_CONNECT()	
	{m_LinkGood.SetWindowText(_T("����� �����������"));
	m_LinkGood.SetTextColor(RGB(0,255,0));}
	void LINK_FORMATERROR()
	{m_LinkGood.SetWindowText(_T("�������� ������"));
	m_LinkGood.SetTextColor(RGB(0,0,0));}

	//��������� �������
	CColorStatic m_ParamDisel;
	CColorStatic m_DrvDisel;
	CColorStatic m_SystemState;

public:
	afx_msg void OnMenuExit();
	afx_msg void OnNonlinear();
	afx_msg void OnEguCtrl();
};

