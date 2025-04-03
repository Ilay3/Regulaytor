#pragma once
#include "resource.h"

//продекларируем здесь класс главного окна графика, чтобы не возиться с подключением графика
class CGRAPH_Dlg;

class CDlgCodomain : public CDialog
{
	DECLARE_DYNAMIC(CDlgCodomain)

public:
	CDlgCodomain(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgCodomain();

// Dialog Data
	enum { IDD = IDD_CODOMAIN_DLG };

	// переопределим для защиты
	void CreateMAIN(UINT str, CGRAPH_Dlg * parent);
	
	// указатель на родителя (главное окно графика)
	CGRAPH_Dlg* GRAPH_WINDOW;

	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//переменная источника верхнего предела
	CEdit CUpLimit;
	//переменная источника верхнего предела
	CEdit CDownLimit;
	//переменная источника отображаемого времени
	CEdit CVisibleTime;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeUplimit();
	afx_msg void OnEnChangeDownlimit();
	afx_msg void OnEnChangeVisibleTime();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
