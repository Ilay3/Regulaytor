/*************************************************
Ётот диалог нужен дл€ удовлетворини€ вс€ких там разных капризов
 ирь€нова и кампании. Ќа сегодн€шний день он умеет:
 1. ѕортить контрольную сумму
 2. ¬ыпаливать подр€д определенное число сообщений
**************************************************/
#pragma once

//«адекларируем класс главного окна
class CVERSION_4Dlg;

class CDlg4 : public CDialog
{
	DECLARE_DYNAMIC(CDlg4)

public:
	virtual ~CDlg4();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	CVERSION_4Dlg* MAIN_WINDOW;	//указатель на главное окно

public:
	virtual BOOL Create(UINT ID, CVERSION_4Dlg* pParentWnd);
public:
	afx_msg void OnBnClickedPacketatack();
};
