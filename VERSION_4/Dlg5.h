#pragma once

class CVERSION_4Dlg;

class CDlg5 : public CDialog
{
	DECLARE_DYNAMIC(CDlg5)

public:
	virtual ~CDlg5();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

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

	afx_msg void OnBnClickedSAVE();

	afx_msg void OnBnClickedType7();
	afx_msg void OnBnClickedType8();

	afx_msg void OnBnClickedSetSpeed_1();	//������� ��� �3-02, �3-06, �3-10
	afx_msg void OnBnClickedSetSpeed_2();	//������� ��� �3-12

	afx_msg void OnClose();
	afx_msg void OnOK();
	afx_msg void OnCANCEL();


	// ���������� ������ � ���������� �������
	CComboBox m_ComNumList;

public:
	
	// ��������� �� �������� (������� ����)
	CVERSION_4Dlg* MAIN_WINDOW;

	virtual BOOL OnInitDialog();

	//����, ����� ������� ���� ������� �� �����
	UINT m_iPred_pos;

	//��������� ������������ �������� ������� �, ���� ���������, ���������� �� � Positions_data
	bool CheckAllRight();
	//��������� ��������� � ����, ��� ��������� �� �� ����
	bool Storing(bool save_or_load);

	//�������������� ����� �������� �������
	void EditPositionSpeed(UINT idc_pos);

	//�������������� ���� �������
	void EditAllPositionSpeed(int new_speed_pos[15]);


	/*���������, ������� �������� � ���� ������� �������� �� ��������*/
	struct Positions_data_struct
	{
		int Pos[15];
	}Positions_data;

	virtual BOOL Create(UINT ID, CVERSION_4Dlg* pParentWnd);
	afx_msg void OnBnClickedEnableGraph();
};

