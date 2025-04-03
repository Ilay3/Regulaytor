/*******************************************************************************************8
*********************************************************************************************
**	Это окно отвечает за прокрутку графика на экране
********************************************************************************************
********************************************************************************************/

/********************	ЛИСТ РЕГИСТРАЦИИ ИЗМЕНЕНИЙ	******************************************
*	22.01.2007 - исправлены некоторые warningi
**********************************************************************************************/
#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "resource.h"


class CGRAPH_Dlg;

class CDlgScroll : public CDialog
{
	DECLARE_DYNAMIC(CDlgScroll)

public:
	CDlgScroll(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgScroll();

// Dialog Data
	enum { IDD = IDD_SCROLL_BACK };

	// переопределим для защиты
	void CreateMAIN(UINT str, CGRAPH_Dlg * parent);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// указатель на родителя (главное окно)
	CGRAPH_Dlg* GRAPH_WINDOW;
	//шрифт для эдитика со временем
	CFont m_Font;

	//показывает текущее время прокрутки
	void ShowTime(float time);
	//время, до которого можно прокручивать
	float m_EndTime;

	float m_Scale;	//Масштабный коэффициент времени прокрутки, это значение 
					//обычно кратное (10), на которое будет умножаться одна секунда
					//при прокручивании. Например, если m_Scale=0.01, то шаг прокрутки
					//будет 0.01 с.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	// ползунок
	CSliderCtrl m_Slider;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult);
	// кнопочки вверх/вниз
	CSpinButtonCtrl m_Spin;
	// показывает текущее время останова
	CEdit m_Current;
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnClose();
	afx_msg void OnEnChangeCurrent();
};
