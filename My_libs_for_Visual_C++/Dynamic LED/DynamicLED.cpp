// DynamicLED.cpp : implementation file
//

#include "stdafx.h"
//#include "DynLED.h"
#include "DynamicLED.h"

#define ID_TIMER_START 1001

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDynamicLED

CDynamicLED::CDynamicLED()
{
	// Initialize the variables
	m_bBlink = TRUE;
	m_bBright = FALSE;
	m_nTimerInterval = 0;
	m_SigTmrID=0;					//������ ��� �� ������
	SetShape(LS_ROUND,false);		//�� ��������� �� �������
	SetColorScheme(LC_GREEN,false);	//� �������
	SetLedSize(15,15,false);		//������������ ������ �������
	SetCaption(_TEXT("No caption"),false);	//��� ��������
}

CDynamicLED::~CDynamicLED()
{
}

BEGIN_MESSAGE_MAP(CDynamicLED, CStatic)
	//{{AFX_MSG_MAP(CDynamicLED)
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDynamicLED message handlers

bool CDynamicLED::SetLED(BYTE ColorScheme, BYTE Shape, CString Caption, UINT LedWidth, UINT LedHeight)
{
	if(!SetLedSize(LedWidth,LedHeight)) return false;
	if(!SetShape(Shape))				return false;
	if(!SetColorScheme(ColorScheme))	return false;
	if(!SetCaption(Caption))			return false;

	return true;
}

void CDynamicLED::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	if(IsWindowVisible())
		UpdateLED(&dc);
}

void CDynamicLED::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent==m_SigTmrID)
	{//�������� ������, ������� ������ �����������
		m_bBright=!m_bBright;	//������� �������� ����������
		Invalidate();			//������� ��������
	}

	CWnd::OnTimer(nIDEvent);
}

/**********************************************************************************
*		��������� ����� ���������� �� ����������, ����������� � �������� ���������.
*	��� ��� ������� ��������� ��� � ��� ���������.
**********************************************************************************/
void CDynamicLED::LedON()
{
	//�� �������� ��� ��, ����� �� �������������� ��������� ������ ���, 
	//���� �� ��� ��� � ���� ���������
	if(m_SigTmrID!=0)
	{//�� ����� � ��������� � ��������� �������
		KillTimer(m_SigTmrID);
		m_SigTmrID=0;
		m_bBright = TRUE;	//������ � ������
		Invalidate();		//����� ������������
	}
	else if(m_bBright==FALSE)
	{//�� ����� � ��� ��������
		m_bBright = TRUE;	//������ � ������
		Invalidate();		//����� ������������
	}
}

void CDynamicLED::LedOFF()
{
	//�� �������� ��� ��, ����� �� �������������� ��������� ������ ���, 
	//���� �� ��� ��� � ���� ���������
	if(m_SigTmrID!=0)
	{//�� ����� � ��������� � ��������� �������
		KillTimer(m_SigTmrID);
		m_SigTmrID=0;
		m_bBright = FALSE;	//������ � ��������
		Invalidate();		//����� ������������
	}
	else if(m_bBright==TRUE)
	{//�� ����� � ��� �������
		m_bBright = FALSE;	//������ � ��������
		Invalidate();		//����� ������������
	}
}

/********************     StartSignaling    *******************************
*		�������� ������ ������������� � �������� Period_ms. ����� �����
*	�� ����� ��������� ���������� ������� LedON ��� LedOFF.
**************************************************************************/
bool CDynamicLED::StartSignaling(UINT Period_ms)
{
	if(Period_ms==0) return false;
	if(m_SigTmrID==0)
	{//�� ����� � �� �����
		m_SigTmrID=SetTimer(1, Period_ms, NULL);
	}
	return true;
}

/*************************************************************************
*		��� ������� ������ ����������, � ����� ��������� ������ ���������
*************************************************************************/
bool CDynamicLED::IsON()
{
	if(m_SigTmrID!=0)	return false;	//��������� ������
	if(!m_bBright)		return false;	//��������� �������
	return true;						//����� �����
}
bool CDynamicLED::IsOFF()
{
	if(m_SigTmrID!=0)	return false;	//��������� ������
	if(m_bBright)		return false;	//��������� �����
	return true;						//����� �������
}
bool CDynamicLED::IsSignaling()
{
	if(m_SigTmrID==0)	return false;	//������ �� �������
	return true;						//���� �������� ������, ������ ������
}

/**********************    GetCurrentLED_Color()    ***********************
*		������ ���������� ������� ���� ����������, � ����������� �� ����,
*	������� �� ������ ��� ��������.
***************************************************************************/
COLORREF CDynamicLED::GetCurrentLED_Color()
{
	if(m_bBright==TRUE)
	{//��������� ������
		return m_cLight;
	}
	else
	{//��������� �������
		return m_cDark;
	}
	return RGB(0,0,0);
}

