/*************************************************
���� ������ ����� ��� �������������� ������ ��� ������ ��������
��������� � ��������. �� ����������� ���� �� �����:
 1. ������� ����������� �����
 2. ���������� ������ ������������ ����� ���������
**************************************************/
#pragma once

//������������� ����� �������� ����
class CVERSION_4Dlg;

class CDlg4 : public CDialog
{
	DECLARE_DYNAMIC(CDlg4)

public:
	virtual ~CDlg4();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	CVERSION_4Dlg* MAIN_WINDOW;	//��������� �� ������� ����

public:
	virtual BOOL Create(UINT ID, CVERSION_4Dlg* pParentWnd);
public:
	afx_msg void OnBnClickedPacketatack();
};
