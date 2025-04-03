/***************************************************************************************
*****************************************************************************************
**	Этот модуль делает кнопки цветными, при этом кнопке ОБЯЗАТЕЛЬНО нужно задать свойство
**	owner draw - true (или SetStyle c флагом BS_OWNERDRAW), в противном случае отрисовка 
**  не произведется
****************************************************************************************
*****************************************************************************************/

/*********************	ЛИСТ РЕГИСТРАЦИИ ИЗМЕНЕНИЙ	*************************************
*	25.10.2006 - 
******************************************************************************************/
#pragma once
#include <tchar.h>	//для подержки UNICODE

class CColorButton : public CButton
{
DECLARE_DYNAMIC(CColorButton)
public:
	CColorButton(); 
	virtual ~CColorButton(); 

	BOOL Attach(const UINT nID, CWnd* pParent, 
		const COLORREF BGColor = RGB(192, 192, 192),		// gray button
		const COLORREF FGColor = RGB(1, 1, 1),				// black text 
		const COLORREF DisabledColor = RGB(128, 128, 128),	// dark gray disabled text
		const UINT nBevel = 2
	);
	//выставляет цвет текста на кнопке
	void SetFGColor(const COLORREF fgcolor);
	//выставляет цвет кнопки
	void SetBGColor(const COLORREF bgcolor);
	//выставляет цвет кнопки в неактивном состоянии
	void SetDisabledColor(const COLORREF disabledcolor);
	//установить скос
	void SetBevel(UINT bevel);
	
	//ВСЁ ТОЖЕ САМОЕ, ТОЛЬКО ВОЗВРАТИТЬ
	COLORREF GetFGColor();
	COLORREF GetBGColor(); 
    COLORREF GetDisabledColor(); 
	UINT GetBevel(); 
	
protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	void DrawFrame(CDC *DC, CRect R, int Inset);
	void DrawFilledRect(CDC *DC, CRect R, COLORREF color);
	void DrawLine(CDC *DC, CRect EndPoints, COLORREF color);
	void DrawLine(CDC *DC, long left, long top, long right, long bottom, COLORREF color);
	void DrawButtonText(CDC *DC, CRect R, const TCHAR *Buf, COLORREF TextColor);

	

private:
	COLORREF m_fg, m_bg, m_disabled;
	UINT m_bevel;

};

