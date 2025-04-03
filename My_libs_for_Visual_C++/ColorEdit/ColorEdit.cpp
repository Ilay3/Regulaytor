/*
	ReadOnlyEdit.cpp
	By: Kevin Bond
	kevinbond@gmail.com
	www.mechdesk.ath.cx

	Use and distribute freely.  
	Please don't remove my name and don't take credit for my work.	

	Send comments, questions or bugs to kevinbond@gmail.com 
	or visit my site www.mechdesk.ath.cx
*/

/*Instructions:	This class is derived from the CEdit class.  When creating
				the edit box you want to be white and read-only create it using
				CColorEdit instead of CEdit.  Make sure you remember to set 
				the dwStyle to ES_READONLY.
				
*/					

#include "stdafx.h"
#include "ColorEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorEdit

CColorEdit::CColorEdit()
{
	//цвет текста по умолчанию
	m_crText = RGB(0,0,0);
	//цвет внутри эдитика
	m_crBackGnd=GetSysColor(COLOR_3DFACE);
	m_brBackGnd.CreateSolidBrush(m_crBackGnd);
}

CColorEdit::~CColorEdit()
{
	//delete brush
	if (m_brBackGnd.GetSafeHandle())
       m_brBackGnd.DeleteObject();
}


BEGIN_MESSAGE_MAP(CColorEdit, CEdit)
	//{{AFX_MSG_MAP(CColorEdit)
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorEdit message handlers



HBRUSH CColorEdit::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	//цвет текста
	pDC->SetTextColor(m_crText);
	//цвет эдитики
	pDC->SetBkColor(m_crBackGnd);
	//цвет за текстом
	pDC->SetDCBrushColor(m_crBackGnd);	 

	//return the brush used for background this sets control background
	//return m_brBackGnd;
	return (HBRUSH)::GetStockObject(DC_BRUSH);
}


void CColorEdit::SetBackColor(COLORREF rgb)
{
	//set background color ref (used for text's background)
	m_crBackGnd = rgb;
	
	//free brush
	if (m_brBackGnd.GetSafeHandle())
       m_brBackGnd.DeleteObject();
	//set brush to new color
	m_brBackGnd.CreateSolidBrush(rgb);
	
	//redraw
	Invalidate(TRUE);
}


void CColorEdit::SetTextColor(COLORREF rgb)
{
	//set text color ref
	m_crText = rgb;

	//redraw
	Invalidate(TRUE);
}
