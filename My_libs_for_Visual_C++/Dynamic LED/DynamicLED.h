/**************************************************************************
***************************************************************************
**		��� ������ �������� ������������� �� �������
***************************************************************************
**************************************************************************/

/*************************************************************************
*			���� ����������� ���������
*	31.07.08  - �������������� UNICODE
*************************************************************************/
#pragma once

class CDynamicLED : public CStatic
{
// Construction
public:
	CDynamicLED();

	//�������� �������� ������� ����������
	#define LC_RED		0x01
	#define LC_ORANGE	0x03
	#define LC_GREEN	0x04
	#define LC_BLUE		0x05
	#define LC_YELLOW	0x06

	//�������� ����� ����������
	#define LS_ROUND	0x01	//�������
	#define LS_SQUARE	0x02	//����������
	
// Attributes
protected:
	
	BYTE m_nShape;			//��� ����������

	BOOL m_bBright;
	BOOL m_bBlink;

	CWnd *m_pWnd;

	int m_nTimerInterval,m_nPrevTimerInterval;

	COLORREF m_cLight;				//���� ������������, ����� �� �������
	COLORREF m_cDark;				//���� ������������, ����� �� ��������

	UINT m_LedWidth;				//|
	UINT m_LedHeight;				//| ������� ������������ ��� �������

	CString m_Caption;				// ������� ������������

	bool DrawLED(CDC* pDC);			// ������������ �������� ����������
	bool DrawCaption(CDC* pDC);		// ������������ ������� ����������

	COLORREF GetCurrentLED_Color();					//���������� ������� ���� ������������
	bool DrawRoundLED(CDC* pDC, CRect LED_Rect);	//������������ ������� �����������
	bool DrawSquareLED(CDC* pDC, CRect LED_Rect);	//������������ ���������� �����������

	UINT_PTR m_SigTmrID;			//������������� ��������� �������

	CRect GetCaptionRect();			//���������� ������� ������� ���������
	void ResizeWindow();			//������ ��������� � �������� ������� ���� ����������
public:

	void LedON();
	void LedOFF();
	void UpdateLED(CDC *dc); 

	bool SetLED(BYTE ColorScheme, BYTE Shape, CString Caption, UINT LedWidth, UINT LedHeight);
	bool SetColorScheme(BYTE ColorScheme, bool invalidate=true);
	bool SetShape(BYTE Shape, bool invalidate=true);
	bool SetLedWidth(UINT LedWidth);
	bool SetLedHeight(UINT LedHeight);
	bool SetLedSize(UINT LedWidth,UINT LedHeight, bool invalidate=true);
	bool SetCaption(CString Caption, bool invalidate=true);
	CString GetCaption();
	bool StartSignaling(UINT Period_ms);
	//P.S. StopSignaling ����, �.�. ����������� LedON � LedOFF, ����� ���������� ������
	bool IsON();
	bool IsOFF();
	bool IsSignaling();
	
	virtual ~CDynamicLED();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDynamicLED)
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};



