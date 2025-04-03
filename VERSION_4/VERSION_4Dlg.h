#pragma once
#pragma comment(lib,"USART.lib")
#include <USART_window\USART\USART_Dlg.h>			//окно и класс для связи
#pragma comment(lib,"GRAPHIC.lib")
#include "GRAPHIC_window\GRAPHIC\GRAPH_Dlg.h"		//окно и класс для показа графиков
#include <ERRORS\ERRORS.h>							//класс регистрации ошибок
#include <ProtocolOptions\ProtocolOptions.h>		//для настройки связи
#include <LogFile\LogFile.h>						//журнал сообщений
#include <SummCollection\SummCollection.h>			//функции накапливания контрольной суммы
#include <ColorStatic\ColorStatic.h>				//Цветные надписи

#include "Dlg1.h"
#include "Dlg2.h"
#include "Dlg3.h"
#include "Dlg4.h"
#include "Dlg5.h"
#include "EnterNLDlg.h"	
#include "EGUDlg.h"
#include "GlavniyClass.h"		//переменные состояния объекта
#include "my_functions.h"		//различные дополнительные функции
#include "Resource.h"			//названия идентификаторов ресурсов



// CVERSION_4Dlg dialog
class CVERSION_4Dlg : public CDialog
{
// Construction
public:
	CVERSION_4Dlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_VERSION_4_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	//наша менюшка
	CMenu menu;

	/****************************************************
	*	Окно связи по RS232 или RS485
	*****************************************************/
public:
	CUSART_Dlg *pUSART_Dlg;	
	//показать окно для связи по USART
	afx_msg void ShowpUSART_Dlg(void);

	/***************************************************
	*	Окно для показа графиков
	****************************************************/
public:
	CGRAPH_Dlg *pGRAPH_Dlg;
	/****************************************************
	*	Структура в которую мы запихнем настройки графиков
	*****************************************************/
	CGraphicOptions GraphicOptions;
protected:
	//функция, которая будет инициализировать график
	bool InitGraph();
	//Показать диалог с графиком
	afx_msg void ShowGraph();

	/****************************************************
	*	Переменная с ошибками
	*****************************************************/
public:
	ERROR_Class m_Error;
	afx_msg LRESULT ShowError(WPARAM w, LPARAM l);
	// эдитик строки состояния программы
	//++++ CColorEdit m_Error_window;

	/****************************************************
	*	Переменная главного класса
	*****************************************************/
	GLAVNIY_CLASS_Struct GLAVNIY_CLASS;
	
	/****************************************************
	*	Переменная файла регистрации сообщений
	****************************************************/
	LOG_FILE_Class LogFile;
	//показывает пользователю журнал событий в блокнотике
	afx_msg void ShowLogFile();

	/****************************************************
	*	Переменная, в которую выведены настройки режимов
	*	приемо-передачи
	*****************************************************/
	POTOCOL_OPTIONS_Struct ProtocolOptions;

	/******************************************************
	*	Различные диалоги в программе
	*******************************************************/
	CDlg1 Dlg1;
	void ShowDlg1(void);	//окно перещелкивания позиций
	
	CDlg2 Dlg2;
	void ShowDlg2(void);	//панель состояния двигателя
	
	CDlg3 Dlg3;
	void ShowDlg3(void);	//панель событий

	CDlg4 Dlg4;
	void ShowDlg4(void);	//диалог дополнительных действий

	CDlg5 Dlg5;
	void ShowDlg5(void);	//настройка уставок по позициям, COM порт

	CEnterNLDlg m_EnterDlg;

	CEGUDlg EGUDlg;

	//показать окно о программе
	void ShowAboutBox(void);

	//до основной обработки сообщения вызывается эта функция
	virtual BOOL PreTranslateMessage(MSG *pMsg);
	//функция, узнающая цвет
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	//надпись о состоянии связи
	CColorStatic m_LinkGood;
	void LINK_NOCONNECT()	
	{m_LinkGood.SetWindowText(_T("Нет связи"));
	m_LinkGood.SetTextColor(RGB(255,0,0));}
	void LINK_PORTERRROR()	
	{m_LinkGood.SetWindowText(_T("Ошибка порта"));
	m_LinkGood.SetTextColor(RGB(255,0,0));}
	void LINK_CONNECT()	
	{m_LinkGood.SetWindowText(_T("Связь установлена"));
	m_LinkGood.SetTextColor(RGB(0,255,0));}
	void LINK_FORMATERROR()
	{m_LinkGood.SetWindowText(_T("Неверный формат"));
	m_LinkGood.SetTextColor(RGB(0,0,0));}

	//остальные надписи
	CColorStatic m_ParamDisel;
	CColorStatic m_DrvDisel;
	CColorStatic m_SystemState;

public:
	afx_msg void OnMenuExit();
	afx_msg void OnNonlinear();
	afx_msg void OnEguCtrl();
};

