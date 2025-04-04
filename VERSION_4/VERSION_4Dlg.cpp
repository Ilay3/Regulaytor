// VERSION_4Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "VERSION_4.h"
#include "VERSION_4Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVERSION_4Dlg dialog




CVERSION_4Dlg::CVERSION_4Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVERSION_4Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVERSION_4Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CVERSION_4Dlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_LOGFILE, &CVERSION_4Dlg::ShowLogFile)
	//++++ON_COMMAND(ID_SHOWGRAPH, &CVERSION_4Dlg::ShowGraph)
	ON_MESSAGE(MSG_ER_ERROR,ShowError)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_COMMAND(ID_MENU_DLG4,ShowDlg4)
	ON_COMMAND(ID_MENU_DLG5,ShowDlg5)
	ON_COMMAND(ID_MENU_USART,ShowpUSART_Dlg)
	ON_COMMAND(ID_MENU_ABOUTBOX,ShowAboutBox)
	ON_COMMAND(ID_MENU_EXIT, &CVERSION_4Dlg::OnMenuExit)
	ON_COMMAND(ID_MENU_GRAPH, &CVERSION_4Dlg::ShowGraph)
	ON_COMMAND(ID_NONLINEAR, &CVERSION_4Dlg::OnNonlinear)
	ON_COMMAND(ID_EGU_CTRL, &CVERSION_4Dlg::OnEguCtrl)
END_MESSAGE_MAP()


// CVERSION_4Dlg message handlers

