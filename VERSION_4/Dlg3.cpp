// Dlg3.cpp : implementation file
//

#include "stdafx.h"
#include "Dlg3.h"
#include "VERSION_4Dlg.h"


// CDlg3 dialog

IMPLEMENT_DYNAMIC(CDlg3, CDialog)

CDlg3::~CDlg3()
{
}

void CDlg3::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, edit1);
	DDX_Control(pDX, IDC_EDIT2, edit2);
	DDX_Control(pDX, IDC_EDIT3, edit3);
	DDX_Control(pDX, IDC_EDIT4, edit4);
	DDX_Control(pDX, IDC_EDIT5, edit5);
	DDX_Control(pDX, IDC_EDIT6, edit6);
	DDX_Control(pDX, IDC_EDIT7, edit7);
	DDX_Control(pDX, IDC_EDIT8, edit8);
	
}


BEGIN_MESSAGE_MAP(CDlg3, CDialog)

	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// переопределим для защиты
BOOL CDlg3::Create(UINT ID, CVERSION_4Dlg* pParentWnd)
{
	MAIN_WINDOW=pParentWnd;	//сохраним указатель на родителя

	return CDialog::Create(ID, pParentWnd);
}

BOOL CDlg3::OnInitDialog()
{
	CDialog::OnInitDialog();
	/********	СОЗДАДИМ КРАСИВЫЕ НАДПИСИ	*******************************/
	static CFont m_Font;
	m_Font.DeleteObject();  //на всякий случай
	m_Font.CreateFont		/*Выставим шрифт побольше*/
		(
			-10,                   // nHeight
			0,                         // nWidth
			0,                         // nEscapement
			0,                         // nOrientation
			FW_NORMAL,                 // nWeight
			FALSE,                     // bItalic
			FALSE,                     // bUnderline
			0,                         // cStrikeOut
			DEFAULT_CHARSET,           // nCharSet
			OUT_DEFAULT_PRECIS,        // nOutPrecision
			CLIP_DEFAULT_PRECIS,       // nClipPrecision
			DEFAULT_QUALITY,           // nQuality
			DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
			_T("Arial"));                 // lpszFacename

	/*GetDlgItem(IDC_EDIT5)->SetFont(&m_Font,true);
	GetDlgItem(IDC_EDIT6)->SetFont(&m_Font,true);
	GetDlgItem(IDC_EDIT7)->SetFont(&m_Font,true);
	GetDlgItem(IDC_EDIT8)->SetFont(&m_Font,true);*/
	
	static CFont m_Font2;
	m_Font.DeleteObject();  //на всякий случай
	m_Font.CreateFont		/*Выставим шрифт побольше*/
		(
			-14,                   // nHeight
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

	GetDlgItem(IDC_EDIT1)->SetFont(&m_Font,true);
	GetDlgItem(IDC_EDIT2)->SetFont(&m_Font,true);
	GetDlgItem(IDC_EDIT3)->SetFont(&m_Font,true);
	GetDlgItem(IDC_EDIT4)->SetFont(&m_Font,true);

	/*Начальные текстовые надписи*/
	GetDlgItem(IDC_EDIT5)->SetWindowText(_T("  Сигнализация по давлению масла "));
	GetDlgItem(IDC_EDIT6)->SetWindowText(_T("  Защита двигателя по давлению масла "));
	GetDlgItem(IDC_EDIT7)->SetWindowText(_T("  Защита по предельным оборотам "));
	GetDlgItem(IDC_EDIT8)->SetWindowText(_T("  Ограничение по наддуву "));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CDlg3::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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
	#define RedColor RGB(251,91,68)	//красненький цвет которым подсвечивается происшедшее событие
	
	if (CTLCOLOR_EDIT == nCtlColor)
	{
		/*ЭТО СТОЛБЕЦ НАДПИСЕЙ СПРАВА*/
		UINT id = pWnd->GetDlgCtrlID();
		if (id == IDC_EDIT1)
		{/*сигнализация по давлению масла*/
			//будем сигнализировать не только, когда есть сигнализация, но и когда есть защита
//			if(MAIN_WINDOW->GLAVNIY_CLASS.var_Oil_Prot==0x40 ||MAIN_WINDOW->GLAVNIY_CLASS.var_Oil_Prot==0xC0 || MAIN_WINDOW->GLAVNIY_CLASS.var_Oil_Prot==0x80)
			if(MAIN_WINDOW->GLAVNIY_CLASS.var_Oil_Prot & 0xC0)
			{//есть сигнализация
				pDC->SetTextColor			(RGB(000,000,000));	//цвет текста
				pDC->SetBkColor				(RGB(255,255,000));	//цвет за текстом 
				SetDCBrushColor(pDC->m_hDC,	 RGB(255,255,000));	//цвет эдитика
			}
			else if(MAIN_WINDOW->GLAVNIY_CLASS.var_Oil_Prot & 0x10)
			{//произошёл отказ отказ датчика положения рейки
				pDC->SetTextColor			(RGB(000,000,000));	//цвет текста
				pDC->SetBkColor				(RGB(200,200,000));	//цвет за текстом 
				SetDCBrushColor(pDC->m_hDC,	 RGB(200,200,000));	//цвет эдитика
			}
			else
			{//нет сигнализации
				pDC->SetTextColor			(RGB(000,000,000));	//цвет текста
				pDC->SetBkColor				(RGB(070,198,240));	//цвет за текстом 
				SetDCBrushColor(pDC->m_hDC,	 RGB(070,198,240));	//цвет эдитика
			}
			return (HBRUSH)::GetStockObject(DC_BRUSH);		//восстановим эти параметры
		
		}
		if ( id == IDC_EDIT2)
		{/*защита по давлению масла*/
			
			if(MAIN_WINDOW->GLAVNIY_CLASS.var_Oil_Prot==0xC0 || MAIN_WINDOW->GLAVNIY_CLASS.var_Oil_Prot==0x80)
			{//есть защита
				pDC->SetTextColor			(RGB(000,000,000));	//цвет текста
				pDC->SetBkColor				(RedColor);	//цвет за текстом 
				SetDCBrushColor(pDC->m_hDC,	 RedColor);	//цвет эдитика
			}
			else if(MAIN_WINDOW->GLAVNIY_CLASS.var_Oil_Prot & 0x20)
			{//произошёл отказ датчика давления масла
				pDC->SetTextColor			(RGB(000,000,000));	//цвет текста
				pDC->SetBkColor				(RGB(200,200,000));	//цвет за текстом 
				SetDCBrushColor(pDC->m_hDC,	 RGB(200,200,000));	//цвет эдитика
			}
			else
			{//нет защита
				pDC->SetTextColor			(RGB(000,000,000));	//цвет текста
				pDC->SetBkColor				(RGB(070,198,240));	//цвет за текстом 
				SetDCBrushColor(pDC->m_hDC,	 RGB(070,198,240));	//цвет эдитика
			}
			return (HBRUSH)::GetStockObject(DC_BRUSH);		//восстановим эти параметры
		}
		if ( id == IDC_EDIT3)
		{/*защита по предельным оборотам*/
			static bool flag=false;		//временный флаг

			if(MAIN_WINDOW->GLAVNIY_CLASS.var_Raznos)
			{//есть защита по предельным оборотам
				pDC->SetTextColor			(RGB(000,000,000));	//цвет текста
				pDC->SetBkColor				(RedColor);	//цвет за текстом 
				SetDCBrushColor(pDC->m_hDC,	 RedColor);	//цвет эдитика
			}
			else
			{//нет защиты по предельным оборотам
				pDC->SetTextColor			(RGB(000,000,000));	//цвет текста
				pDC->SetBkColor				(RGB(070,198,240));	//цвет за текстом 
				SetDCBrushColor(pDC->m_hDC,	 RGB(070,198,240));	//цвет эдитика
			}
			return (HBRUSH)::GetStockObject(DC_BRUSH);		//восстановим эти параметры
		}
		
		if ( id == IDC_EDIT4)
		{/*ограничение по наддуву*/
			if(MAIN_WINDOW->GLAVNIY_CLASS.var_Nadduv_Limit==0x00)
			{//нет защиты по наддуву
				pDC->SetTextColor			(RGB(000,000,000));	//цвет текста
				pDC->SetBkColor				(RGB(070,198,240));	//цвет за текстом 
				SetDCBrushColor(pDC->m_hDC,	 RGB(070,198,240));	//цвет эдитика
			}
			else if(MAIN_WINDOW->GLAVNIY_CLASS.var_Nadduv_Limit==0xFE)
			{//есть защита
				pDC->SetTextColor			(RGB(000,000,000));	//цвет текста
				pDC->SetBkColor				(RedColor);	//цвет за текстом 
				SetDCBrushColor(pDC->m_hDC,	 RedColor);	//цвет эдитика
			}
			else if(MAIN_WINDOW->GLAVNIY_CLASS.var_Nadduv_Limit==0xEE)
			{//вышел из строя датчик давления наддува
				pDC->SetTextColor			(RGB(000,000,000));	//цвет текста
				pDC->SetBkColor				(RGB(200,200,000));	//цвет за текстом 
				SetDCBrushColor(pDC->m_hDC,	 RGB(200,200,000));	//цвет эдитика
			}
			return (HBRUSH)::GetStockObject(DC_BRUSH);		//восстановим эти параметры
		}

		/*ЭТО СТОЛБЕЦ НАДПИСЕЙ СПРАВА*/
		if (id == IDC_EDIT5)
		{/*сигнализация по давлению масла*/
			if(MAIN_WINDOW->GLAVNIY_CLASS.var_Oil_Prot & 0x40)
			{//есть защита по предельным оборотам
				pDC->SetTextColor			(RGB(000,000,000));	//цвет текста
				pDC->SetBkColor				(RedColor);	//цвет за текстом 
				SetDCBrushColor(pDC->m_hDC,	 RedColor);	//цвет эдитика
			}
			else if(MAIN_WINDOW->GLAVNIY_CLASS.var_Oil_Prot & 0x10)
			{//произошёл отказ датчика положения рейки
				pDC->SetTextColor			(RGB(000,000,000));	//цвет текста
				pDC->SetBkColor				(RedColor);	//цвет за текстом 
				SetDCBrushColor(pDC->m_hDC,	 RedColor);	//цвет эдитика
			}
			else
			{//нет защита
				pDC->SetTextColor			(RGB(000,000,000));	//цвет текста
				pDC->SetBkColor				(RGB(146,228,174));	//цвет за текстом 
				SetDCBrushColor(pDC->m_hDC,	 RGB(146,228,174));	//цвет эдитика
			}
			return (HBRUSH)::GetStockObject(DC_BRUSH);		//восстановим эти параметры
		}
		if ( id == IDC_EDIT6)
		{/*защита по давленич масла*/
//			if(MAIN_WINDOW->GLAVNIY_CLASS.var_Oil_Prot==0xC0 || MAIN_WINDOW->GLAVNIY_CLASS.var_Oil_Prot==0x80)
			if(MAIN_WINDOW->GLAVNIY_CLASS.var_Oil_Prot & 0xC0)
			{//есть защита по предельным оборотам
				pDC->SetTextColor			(RGB(000,000,000));	//цвет текста
				pDC->SetBkColor				(RedColor);	//цвет за текстом 
				SetDCBrushColor(pDC->m_hDC,	 RedColor);	//цвет эдитика
			}
			else if(MAIN_WINDOW->GLAVNIY_CLASS.var_Oil_Prot & 0x20)
			{//произошёл отказ датчика давления масла
				pDC->SetTextColor			(RGB(000,000,000));	//цвет текста
				pDC->SetBkColor				(RedColor);	//цвет за текстом 
				SetDCBrushColor(pDC->m_hDC,	 RedColor);	//цвет эдитика
			}
			else
			{//нет защита
				pDC->SetTextColor			(RGB(000,000,000));	//цвет текста
				pDC->SetBkColor				(RGB(146,228,174));	//цвет за текстом 
				SetDCBrushColor(pDC->m_hDC,	 RGB(146,228,174));	//цвет эдитика
			}
			return (HBRUSH)::GetStockObject(DC_BRUSH);		//восстановим эти параметры
		}
		if ( id == IDC_EDIT7)
		{/*защита по предельным оборотам*/
			if(MAIN_WINDOW->GLAVNIY_CLASS.var_Raznos)
			{//есть защита по предельным оборотам
				pDC->SetTextColor			(RGB(000,000,000));	//цвет текста
				pDC->SetBkColor				(RedColor);	//цвет за текстом 
				SetDCBrushColor(pDC->m_hDC,	 RedColor);	//цвет эдитика
			}
			else
			{//нет защита
				pDC->SetTextColor			(RGB(000,000,000));	//цвет текста
				pDC->SetBkColor				(RGB(146,228,174));	//цвет за текстом 
				SetDCBrushColor(pDC->m_hDC,	 RGB(146,228,174));	//цвет эдитика
			}
			return (HBRUSH)::GetStockObject(DC_BRUSH);		//восстановим эти параметры
		}
		if ( id == IDC_EDIT8)
		{/*ограничение по наддуву*/
			if(MAIN_WINDOW->GLAVNIY_CLASS.var_Nadduv_Limit==0xFE || MAIN_WINDOW->GLAVNIY_CLASS.var_Nadduv_Limit==0xEE)
			{//есть защита по предельным оборотам, или сломался датчик
				pDC->SetTextColor			(RGB(000,000,000));	//цвет текста
				pDC->SetBkColor				(RedColor);	//цвет за текстом 
				SetDCBrushColor(pDC->m_hDC,	 RedColor);	//цвет эдитика
			}
			else
			{//нет никаких событий в этой строке
				pDC->SetTextColor			(RGB(000,000,000));	//цвет текста
				pDC->SetBkColor				(RGB(146,228,174));	//цвет за текстом 
				SetDCBrushColor(pDC->m_hDC,	 RGB(146,228,174));	//цвет эдитика
			}
			return (HBRUSH)::GetStockObject(DC_BRUSH);		//восстановим эти параметры
		}
	}

	if (CTLCOLOR_STATIC == nCtlColor)
	{
		UINT id = pWnd->GetDlgCtrlID();
		
	}

	if (CTLCOLOR_DLG == nCtlColor)
	{
		SetDCBrushColor(pDC->m_hDC,RGB(5,196,211));	//цвет заднего плана
		return (HBRUSH)::GetStockObject(DC_BRUSH);		//восстановим эти параметры
	}

 //   pDC->SetBkColor(RGB(127,127,127));


	return hbr;
}

/***************************	PreTranslateMessage(MSG *pMsg)	***********************************
*	Лично я использую эту функцию только, чтобы отловить горячие клавиши
***************************************************************************************************/
BOOL CDlg3::PreTranslateMessage(MSG *pMsg)
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

