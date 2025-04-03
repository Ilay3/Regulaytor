// MyEdit.cpp : implementation file
//

#include "stdafx.h"
#include "MyEdit.h"


// CMyEdit

IMPLEMENT_DYNAMIC(CMyEdit, CEdit)

CMyEdit::CMyEdit()
{

}

CMyEdit::~CMyEdit()
{
}


BEGIN_MESSAGE_MAP(CMyEdit, CEdit)
	ON_WM_CHAR()
END_MESSAGE_MAP()



// CMyEdit message handlers



void CMyEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch(nChar)
	{ /* validate */
//		case _T('+'):
//		case _T('-'):
		case _T('.'):
//		case _T('E'):
//		case _T('e'):
		case _T('0'):
		case _T('1'):
		case _T('2'):
		case _T('3'):
		case _T('4'):
		case _T('5'):
		case _T('6'):
		case _T('7'):
		case _T('8'):
		case _T('9'):
		case _T('\b'):
			break;
		default:
			MessageBeep(0);
		return;
	} /* validate */

	CEdit::OnChar(nChar, nRepCnt, nFlags);



	CString s;
	CString s_new = _T("");
	enum {S0, IPART, FPART};
	WORD state = S0;
	int f_cnt = 0;	// счетчик дробной части

	CEdit::GetWindowText(s);

	for (int i=0; (i<s.GetLength()) && (f_cnt < 2);i++)		// дробная часть .00 (f_cnt < 2) 
	{
		TCHAR ch = s[i];
		switch (MAKELONG(state, ch)) 
		{
			case MAKELONG(S0, _T('0')):
			case MAKELONG(S0, _T('1')):
			case MAKELONG(S0, _T('2')):
			case MAKELONG(S0, _T('3')):
			case MAKELONG(S0, _T('4')):
			case MAKELONG(S0, _T('5')):
			case MAKELONG(S0, _T('6')):
			case MAKELONG(S0, _T('7')):
			case MAKELONG(S0, _T('8')):
			case MAKELONG(S0, _T('9')):
				state = IPART;
				s_new.AppendChar(ch);
				continue;

			case MAKELONG(IPART, _T('0')):
			case MAKELONG(IPART, _T('1')):
			case MAKELONG(IPART, _T('2')):
			case MAKELONG(IPART, _T('3')):
			case MAKELONG(IPART, _T('4')):
			case MAKELONG(IPART, _T('5')):
			case MAKELONG(IPART, _T('6')):
			case MAKELONG(IPART, _T('7')):
			case MAKELONG(IPART, _T('8')):
			case MAKELONG(IPART, _T('9')):
				s_new.AppendChar(ch);
				continue;

			case MAKELONG(IPART, _T('.')):
				state = FPART;
				s_new.AppendChar(ch);
				continue;

			case MAKELONG(FPART, _T('0')):
			case MAKELONG(FPART, _T('1')):
			case MAKELONG(FPART, _T('2')):
			case MAKELONG(FPART, _T('3')):
			case MAKELONG(FPART, _T('4')):
			case MAKELONG(FPART, _T('5')):
			case MAKELONG(FPART, _T('6')):
			case MAKELONG(FPART, _T('7')):
			case MAKELONG(FPART, _T('8')):
			case MAKELONG(FPART, _T('9')):
				s_new.AppendChar(ch);
				f_cnt++;
				continue;
			default:
				break;
		}
	}
	CEdit::SetWindowText(s_new);
	CEdit::SetSel(-1);
}
