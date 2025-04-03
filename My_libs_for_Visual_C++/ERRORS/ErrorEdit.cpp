// ErrorEdit.cpp : implementation file
//

#include "stdafx.h"
#include "ErrorEdit.h"


// CErrorEdit

IMPLEMENT_DYNAMIC(CErrorEdit, CWnd)

CErrorEdit::CErrorEdit()
{

}

CErrorEdit::~CErrorEdit()
{
}


BEGIN_MESSAGE_MAP(CErrorEdit, CWnd)
	ON_WM_TIMER()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_CREATE()
	ON_WM_NCCREATE()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

/*BOOL CErrorEdit::OnInitDialog()
{
	CDialog::OnInitDialog();
	ModifyStyle(0,ES_MULTILINE | ES_READONLY);
	return TRUE;
}*/

/*************************    SetError    ********************************************
*		Показывает в окне новое сообщение. Я решил оставить передачу текста и приоритета
*	напрямую, чтобы была возможность игнорировать ERROR_Class.
*************************************************************************************/
bool CErrorEdit::SetError(CString error, INT8U priority)
{
	SetWindowText(error);
	switch (priority)	//обрежем часть в флагами
	{
		/* Далее идет так, первые 4 бита (всего 16 значений) отвечаю собсно за приоритет. 
		Причем
			0 - ну ОХРЕНЕННО хороший результат
			7 - простое мимолетное (но успешное) сообщение
			8 - простое мимолетное (но предупреждающее) сообщение
			15 - полный кранты
		*/
		case 0:SetTextColor(RGB(50, 255, 50)); break;
		case 1:SetTextColor(RGB(140, 240, 150)); break;
		case 2:SetTextColor(RGB(140, 140, 240)); break;
		case 3:SetTextColor(RGB(154, 162, 228)); break;
		case 4:SetTextColor(RGB(74, 78, 189)); break;
		case 5:SetTextColor(RGB(102, 91, 172)); break;
		case 6:SetTextColor(RGB(36, 108, 51)); break;
		case 7:
		case 8:SetTextColor(RGB(83, 55, 89)); break;
		case 9:SetTextColor(RGB(91, 45, 100)); break;
		case 10:SetTextColor(RGB(107, 37, 62)); break;
		case 11:SetTextColor(RGB(117, 28, 56)); break;
		case 12:SetTextColor(RGB(126, 18, 53)); break;
		case 13:SetTextColor(RGB(223, 17, 58)); break;
		case 14:SetTextColor(RGB(235, 5, 11)); break;
		case 15:SetTextColor(RGB(255, 50, 50)); break;
		default:AfxMessageBox(_TEXT("Чет не заладилось с важностью ошибки..."));
	}
	SetBackColor(RGB(255,190,190));
	SetTimer(NEW_ERR_SIGNAL_ID,200,NULL);
	return true;
}

/**************************   OnTimer(UINT_PTR nIDEvent)   **********************************
*		Функция вызывается при срабатывании таймера. Таймер используется для мигания 
*	при появлении очередного сообщения.
********************************************************************************************/
void CErrorEdit::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent==NEW_ERR_SIGNAL_ID)
	{//истекло время подсвечивания новой ошибка
		KillTimer(NEW_ERR_SIGNAL_ID);
		SetBackColor(GetSysColor(COLOR_3DFACE));
	}
}

/**********************    ClearErrorWindow    **********************************************
*		Если старая ошибка устарела - самое время очистить окно состояния программы
********************************************************************************************/
void CErrorEdit::ClearErrorWindow()
{
	SetWindowText(_TEXT(""));
}

/********************   PreSubclassWindow   ***********************************
*		Эта функция вызывается при создании, и в ней я буду менять стиль нашего 
*	эдитика, чтобы не делать это каждый раз в Properties.
******************************************************************************/
void CErrorEdit::PreSubclassWindow()
{
	SendMessage((UINT) EM_SETREADONLY,TRUE,0);
	/*не работает*/ModifyStyle(0,ES_MULTILINE | ES_CENTER);

	CColorEdit::PreSubclassWindow();
	
}

BOOL CErrorEdit::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	// TODO: Add your specialized code here and/or call the base class

	return CColorEdit::Create(dwStyle, rect, pParentWnd, nID);
}
