#pragma once
#include "afxwin.h"
#include <ColorButton\ColorButton.h>
#include "MyEdit.h"

// CDlg1 dialog
class CVERSION_4Dlg;
class CDlg1 : public CDialog
{
	DECLARE_DYNAMIC(CDlg1)

public:
	virtual ~CDlg1();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	

	DECLARE_MESSAGE_MAP()
public:
	// ��������� �� �������� (������� ����)
	CVERSION_4Dlg* MAIN_WINDOW;
	
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL PreTranslateMessage(MSG *pMsg);


	//������� ������� ��������
	CEdit m_UST_Freq_Rot;
	afx_msg void OnEnChangeUST_Freq_Rot();

	//������� �����
	CMyEdit m_UST_Reyka;
	//������ ���� ������� ������
	CColorButton m_UST_Reyka_Set;
	afx_msg void OnReykaSet();
	void ReykaCancel();
	bool m_bReyka_Edit;
	float m_reyka_temp;

	//������� �������
	CEdit m_UST_Position;

	#define BGColorDis		RGB(200,184,223)  //���� ������, ����� ���� ����� ��� �� ������
	#define BGColorEn		RGB(251,91,68)		//���� ������, ����� ���� ����� �������
	#define BGColorUST_Set	RGB(230,230,0)		//������� ����������, �� ���������� ��� �� ���������� ���������

	//������ ���� ������� ������
	CColorButton m_UST_Stop;
	//������ ���� ������� ������
	CColorButton m_UST_Run;
	//������ ��������� ����
	CColorButton m_UST_Holost;
	//������ ������ ��� ���������
	CColorButton m_UST_Nagr;
	//������ ����������
	CColorButton m_UST_Bux;
	bool m_bBux;
	//�������� ����������, ������� ����� ������ ����
	//CColorButton CBRun, CBStop, CBHolost, CBNagr;

	afx_msg void OnStop();		//������� �� ����
	afx_msg void OnRun();		//������� �� ������
	afx_msg void OnNagr();		//������� �� ������ ��� ���������
	afx_msg void OnHolost();	//������� �� ������ �� �������� ����

	//����������, ���������������� ������ � ������ ����
	CColorButton r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14,r15, *prev_regime;

	afx_msg void OnBnClickedPosition_1();
	afx_msg void OnBnClickedPosition_2();
	afx_msg void OnBnClickedPosition_3();
	afx_msg void OnBnClickedPosition_4();
	afx_msg void OnBnClickedPosition_5();
	afx_msg void OnBnClickedPosition_6();
	afx_msg void OnBnClickedPosition_7();
	afx_msg void OnBnClickedPosition_8();
	afx_msg void OnBnClickedPosition_9();
	afx_msg void OnBnClickedPosition_10();
	afx_msg void OnBnClickedPosition_11();
	afx_msg void OnBnClickedPosition_12();
	afx_msg void OnBnClickedPosition_13();
	afx_msg void OnBnClickedPosition_14();
	afx_msg void OnBnClickedPosition_15();

	afx_msg void OnBnClickedBux();

	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnClose();
public:
	virtual BOOL Create(UINT ID, CVERSION_4Dlg*);
};