BOOL CVERSION_4Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	/*************	Установим во всей программе кодировку ANSI	********************************/
	//	!!!!!   Делать это нужно ОБЯЗАТЕЛЬНО ПОБЫСТРЕЕ, т.к. вся работа с файлами без этого 
	//			накроется
	char *buf=setlocale( LC_CTYPE, "rus" );
	if(buf==NULL)
	{//если вернула ноль, то отработала плохо
		AfxMessageBox(_T("Не удалось поменять кодировку!!!"));
	}

	/****************************************************************************
	*	Инициализируем класс отображения ошибок
	******************************************************************************/
	m_Error.InitERROR(NULL,this);

	/*************************************************************************
	*		Общие настройки по программе (дублируются в окне USART настройка)
	**************************************************************************/
	ProtocolOptions.o_emulation=false;				//эмуляция приемо-передачи
	ProtocolOptions.o_verify=true;					//верификация полученных сообщений
	ProtocolOptions.o_inMesFile=_T("InMessage.txt");	//имя файла входящих сообщений
	ProtocolOptions.o_outMesFile=_T("OutMessage.txt");	//имя файла отправляемых сообщений
	ProtocolOptions.o_nepr=false;					//вести непрерывную передачу
	ProtocolOptions.o_OFF=false;					//вести передачу из файла
	ProtocolOptions.o_HexDisplayOutMessage=true;	//шестнадцатеричные значения при просмотре отправляемого
	ProtocolOptions.o_HexDisplayInMessage=true;		//шестнадцатеричные значения при просмотре принимаемого
	ProtocolOptions.o_LogOutMessage=false;			//фиксировать отправляемое сообщения в inMesFile
	#ifdef LAST_BUILD
	ProtocolOptions.o_LogInMessage=false;			//В релизе не будем писать полученные сообщения
	#else
	ProtocolOptions.o_LogInMessage=true;			//При отладке будем фиксировать полученные сообщения
	#endif
	ProtocolOptions.o_LogTimeMessages=false;		//регистрировать время приема и передачи сообщений
	ProtocolOptions.o_Period=700;					//период отправки сообщений при непрерывной передаче
	ProtocolOptions.o_EmulSpeed=100;				//период передачи символов при эмуляции
	
	/*******************************************************************
	*		А теперь зададим параметры нашего приемопередатчика
	*	Более точное описание параметров находится в файле  protokol_options.h
	********************************************************************/
	ProtocolOptions.var_Regime			= BYTE_STUFFING;	//режим работы
	ProtocolOptions.var_STUF_BYTE		= 0xFE;				//байт байтстаффинга
	ProtocolOptions.var_LEN_STATE		= 1;				//положение в посылке поля длины
	ProtocolOptions.var_LEN_ADD			= 5;				//сколько байт нужно добавить к длине, чтобы получить длину всего пакета
	ProtocolOptions.var_PORT_NUMBER		= PN_FIRST_AVAILABLE;//номер открываемого при старте порта
	ProtocolOptions.var_TRANSFER_SPEED	= 9600;				//скорость передачи
	ProtocolOptions.var_BITS_PER_PACKET	= 8;				//бит в байте (ну, в общем, умный меня понял...)
	ProtocolOptions.var_STOP_BITS		= 2;				//стоповых битов
	ProtocolOptions.var_PARITY			= 'O';				//контроль четности
															//'N' - нет контроля
	ProtocolOptions.var_RS485EN			= false;			//Выключим поддержку RS485
	ProtocolOptions.var_FIRST_BYTE		= 0xFF;				//первый байт, с котрого начинается посылка
	ProtocolOptions.var_ID_STATE		= 1;				//положение в пакете идентификатора отправителя (или предназначенца, зависит от протокола)
	ProtocolOptions.var_MT_STATE		= 2;				//Message Type, положение в пакете байта - идентификатора типа сообщения
	ProtocolOptions.var_DAT_STATE		= 3;				//с какого быйта в пакете начинаются данные
	ProtocolOptions.var_SUM_OFFSET		= 1;				//на сколько байт от конца отстоит чек-сумма (1 - последний байт)
	ProtocolOptions.var_MAX_LEN			= 50;				//максимальная длина сообщения в байтах (LEN = 0x14 = 20)
	ProtocolOptions.var_MY_ID			= 0x10;				//идентификатор компьютера
	ProtocolOptions.var_KONTROLLER_ID	= 0x55;				//идентификатор контроллера
	ProtocolOptions.CURRENT_SUMM		= XOR256_PLUS_1_summa;//указатель на функцию, считающую контрольную сумму
	ProtocolOptions.ExternReceiveChar	= NULL;				//функция вызывается если существующий протокол приема байтов не устраивает
	ProtocolOptions.OptimazeInMessageView=false;			//оптимизация отображения принимаемого сообщения
	ProtocolOptions.OnVerify			= OnVerify;			//функция, которая будет расшифровывать корректно принятые пакеты
	ProtocolOptions.NotifyProgram		= NotifyProgram;	//Через эту функцию диалог со связью оповещает нас о состоянии связи

	/**********************************************************************************
	*		А теперь выставим указатели на функции, которые будут компоновать сообщения
	***********************************************************************************/
	ProtocolOptions.CompileMessage_x.push_back(CompileRequestMessage);
	ProtocolOptions.CompileMessage_x.push_back(CompileAnswerMessage);

	/**************************************************************************
	*	Функция просто передает указатель на главное окно к сборнику моих функций
	****************************************************************************/
	InitMyFunctions(this);

	/*****************************************************************************
	*	ПОРАБОТАЕМ С НАШЕЙ МЕНЮШКОЙ	
	*******************************************************************************/
	if(!menu.LoadMenu(IDR_MAIN_MENU))
	{
		m_Error.SetERROR(_T("В главном окне почему-то не оказалось меню"),ERP_ERROR | ER_WRITE_TO_LOG);
	}
	SetMenu(&menu);



	/**********************************************************************************
	*	НАСТРОИМ ФАЙЛ РЕГИСТРАЦИИ ОШИБОК
	**********************************************************************************/
	LogFile.InitLogFile(_T("LogFile.txt"), &m_Error, PROGRAM_NAME);
	
	// создадим и покажем окно переключения режимов работы
	Dlg1.Create(IDD_DLG1,this);
	/*//----*/ ShowDlg1();

	// создадим и покажем окно переменных состояния
	Dlg2.Create(IDD_DLG2,this);
	/*//----*/ ShowDlg2();

	// создадим и покажем окно сигнализации
	Dlg3.Create(IDD_DLG3,this);
	/*//----*/ ShowDlg3();

	// создадим и покажем окно с дополнительными действиями
	Dlg4.Create(IDD_DLG4,this);
	//---- ShowDlg4();

	// создадим и покажем панель настройки режимов работы
	Dlg5.Create(IDD_DLG5,this);
	//---- ShowDlg5();

	/******************	Инициализация главного класса	***************************/
	if(!GLAVNIY_CLASS.InitGLAVNIY_CLASS(this, &m_Error))//должна вызываться после присвоения источников
	{//были ошибки, опевестим о них
		AfxMessageBox(_T("Инициализация главного класса прошла с ошибками!!! Программа будет закрыта."));
		return false;
	}

	// создадим и покажем окно ручной настройки связи, попутно проинициализировав DLL с ней
	//Должно вызываться ТОЛЬКО после создания главного класса и класса протокола
	pUSART_Dlg=InitUSART_Window(this,&m_Error,&ProtocolOptions);
	if(pUSART_Dlg==NULL)
	{//там что-то стреслось
		m_Error.SetERROR(_T("Невозможно создать модуль приемопередатчика"), ERP_FATALERROR | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
		return false;
	}

	#ifndef LAST_BUILD
	/*//----*/ ShowpUSART_Dlg();
	#endif

	/******************************************************************************
	*	создадим и покажем окно графиков, попутно проинициализировав DLL
	*******************************************************************************/
	InitGraph();
	
	#ifdef PROGRAM_NAME
	//автоматом нарисуем надпись
	this->SetWindowText(PROGRAM_NAME);
	#endif

	/*********************	Преобразим наше окно	***************************/
	//какой высоты нам нужен текст
	static CFont m_Font;
	m_Font.DeleteObject();  //на всякий случай
	m_Font.CreateFont		/*Выставим шрифт побольше*/
		(
			-28,                   // nHeight
			8,                         // nWidth
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
			_T("Sylfaen"));            // lpszFacename

	m_ParamDisel.Attach(this,IDC_PARAM_DISEL,&m_Font,RGB(50,180,160),RGB(15,146,189));
	m_DrvDisel.Attach(this,IDC_DRV_DISEL,&m_Font,RGB(50,180,160),RGB(15,146,189));
	m_SystemState.Attach(this,IDC_SYSTEM_STATE,&m_Font,RGB(50,180,160),RGB(15,146,189));

	/***********************	ПОРАБОТАЕМ С НАШЕЙ МЕНЮШКОЙ	**************************/
	#ifdef LAST_BUILD
	//скроем от тупорылых пользователей все сложные настройки
	menu.DeleteMenu(ID_MENU_DLG4,MF_BYCOMMAND);
	menu.DeleteMenu(ID_MENU_USART,MF_BYCOMMAND);
	#endif

	/*Только для этой программы, высветим эдитик "Нет связи"*/
	m_LinkGood.Attach(this,IDC_LINK_GOOD,&m_Font);
	m_LinkGood.SetBkColor(RGB(15,146,189));	//цвет за текстом
	LINK_NOCONNECT();						//изначально связи просто нет

	//	Сразу же начнем связываеться с блоком
	SetTimer(GLAVNIY_CLASS.PERIOD_OF_TRANSFER_ID,GLAVNIY_CLASS.period_of_transfer,NULL);

	// создадим окно определения нелинейности датчика положения
	m_EnterDlg.pGLAVNIY_CLASS = &GLAVNIY_CLASS;
	m_EnterDlg.Create(IDD_INSERT_NL_DLG, this);
	m_EnterDlg.CenterWindow();

	// создадим окно определения нелинейности датчика положения
	EGUDlg.pGLAVNIY_CLASS = &GLAVNIY_CLASS;
	EGUDlg.Create(IDD_EGU_CTRL_DLG, this);
	EGUDlg.CenterWindow();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVERSION_4Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVERSION_4Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/*******************    ShowpUSART_Dlg(void)   ******************************************
*		Функция показывает на экране диалог связи по USART
******************************************************************************************/
void CVERSION_4Dlg::ShowpUSART_Dlg(void)
{
	pUSART_Dlg->ShowWindow(SW_SHOWNORMAL);
	//pUSART_Dlg->SetFocus();
}

/*********************   ShowGraph()   **************************************************
*		Функция показывает окно с графиками
****************************************************************************************/
void CVERSION_4Dlg::ShowGraph()
{
	if(GLAVNIY_CLASS.GetGraphRegime()!=GR_ENABLED)
	{
		if(MessageBox(_T("Построение графика выключено. Включить? \n Вы всегда сможете сделать это самостоятельно по вкладке \n Настройки->Уставки по позициям, COМ порт"),_T("УВЕДОМЛЕНИЕ"), MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2)==IDYES)
			GLAVNIY_CLASS.SetGraphRegime(GR_ENABLED);
	}
	pGRAPH_Dlg->ShowWindow(SW_NORMAL);
}


/******************************    OnMenuAbout()   **************************************
*		В менюшке нажали на кнопку "О программе"
****************************************************************************************/
void CVERSION_4Dlg::ShowAboutBox()
{
	CDialog(IDD_ABOUTBOX).DoModal();
}

/***********************   OnMenuExit()   **********************************
*		В менюшке нажали на кнопку закрытия программы
****************************************************************************/
void CVERSION_4Dlg::OnMenuExit()
{
	SendMessage(WM_CLOSE,0,0);	//Отправим сообщение о выходе
}

/****************************	OnLogfile()	*************************
*		Показывает текстовый документ с происшедшими ошибками
*************************************************************************/
void CVERSION_4Dlg::ShowLogFile()
{
	LogFile.Show();
}

/************************	ShowERROR	*************************************
*	Функция занимается отображением ошибок и сообщений
****************************************************************************/
LRESULT CVERSION_4Dlg::ShowError(WPARAM w, LPARAM l)
{
	//получим указатель на объект ошибки, который мы обслуживаем
	ERROR_Class* pError=(ERROR_Class*)l;	

	unsigned char priority=pError->m_iPriority & 0x0F;
	
	//НИКАКОГО ОКНА СОСТОЯНИЯ В ЭТОЙ ПРОГРАММЕ НЕТ!!!

	if(pError->m_iPriority & ER_WRITE_TO_LOG)
	{//Стоит флаг записи в файл
		this->LogFile.Write(pError);
	}
	return 0;
}

/*************************************************************************************************
*	Следующие функции просто показывают другие диалоги на экран
**************************************************************************************************/
void CVERSION_4Dlg::ShowDlg1(void)
{
	Dlg1.ShowWindow(SW_NORMAL);
	Dlg1.SetWindowPos(&wndTopMost,10,10,50,50, SWP_NOMOVE |SWP_NOSIZE );
	
	//попробуем прорисовать окно таким образом
//	CRect rect;
//	rect.top=315;
//	rect.left=15;
//	rect.right=/*rect.left+780*/-1;
//	rect.bottom=/*rect.top+190*/-1;
//	Dlg1.MoveWindow(&rect);
}

void CVERSION_4Dlg::ShowDlg2(void)
{
	Dlg2.ShowWindow(SW_NORMAL);
	Dlg2.SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
	//попробуем прорисовать окно таким образом
//	CRect rect;
//	rect.top=40;
//	rect.left=15;
//	rect.right=rect.left+250;
//	rect.bottom=rect.top+220;
//	Dlg2.MoveWindow(&rect);
}

void CVERSION_4Dlg::ShowDlg3(void)
{
	Dlg3.ShowWindow(SW_NORMAL);
	Dlg2.SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
	//попробуем прорисовать окно таким образом
//	CRect rect;
//	rect.top=40;
//	rect.left=290;
//	rect.right=rect.left+505;
//	rect.bottom=rect.top+150;
//	Dlg3.MoveWindow(&rect);
}

void CVERSION_4Dlg::ShowDlg4(void)
{
	Dlg4.ShowWindow(SW_NORMAL);
}

void CVERSION_4Dlg::ShowDlg5(void)
{
	Dlg5.ShowWindow(SW_NORMAL);
}

/*********************    InitGraph()   *************************************
*	Функция, которая будет инициализировать график, как класс из DLL и его
*	внешний вид
******************************************************************************/
bool CVERSION_4Dlg::InitGraph()
{
	GraphicOptions.m_GraphName=_T("Графики процессов");	//имя графика
	GraphicOptions.m_oHorisontalTicks =   10;	//количество вертикальных столбиков в сетке
	GraphicOptions.m_oVerticalTicks   =    6;	//количество горизонтальных линий в сетке
	GraphicOptions.var_VisibleTime    =  100;	//время, которое график будет отображаться на экране
	GraphicOptions.m_oMoveGrid        = true;	//движущаяся по экрану сетка
	GraphicOptions.var_yMax           = 1200;	//верхняя граница графика
	GraphicOptions.var_yMin           =    0;	//нижняя граница графика
	GraphicOptions.var_X_pos          =    0;	//
	GraphicOptions.var_Y_pos          =    0;	//положение левого верхнего угла
	GraphicOptions.var_GraphWidth     =  800;	//
	GraphicOptions.var_GraphHeight    =  500;	// размеры графика

	pGRAPH_Dlg=InitGraphic_Window(this,&m_Error, &GraphicOptions);

	pGRAPH_Dlg->m_Graph.VecFunctions.resize(6);

	pGRAPH_Dlg->m_Graph.VecFunctions[0].szFunctionName = _T("Частота вращения");
	pGRAPH_Dlg->m_Graph.VecFunctions[0].szYLegend      = _T("об/мин");
	pGRAPH_Dlg->m_Graph.VecFunctions[0].ChartType      = G_LINECHART;
	pGRAPH_Dlg->m_Graph.VecFunctions[0].Color          = pGRAPH_Dlg->m_Graph.GetMultiPlotPenColor(0);
	pGRAPH_Dlg->m_Graph.VecFunctions[0].ProporCoef     = 1.0f;
	pGRAPH_Dlg->m_Graph.VecFunctions[0].Visible        = true;

	pGRAPH_Dlg->m_Graph.VecFunctions[1].szFunctionName = _T("Положение рейки");
	pGRAPH_Dlg->m_Graph.VecFunctions[1].szYLegend      = _T("код");
	pGRAPH_Dlg->m_Graph.VecFunctions[1].ChartType      = G_LINECHART;
	pGRAPH_Dlg->m_Graph.VecFunctions[1].Color          = pGRAPH_Dlg->m_Graph.GetMultiPlotPenColor(1);
	pGRAPH_Dlg->m_Graph.VecFunctions[1].ProporCoef     = 40.0f;
	pGRAPH_Dlg->m_Graph.VecFunctions[1].Visible        = true;

	pGRAPH_Dlg->m_Graph.VecFunctions[2].szFunctionName = _T("Температура масла");
	pGRAPH_Dlg->m_Graph.VecFunctions[2].szYLegend      = _T("°С");
	pGRAPH_Dlg->m_Graph.VecFunctions[2].ChartType      = G_LINECHART;
	pGRAPH_Dlg->m_Graph.VecFunctions[2].Color          = pGRAPH_Dlg->m_Graph.GetMultiPlotPenColor(2);
	pGRAPH_Dlg->m_Graph.VecFunctions[2].ProporCoef     = 1.0f;
	pGRAPH_Dlg->m_Graph.VecFunctions[2].Visible        = true;

	pGRAPH_Dlg->m_Graph.VecFunctions[3].szFunctionName = _T("Обороты турбины");
	pGRAPH_Dlg->m_Graph.VecFunctions[3].szYLegend      = _T("об/мин");
	pGRAPH_Dlg->m_Graph.VecFunctions[3].ChartType      = G_LINECHART;
	pGRAPH_Dlg->m_Graph.VecFunctions[3].Color          = pGRAPH_Dlg->m_Graph.GetMultiPlotPenColor(3);
	pGRAPH_Dlg->m_Graph.VecFunctions[3].ProporCoef     = 0.03f;
	pGRAPH_Dlg->m_Graph.VecFunctions[3].Visible        = true;

	pGRAPH_Dlg->m_Graph.VecFunctions[4].szFunctionName = _T("Давление наддува");
	pGRAPH_Dlg->m_Graph.VecFunctions[4].szYLegend      = _T("кг/см²");
	pGRAPH_Dlg->m_Graph.VecFunctions[4].ChartType      = G_LINECHART;
	pGRAPH_Dlg->m_Graph.VecFunctions[4].Color          = pGRAPH_Dlg->m_Graph.GetMultiPlotPenColor(4);
	pGRAPH_Dlg->m_Graph.VecFunctions[4].ProporCoef     = 100.0f;
	pGRAPH_Dlg->m_Graph.VecFunctions[4].Visible        = true;

	pGRAPH_Dlg->m_Graph.VecFunctions[5].szFunctionName = _T("Давление масла");
	pGRAPH_Dlg->m_Graph.VecFunctions[5].szYLegend      = _T("кг/см²");
	pGRAPH_Dlg->m_Graph.VecFunctions[5].ChartType      = G_LINECHART;
	pGRAPH_Dlg->m_Graph.VecFunctions[5].Color          = pGRAPH_Dlg->m_Graph.GetMultiPlotPenColor(5);
	pGRAPH_Dlg->m_Graph.VecFunctions[5].ProporCoef     = 20.0f;
	pGRAPH_Dlg->m_Graph.VecFunctions[5].Visible        = true;

	//чтобы избежать переполнения не буду строить графики, пока 
	//пользователь сам этого не захочет
	GLAVNIY_CLASS.SetGraphRegime(GR_DISABLED);	
	//но время буду отсчитывать как положено
	pGRAPH_Dlg->m_Graph.SetRegime(GRG_PLAY);
	return true;
}

/**********************	    PreTranslateMessage(MSG *pMsg)    *******************************
*	Вообщето в этой функции можно отлавливать любые сообщения, приходящие в 
*	главное окно, но я ее использую только для определения нажатия горячей
*	клавиши.
*********************************************************************************************/
BOOL CVERSION_4Dlg::PreTranslateMessage(MSG *pMsg)
{
	int k=0;
	switch(pMsg->message)
	{
	case WM_KEYDOWN:	//была нажата кнопка
		switch(pMsg->wParam)
		{

			case VK_ESCAPE:
			case VK_RETURN:
				return TRUE;
				break;
			//цифры будут обозначать режимы работы дизеля
			//таким образом покрывается с 1 по 10 режимы
			case '1':
			case VK_NUMPAD1:
				Dlg1.OnBnClickedPosition_1();
				break;
			case '2':
			case VK_NUMPAD2:
				Dlg1.OnBnClickedPosition_2();
				break;
			case '3':
			case VK_NUMPAD3:
				Dlg1.OnBnClickedPosition_3();
				break;
			case '4':
			case VK_NUMPAD4:
				Dlg1.OnBnClickedPosition_4();
				break;
			case '5':
			case VK_NUMPAD5:
				Dlg1.OnBnClickedPosition_5();
				break;
			case '6':
			case VK_NUMPAD6:
				Dlg1.OnBnClickedPosition_6();
				break;
			case '7':
			case VK_NUMPAD7:
				Dlg1.OnBnClickedPosition_7();
				break;
			case '8':
			case VK_NUMPAD8:
				Dlg1.OnBnClickedPosition_8();
				break;
			case '9':
			case VK_NUMPAD9:
				Dlg1.OnBnClickedPosition_9();
				break;
			case '0':
			case VK_NUMPAD0:
				Dlg1.OnBnClickedPosition_10();
				break;
			//Некоторые другие опции
			case 'W':
				Dlg1.OnRun();		//начало передачи
				break;
			case 'S':
				Dlg1.OnStop();		//конец передачи
				break;
			case 'A':
				Dlg1.OnNagr();		//работаем под нагрузкой
				break;
			case 'D':
				Dlg1.OnHolost();	//работаем на холостом ходу
				break;
			case 'Q':
				if(GetKeyState(VK_CONTROL)& 0x8000)
				{//при этом еще нажат CTRL
					ShowpUSART_Dlg();
				}
				break;
		}
		break;

	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

/********************    OnCtlColor   *******************************
*		Эта функция вызывается внешним миром, когда он хочет узнать цвето
*	какого-либо элемента
*********************************************************************/
HBRUSH CVERSION_4Dlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	/*
	Поменяем цвета шрифтов для большей наглядности
	*/

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
		
		UINT id = pWnd->GetDlgCtrlID();
		//if(id==IDC_BUTTON1)
		{
			//pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor			(RGB(255,000,000));	//цвет текста
			pDC->SetBkColor				(RGB(070,198,240));	//цвет за текстом 
			SetDCBrushColor(pDC->m_hDC,	 RGB(070,198,240));	//цвет эдитика
		}
		return (HBRUSH)::GetStockObject(DC_BRUSH);		//восстановим эти параметры
	}

	if (CTLCOLOR_EDIT == nCtlColor)
	{
		UINT id = pWnd->GetDlgCtrlID();
		
		//для всех остальных эдитиков
		pDC->SetTextColor			(RGB(255,000,000));	//цвет текста
		pDC->SetBkColor				(RGB(070,198,240));	//цвет за текстом 
		SetDCBrushColor(pDC->m_hDC,	 RGB(070,198,240));	//цвет эдитика
		return (HBRUSH)::GetStockObject(DC_BRUSH);		//восстановим эти параметры
	}

	if (CTLCOLOR_DLG == nCtlColor)
	{
		SetDCBrushColor(pDC->m_hDC,RGB(15,146,189));	//цвет заднего плана
		return (HBRUSH)::GetStockObject(DC_BRUSH);		//восстановим эти параметры
	}

	return hbr;
}

/********************   OnTimer(UINT_PTR nIDEvent)   *********************
*		Вызывается, когда сработал очередной таймер. Используется для 
*	отправики сообщений на МПСУ_ТП
**************************************************************************/
void CVERSION_4Dlg::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent==GLAVNIY_CLASS.PERIOD_OF_TRANSFER_ID)
	{//Пришло время отправить очередное сообщение на МПСУ-ТП
		pUSART_Dlg->SetMode(1);
		pUSART_Dlg->StartTransfer();
		if(GLAVNIY_CLASS.m_ReceiveControl==GLAVNIY_CLASS.RECEIVE_CONTROL_THRESHOLD)
		{//слишком много неотвеченных запросов
		//==, чтобы в случае возникновения других ошибок, у них появилась возможность быть показанными
		//на экране
			LINK_NOCONNECT();	//выведем надпись, что нет связи
		}
		else if(GLAVNIY_CLASS.m_ReceiveControl<GLAVNIY_CLASS.RECEIVE_CONTROL_THRESHOLD)
		{//контроллер успел отправить ответ
			LINK_CONNECT();
		}
		if(GLAVNIY_CLASS.m_ReceiveControl<0xFF)
		{//чтобы не вызвать ложной надписи установленной связи при переполнении
			GLAVNIY_CLASS.m_ReceiveControl++;	//нарастим счетчик неотвеченных запросов
		}
	}

	CDialog::OnTimer(nIDEvent);
}



void CVERSION_4Dlg::OnNonlinear()
{
	m_EnterDlg.ShowWindow(SW_SHOW);
}

void CVERSION_4Dlg::OnEguCtrl()
{
	EGUDlg.ShowWindow(SW_SHOW);
}