/*****************************************************************************
*		������� ������������ ������ ���� �������������
*****************************************************************************/
//������������ ������� �����������
bool CDynamicLED::DrawRoundLED(CDC* pDC, CRect LED_Rect)	
{
	ASSERT(pDC);

	CPen lPen;
	CPen *lpOldPen;
	CBrush lBrush;
	CBrush *lpOldBrush;

	// Draw the actual colour of the LED
	lPen.CreatePen(PS_SOLID,1,GetCurrentLED_Color());
	lBrush.CreateSolidBrush(GetCurrentLED_Color());

	lpOldPen=pDC->SelectObject(&lPen);
	lpOldBrush=pDC->SelectObject(&lBrush);
	pDC->Ellipse(LED_Rect);

	// Draw a thick dark gray coloured circle
	lPen.DeleteObject();
	lPen.CreatePen(PS_SOLID,1,RGB(128,128,128));
	pDC->SelectObject(&lPen);
	
	pDC->Ellipse(LED_Rect);

	// Draw a thin light gray coloured circle
/*	lPen.DeleteObject();
	lPen.CreatePen(PS_SOLID,1,RGB(192,192,192));
	pDC->SelectObject(&lPen);
	pDC->Ellipse(LED_Rect);
*/
	// Draw a white arc at the bottom
	lPen.DeleteObject();
	lPen.CreatePen(PS_SOLID,1,RGB(255,255,255));
	pDC->SelectObject(&lPen);

	// The arc function is just to add a 3D effect for the control
/*	CPoint ptStart,ptEnd;
	ptStart = CPoint(LED_Rect.Width()/2,LED_Rect.bottom);
	ptEnd	= CPoint(LED_Rect.right,LED_Rect.top);

	pDC->MoveTo(ptStart);
	pDC->Arc(LED_Rect,ptStart,ptEnd);

	// ����� ���� �� ����������
	lBrush.DeleteObject();
	if(m_bBright)	//���� ��������� �����, �� ������ ���������
		lBrush.CreateSolidBrush(RGB(255,255,255));
	else
		lBrush.CreateSolidBrush(RGB(180,180,180));
	pDC->SelectObject(&lBrush);

	CRect rect;
	rect.left=LED_Rect.left+LED_Rect.Width()/3;
	rect.top=LED_Rect.top+LED_Rect.Height()/3;
	rect.right=rect.left+3;
	rect.bottom=rect.top+4;
	pDC->Ellipse(rect);
*/
	//����������� ������ ��������
	pDC->SelectObject(lpOldPen);
	pDC->SelectObject(lpOldBrush);

	return true;
}

//������������ ���������� �����������
bool CDynamicLED::DrawSquareLED(CDC* pDC, CRect rect)
{
	ASSERT(pDC);

	/*// If you have decided that your LED is going to look square in shape, then

		// Draw the actual rectangle
		pDC->FillRect(rcClient,&m_BrushCurrent);

		// The  code below gives a 3D look to the control. It does nothing more

		// Draw the dark gray lines
		CPen Pen;
		Pen.CreatePen(0,1,RGB(128,128,128));
		pDC->SelectObject(&Pen);

		pDC->MoveTo(rcClient.left,rcClient.bottom);
		pDC->LineTo(rcClient.left,rcClient.top);
		pDC->LineTo(rcClient.right,rcClient.top);
		
		Pen.DeleteObject();

		// Draw the light gray lines
		Pen.CreatePen(0,1,RGB(192,192,192));
		pDC->SelectObject(&Pen);

		pDC->MoveTo(rcClient.right,rcClient.top);
		pDC->LineTo(rcClient.right,rcClient.bottom);
		pDC->LineTo(rcClient.left,rcClient.bottom);	*/

		//����������� ������ ��������
	//pDC->SelectObject(lpOldPen);
	//pDC->SelectObject(lpOldBrush);

	return true;
}

/*******************    DrawLED    ****************************************
*		������������ �������� ����������.
**************************************************************************/
bool CDynamicLED::DrawLED(CDC* pDC)
{
	//������������ ���� � ����� ������� ����
	CRect lLED_Rect(0,0,m_LedWidth,m_LedHeight);
	
	// If the round shape has been selected for the control 
	if(m_nShape==LS_ROUND)
	{//����� �������� ������� ���������
		if(!DrawRoundLED(pDC, lLED_Rect)) return false;
	}
	else if(m_nShape==LS_SQUARE)
	{//����� �������� ���������� ���������
		if(!DrawSquareLED(pDC, lLED_Rect)) return false;
	}
	else
	{//����������� ����� ����������
		return false;
	}

	return true;
}

