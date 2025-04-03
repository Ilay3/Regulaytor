/**************************************************************************
***************************************************************************
**		Мне надоело каждый раз переносить одни и те же функции отображения ошибки,
**	поэтому я решил вынести всё это в отдельный класс.
***************************************************************************
**************************************************************************/

/*************************************************************************
*					ЛИСТ РЕГИСТРАЦИИ ИЗМЕНЕНИЙ
*	12 ноября 2008 - добавил функцию PreSubclassWindow
*************************************************************************/
#pragma once
#include "Types_Visual_c++.h"		//файл с моими типами
#include "ColorEdit\ColorEdit.h"	//класс цветных эдитиков

// CErrorEdit

#pragma once
class CErrorEdit : public CColorEdit
{
	DECLARE_DYNAMIC(CErrorEdit)

public:
	CErrorEdit();
	virtual ~CErrorEdit();

	//показывает в окне новое сообщение
	bool SetError(CString error, INT8U priority);
	//очищает окно сообщений
	void ClearErrorWindow();
protected:
	DECLARE_MESSAGE_MAP()
	//таймер по сигнализации прихода новой ошибки
	const static UINT_PTR	NEW_ERR_SIGNAL_ID=0x199;
	//срабатывания таймера
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual void PreSubclassWindow();
public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
};


