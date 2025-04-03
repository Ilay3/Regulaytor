// Dlg2.cpp : implementation file
//

#include "stdafx.h"
#include "VERSION_4Dlg.h"
#include "Dlg2.h"

// CDlg2 dialog

IMPLEMENT_DYNAMIC(CDlg2, CDialog)
CDlg2::~CDlg2()
{
}

void CDlg2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_1, m_Edit_1);
	DDX_Control(pDX, IDC_EDIT_2, m_Edit_2);
	DDX_Control(pDX, IDC_EDIT_3, m_Edit_3);
	DDX_Control(pDX, IDC_EDIT_4, m_Edit_4);
	DDX_Control(pDX, IDC_EDIT_5, m_Edit_5);
	DDX_Control(pDX, IDC_EDIT_7, m_Edit_7);
}


BEGIN_MESSAGE_MAP(CDlg2, CDialog)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// переопределим для защиты
BOOL CDlg2::Create(UINT ID, CVERSION_4Dlg* pParentWnd)
{
	MAIN_WINDOW=pParentWnd;	//сохраним указатель на родителя

	return CDialog::Create(ID, pParentWnd);
}

BOOL CDlg2::OnInitDialog()
{
	CDialog::OnInitDialog();

	static CFont m_Font;
	m_Font.DeleteObject();  //на всякий случай
	m_Font.CreateFont		/*Выставим шрифт побольше*/
		(
			-16,                   // nHeight
			0,                         // nWidth
			0,                         // nEscapement
			0,                         // nOrientation
			FW_BOLD,                 // nWeight
			FALSE,                     // bItalic
			FALSE,                     // bUnderline
			0,                         // cStrikeOut
			DEFAULT_CHARSET,           // nCharSet
			OUT_DEFAULT_PRECIS,        // nOutPrecision
			CLIP_DEFAULT_PRECIS,       // nClipPrecision
			DEFAULT_QUALITY,           // nQuality
			DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
			_T("Times New Roman"));                 // lpszFacename

	GetDlgItem(IDC_EDIT_1)->SetFont(&m_Font,true);
	GetDlgItem(IDC_EDIT_2)->SetFont(&m_Font,true);
	GetDlgItem(IDC_EDIT_3)->SetFont(&m_Font,true);
	GetDlgItem(IDC_EDIT_4)->SetFont(&m_Font,true);
	GetDlgItem(IDC_EDIT_5)->SetFont(&m_Font,true);
	GetDlgItem(IDC_EDIT_7)->SetFont(&m_Font,true);
	

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CDlg2::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	/*
	CTLCOLOR_BTN    	Кнопка
	CTLCOLOR_DLG    	Диалог
	CTLCOLOR_EDIT   	Элемент редактирования
	CTLCOLOR_LISTBOX 	Список  
	CTLCOLOR_MSGBOX   	Окно сообщений
	CTLCOLOR_SCROLLBAR  Скролинг
	CTLCOLOR_STATIC   	Статический текст
	*/
	if (CTLCOLOR_BTN == nCtlColor)
	{
	}

	if (CTLCOLOR_EDIT == nCtlColor)
	{
		UINT id = pWnd->GetDlgCtrlID();
		
	//	pDC->SetTextColor			(RGB(255,000,000));	//цвет текста
	//	pDC->SetBkColor				(RGB(070,198,240));	//цвет за текстом 
	//	SetDCBrushColor(pDC->m_hDC,	 RGB(070,198,240));	//цвет эдитика
	//	return (HBRUSH)::GetStockObject(DC_BRUSH);		//восстановим эти параметры
	}

	if (CTLCOLOR_STATIC == nCtlColor)
	{
		UINT id = pWnd->GetDlgCtrlID();

//		pDC->SetTextColor			(RGB(255,000,000));	//цвет текста
//		pDC->SetBkColor				(RGB(070,198,240));	//цвет за текстом 
//		SetDCBrushColor(pDC->m_hDC,	 RGB(070,198,240));	//цвет эдитика
//		return (HBRUSH)::GetStockObject(DC_BRUSH);		//восстановим эти параметры
		
	}

	if (CTLCOLOR_DLG == nCtlColor)
	{
		SetDCBrushColor(pDC->m_hDC,RGB(5,196,211));	//цвет заднего плана
		return (HBRUSH)::GetStockObject(DC_BRUSH);		//восстановим эти параметры
	}

	return hbr;
}

/***************************	PreTranslateMessage(MSG *pMsg)	***********************************
*	Лично я использую эту функцию только, чтобы отловить горячие клавиши
***************************************************************************************************/
BOOL CDlg2::PreTranslateMessage(MSG *pMsg)
{
	int k=0;
	switch(pMsg->message)
	{
	case WM_KEYDOWN:
		switch(pMsg->wParam)
		{
			//Следующие сообщения нужны не этому, окну, а всей программе в целом,
			//поэтому отправим его главному окну
			case VK_ESCAPE:
			case VK_RETURN:
			case '1':
			case VK_NUMPAD1:
			case '2':
			case VK_NUMPAD2:
			case '3':
			case VK_NUMPAD3:
			case '4':
			case VK_NUMPAD4:
			case '5':
			case VK_NUMPAD5:
			case '6':
			case VK_NUMPAD6:
			case '7':
			case VK_NUMPAD7:
			case '8':
			case VK_NUMPAD8:
			case '9':
			case VK_NUMPAD9:
			case '0':
			case VK_NUMPAD0:
			case 'W':
			case 'S':
			case 'A':
			case 'D':
		
			return MAIN_WINDOW->PreTranslateMessage(pMsg);
			break;
		}
		break;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}



