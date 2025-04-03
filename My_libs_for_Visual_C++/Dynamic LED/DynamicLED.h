/**************************************************************************
***************************************************************************
**		Это модуль красивых светодиодиков на экеране
***************************************************************************
**************************************************************************/

/*************************************************************************
*			ЛИСТ РЕГИСТРАЦИИ ИЗМЕНЕНИЙ
*	31.07.08  - поддерживается UNICODE
*************************************************************************/
#pragma once

class CDynamicLED : public CStatic
{
// Construction
public:
	CDynamicLED();

	//ОПИСАНИЕ СВЕТОВЫХ РЕШЕНИЙ СВЕТОДИОДА
	#define LC_RED		0x01
	#define LC_ORANGE	0x03
	#define LC_GREEN	0x04
	#define LC_BLUE		0x05
	#define LC_YELLOW	0x06

	//ОПИСАНИЕ ТИПОВ СВЕТОДИОДА
	#define LS_ROUND	0x01	//круглый
	#define LS_SQUARE	0x02	//квадратный
	
// Attributes
protected:
	
	BYTE m_nShape;			//тип светодиода

	BOOL m_bBright;
	BOOL m_bBlink;

	CWnd *m_pWnd;

	int m_nTimerInterval,m_nPrevTimerInterval;

	COLORREF m_cLight;				//цвет светодиодика, когда он включен
	COLORREF m_cDark;				//цвет светодиодика, когда он выключен

	UINT m_LedWidth;				//|
	UINT m_LedHeight;				//| Размеры светодиодика без надписи

	CString m_Caption;				// подпись светодиодика

	bool DrawLED(CDC* pDC);			// отрисовывает картинку светодиода
	bool DrawCaption(CDC* pDC);		// отрисовывает подпись светодиода

	COLORREF GetCurrentLED_Color();					//возвращает текущий цвет светодиодика
	bool DrawRoundLED(CDC* pDC, CRect LED_Rect);	//отрисовывает круглый светодиодик
	bool DrawSquareLED(CDC* pDC, CRect LED_Rect);	//отрисовывает квадратный светодиодик

	UINT_PTR m_SigTmrID;			//идентификатор мигающего таймера

	CRect GetCaptionRect();			//возвращает размеры подписи сетодиода
	void ResizeWindow();			//заного вычисляет и изменяет размеры окна светодиода
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
	//P.S. StopSignaling нету, т.к. используйте LedON и LedOFF, чтобы прекратить мигать
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