/************************     DrawCaption     ****************************
*		������������ ������� ����������
**************************************************************************/
bool CDynamicLED::DrawCaption(CDC* pDC)
{
	CRect lCapRect=GetCaptionRect();

	CPen lPen(PS_SOLID,1,RGB(0,0,0));
	CPen *lpOldPen=pDC->SelectObject(&lPen);
	pDC->SetBkColor(GetSysColor(COLOR_3DFACE));
	pDC->DrawText(GetCaption(),&lCapRect,0);

	//��������������
	pDC->SelectObject(lpOldPen);
	return true;
}

void CDynamicLED::UpdateLED(CDC *pDc) 
{
	DrawLED(pDc);
	DrawCaption(pDc);
}

/********************    SetColorScheme    *************************************
*		������������� �������� ������� ���������� �� ������ �����������.
*******************************************************************************/
bool CDynamicLED::SetColorScheme(BYTE ColorScheme, bool invalidate)
{
	switch(ColorScheme)
	{
	case(LC_RED):
		m_cLight=RGB(250,0,0);
		m_cDark=RGB(150,0,0);
		break;
	case(LC_ORANGE):
		m_cLight=RGB(212,106,42);
		m_cDark=RGB(175,97,79);
		break;
	case(LC_GREEN):
		m_cLight=RGB(0,250,0);
		m_cDark=RGB(10,70,10);
		break;
	case(LC_BLUE):
		m_cLight=RGB(0,0,250);
		m_cDark=RGB(0,0,150);
		break;
	case(LC_YELLOW):
		m_cLight=RGB(240,240,0);
		m_cDark=RGB(150,150,0);
		break;
	default:
		return false;	//������������ �������� �������
	}
	if(invalidate)
	{//����� ������������
		Invalidate();	
	}
	return true;
}

/**********************    SetShape    ****************************************
*		������������� ����� ������������ 
******************************************************************************/
bool CDynamicLED::SetShape(BYTE Shape, bool invalidate)
{
	if(	Shape==LS_ROUND || 
		Shape==LS_SQUARE)
	{//��������� ������� ���
		m_nShape=Shape;
		if(invalidate)
		{//����� ������������
			ResizeWindow();		//����� ����������� �������
			Invalidate();	
		}
		return true;
	}
	return false;		//����� ����� � �� ����
}

/*****************************************************************************
*		������� ���������� ������� ����������
*****************************************************************************/
bool CDynamicLED::SetLedWidth(UINT LedWidth)
{
	if(LedWidth==0) return false;
	m_LedWidth=LedWidth;
	ResizeWindow();
	return true;
}
bool CDynamicLED::SetLedHeight(UINT LedHeight)
{
	if(LedHeight==0) return false;
	m_LedHeight=LedHeight;
	ResizeWindow();
	return true;
}

bool CDynamicLED::SetLedSize(UINT LedWidth,UINT LedHeight, bool invalidate)
{
	if(LedWidth==0) return false;
	if(LedHeight==0) return false;
	m_LedWidth=LedWidth;
	m_LedHeight=LedHeight;
	if(invalidate)
	{//����� ������������
		ResizeWindow();
	}
	return true;
}

/****************************************************************************
*		�������, ���������� � �������� ������������
****************************************************************************/
bool CDynamicLED::SetCaption(CString Caption, bool invalidate)
{
	//������� ������� ������ �������, ����� �� ����� �� ��� �����
	if(invalidate)
	{//����� ������������
		CRect rect=GetCaptionRect();
		GetDC()->FillSolidRect(rect,GetSysColor(COLOR_3DFACE));
	}

	m_Caption=Caption;

	//� ������ ����� �������� �����
	if(invalidate)
	{//����� ������������
		ResizeWindow();
		Invalidate();	
	}
	return true;
}

CString CDynamicLED::GetCaption()
{
	return m_Caption;
}

//���������� ������� ������� ���������
CRect CDynamicLED::GetCaptionRect()
{
	CRect lCapRect;
	//������� ������
	lCapRect.left=m_LedWidth+2;
	lCapRect.top=0;
	CDC *pDC=GetDC();
	if(m_Caption!="")
	{//���� ������� ������������
		pDC->DrawText(GetCaption(),&lCapRect,DT_CALCRECT);
	}
	else
	{//����� ��� ������� ������ DrawText ������
		lCapRect.right=lCapRect.left;
		lCapRect.bottom=m_LedHeight;
	}
	
	if(m_LedHeight>(UINT)lCapRect.bottom)
	{//����� �� ����� �� ���������
		lCapRect.top=(m_LedHeight-lCapRect.bottom)/2;
	}
	return lCapRect;
}

/***************************    ResizeWindow     *****************************
*		��� ��������� ������� ��� �������� ����������, ��� ����� ������
*	��������� ������� ����, ����� ���� ��� �����������.
******************************************************************************/
void CDynamicLED::ResizeWindow()
{
	CRect rect=GetCaptionRect();
	SetWindowPos(NULL,0,0,rect.right,m_LedHeight,SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOREDRAW);
}