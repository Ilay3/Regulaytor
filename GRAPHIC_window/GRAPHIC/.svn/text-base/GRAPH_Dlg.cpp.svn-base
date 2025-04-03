#include "stdafx.h"
#include "GRAPH_Dlg.h"
#include "time.h"
//#include "afx.h"		//для функции GetStatus

IMPLEMENT_DYNAMIC(CGRAPH_Dlg, CDialog)

BEGIN_MESSAGE_MAP(CGRAPH_Dlg, CDialog)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_CLOSE()

	ON_COMMAND(ID_FUNCTION_1,ShowFunction_1)
	ON_COMMAND(ID_FUNCTION_2,ShowFunction_2)
	ON_COMMAND(ID_FUNCTION_3,ShowFunction_3)
	ON_COMMAND(ID_FUNCTION_4,ShowFunction_4)
	ON_COMMAND(ID_FUNCTION_5,ShowFunction_5)
	ON_COMMAND(ID_FUNCTION_6,ShowFunction_6)
	ON_COMMAND(ID_FUNCTION_7,ShowFunction_7)
	ON_COMMAND(ID_FUNCTION_8,ShowFunction_8)

	ON_COMMAND(ID_PAUSE,Pause)
	ON_COMMAND(ID_STOP,Stop)
	ON_COMMAND(ID_RESTART,Restart)

	ON_COMMAND(ID_MOVE,ShowDlgScroll)
	ON_COMMAND(ID_FUNC_OPT ,ShowDlgFuncOptions)
	ON_COMMAND(ID_GRAPH_OPT, ShowDlgGraphOpt)
	ON_COMMAND(ID_CODOMAIN,ShowDlgCodomain)

	ON_COMMAND(ID_SAVE,Save)
	ON_COMMAND(ID_SAVE_FOR_MATHCAD,SaveForMathcad)
	ON_COMMAND(ID_LOAD,Load)
	ON_COMMAND(ID_SAVE_BMP,SaveBMP)

	ON_WM_MENUSELECT()
	ON_WM_SIZE()
	ON_WM_SIZING()

	ON_MESSAGE(MSG_GR_FUNC_NAME_SEL,FuncNameSelect)
	ON_COMMAND(ID_PRINTGRAPH, &CGRAPH_Dlg::OnPrintGraph)
END_MESSAGE_MAP()


/**********************************************************
*		КОНСТРУКТОР
************************************************************/
CGRAPH_Dlg::CGRAPH_Dlg(CWnd* pParent, UINT ID)
	: CDialog(ID, pParent)
{
	m_pGraphicOptions=NULL;
	m_pError=NULL;
//	m_pGraph=NULL;	//сбросим указатель на будущий график
}

/**********************************************************
*		ДЕСТРУКТОР
************************************************************/
CGRAPH_Dlg::~CGRAPH_Dlg()
{
	//подчистим наш график
/*	if (m_pGraph)
	{
		delete m_pGraph;
	}*/
}


BOOL CGRAPH_Dlg::Create(UINT ID, CWnd* pParentWnd,ERROR_Class * pError, CGraphicOptions * pGraphicOptions)
{
	ASSERT(pGraphicOptions);
	m_pGraphicOptions=pGraphicOptions;

	//перебросим указатели на класс регистрации ошибок
	ASSERT(pError);
	m_pError=pError;

	return CDialog::Create(ID, pParentWnd);
}

void CGRAPH_Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CGRAPH_Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	
	menu.LoadMenu(IDR_MENU_FOR_GRAPH);
	SetMenu(&menu);

	//Проставим иконку окна графиков
	HICON	m_hIcon;
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON);
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	//изначально расставим галочки в пунктах меню, как было инициализировано в функциях
	UINT tmp;

	tmp=menu.GetMenuState(ID_FUNCTION_1,MF_BYCOMMAND)==MF_CHECKED?MF_UNCHECKED:MF_CHECKED;
	menu.CheckMenuItem(ID_FUNCTION_1, MF_CHECKED | MF_BYCOMMAND);

	tmp=menu.GetMenuState(ID_FUNCTION_2,MF_BYCOMMAND)==MF_CHECKED?MF_UNCHECKED:MF_CHECKED;
	menu.CheckMenuItem(ID_FUNCTION_2, MF_CHECKED | MF_BYCOMMAND);

	tmp=menu.GetMenuState(ID_FUNCTION_3,MF_BYCOMMAND)==MF_CHECKED?MF_UNCHECKED:MF_CHECKED;
	menu.CheckMenuItem(ID_FUNCTION_3, MF_CHECKED | MF_BYCOMMAND);

	tmp=menu.GetMenuState(ID_FUNCTION_4,MF_BYCOMMAND)==MF_CHECKED?MF_UNCHECKED:MF_CHECKED;
	menu.CheckMenuItem(ID_FUNCTION_4, MF_CHECKED | MF_BYCOMMAND);

	tmp=menu.GetMenuState(ID_FUNCTION_5,MF_BYCOMMAND)==MF_CHECKED?MF_UNCHECKED:MF_CHECKED;
	menu.CheckMenuItem(ID_FUNCTION_5, MF_CHECKED | MF_BYCOMMAND);

	tmp=menu.GetMenuState(ID_FUNCTION_6,MF_BYCOMMAND)==MF_CHECKED?MF_UNCHECKED:MF_CHECKED;
	menu.CheckMenuItem(ID_FUNCTION_6, MF_CHECKED | MF_BYCOMMAND);

	tmp=menu.GetMenuState(ID_FUNCTION_7,MF_BYCOMMAND)==MF_CHECKED?MF_UNCHECKED:MF_CHECKED;
	menu.CheckMenuItem(ID_FUNCTION_7, MF_CHECKED | MF_BYCOMMAND);

	tmp=menu.GetMenuState(ID_FUNCTION_8,MF_BYCOMMAND)==MF_CHECKED?MF_UNCHECKED:MF_CHECKED;
	menu.CheckMenuItem(ID_FUNCTION_8, MF_CHECKED | MF_BYCOMMAND);

	if(!m_Graph.Create(this,m_pGraphicOptions,m_pError,G_DEFAULTSCHEME))
	{
		m_pError->SetERROR(_TEXT("При инициализации графика возникла ошибка"), ERP_FATALERROR | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
		return FALSE;
	}
	
	//Создадим график во весь диалог (!Для корректности размеров это нужно делать после создания графика
	this->SetWindowPos(0,0,0,m_pGraphicOptions->var_GraphWidth+5,m_pGraphicOptions->var_GraphHeight+40,SWP_NOMOVE | SWP_NOZORDER);
	

	//Создадим диалог окна видимой области графика
	DlgCodomain.CreateMAIN(IDD_CODOMAIN_DLG,this);
	//++++ ShowDlgCodomain(); //и покажем его (информация при показе до начала построения графика может быть некорректной)

	//Создадим диалог прокрутки изображения
	DlgScroll.CreateMAIN(IDD_SCROLL_BACK,this);
	//++++ ShowDlgScroll(); //и покажем его
	
	//Создадим окно настройки графика
	DlgGraphOpt.Create(IDD_GRAPH_OPT,this);
	//++++ ShowDlgGraphOpt();

	//создадим диалог с опциями конкретной функции
	DlgFuncOptions.CreateMAIN(IDD_FUNC_OPT,this);
	//++++ ShowDlgFuncOptions();

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CGRAPH_Dlg::OnClose()
{

	if(DlgScroll.IsWindowVisible())
	{//если при этом было показано окно прокрутки, то закроем его
		::SendMessage(DlgScroll.m_hWnd,WM_CLOSE,0,0);
	}

	CDialog::OnClose();
}

//******************************************************************************
// отображать или нет функцию
void CGRAPH_Dlg::ShowFunction_1()
{//показывать / не показывать рекомендуется менять только здесь,
//иначе может быть разноглачие с меню
	ASSERT(m_Graph.VecFunctions.size()>0);
	//узнаем состояние менюшки
	UINT state=menu.GetMenuState(ID_FUNCTION_1,MF_BYCOMMAND);

	if (state & MF_CHECKED)
	{//до этого было вабрано показывать
		menu.CheckMenuItem(ID_FUNCTION_1, MF_UNCHECKED | MF_BYCOMMAND);
		m_Graph.VecFunctions[0].Visible=false;//а теперь спрячем
	}
    else
	{//до этого было выбрано не показывать
		menu.CheckMenuItem(ID_FUNCTION_1, MF_CHECKED | MF_BYCOMMAND);
		m_Graph.VecFunctions[0].Visible=true;//и снова покажем
	}
}
// отображать или нет функцию
void CGRAPH_Dlg::ShowFunction_2()
{//показывать / не показывать рекомендуется менять только здесь,
//иначе может быть разноглачие с меню
	ASSERT(m_Graph.VecFunctions.size()>1);
	//узнаем состояние менюшки
	UINT state=menu.GetMenuState(ID_FUNCTION_2,MF_BYCOMMAND);

	if (state & MF_CHECKED)
	{//до этого было вабрано показывать
		menu.CheckMenuItem(ID_FUNCTION_2, MF_UNCHECKED | MF_BYCOMMAND);
		m_Graph.VecFunctions[1].Visible=false;//а теперь спрячем
	}
    else
	{//до этого было выбрано не показывать
		menu.CheckMenuItem(ID_FUNCTION_2, MF_CHECKED | MF_BYCOMMAND);
		m_Graph.VecFunctions[1].Visible=true;//и снова покажем
	}
}
// отображать или нет функцию
void CGRAPH_Dlg::ShowFunction_3()
{//показывать / не показывать рекомендуется менять только здесь,
//иначе может быть разноглачие с меню
	ASSERT(m_Graph.VecFunctions.size()>2);
	//узнаем состояние менюшки
	UINT state=menu.GetMenuState(ID_FUNCTION_3,MF_BYCOMMAND);

	if (state & MF_CHECKED)
	{//до этого было вабрано показывать
		menu.CheckMenuItem(ID_FUNCTION_3, MF_UNCHECKED | MF_BYCOMMAND);
		m_Graph.VecFunctions[2].Visible=false;//а теперь спрячем
	}
    else
	{//до этого было выбрано не показывать
		menu.CheckMenuItem(ID_FUNCTION_3, MF_CHECKED | MF_BYCOMMAND);
		m_Graph.VecFunctions[2].Visible=true;//и снова покажем
	}
}
// отображать или нет функцию
void CGRAPH_Dlg::ShowFunction_4()
{//показывать / не показывать рекомендуется менять только здесь,
//иначе может быть разноглачие с меню
	ASSERT(m_Graph.VecFunctions.size()>3);
	//узнаем состояние менюшки
	UINT state=menu.GetMenuState(ID_FUNCTION_4,MF_BYCOMMAND);

	if (state & MF_CHECKED)
	{//до этого было вабрано показывать
		menu.CheckMenuItem(ID_FUNCTION_4, MF_UNCHECKED | MF_BYCOMMAND);
		m_Graph.VecFunctions[3].Visible=false;//а теперь спрячем
	}
    else
	{//до этого было выбрано не показывать
		menu.CheckMenuItem(ID_FUNCTION_4, MF_CHECKED | MF_BYCOMMAND);
		m_Graph.VecFunctions[3].Visible=true;//и снова покажем
	}
}
// отображать или нет функцию
void CGRAPH_Dlg::ShowFunction_5()
{//показывать / не показывать рекомендуется менять только здесь,
//иначе может быть разноглачие с меню
	ASSERT(m_Graph.VecFunctions.size()>4);
	//узнаем состояние менюшки
	UINT state=menu.GetMenuState(ID_FUNCTION_5,MF_BYCOMMAND);

	if (state & MF_CHECKED)
	{//до этого было вабрано показывать
		menu.CheckMenuItem(ID_FUNCTION_5, MF_UNCHECKED | MF_BYCOMMAND);
		m_Graph.VecFunctions[4].Visible=false;//а теперь спрячем
	}
    else
	{//до этого было выбрано не показывать
		menu.CheckMenuItem(ID_FUNCTION_5, MF_CHECKED | MF_BYCOMMAND);
		m_Graph.VecFunctions[4].Visible=true;//и снова покажем
	}
}
// отображать или нет функцию
void CGRAPH_Dlg::ShowFunction_6()
{//показывать / не показывать рекомендуется менять только здесь,
//иначе может быть разноглачие с меню
	ASSERT(m_Graph.VecFunctions.size()>5);
	//узнаем состояние менюшки
	UINT state=menu.GetMenuState(ID_FUNCTION_6,MF_BYCOMMAND);

	if (state & MF_CHECKED)
	{//до этого было вабрано показывать
		menu.CheckMenuItem(ID_FUNCTION_6, MF_UNCHECKED | MF_BYCOMMAND);
		m_Graph.VecFunctions[5].Visible=false;//а теперь спрячем
	}
    else
	{//до этого было выбрано не показывать
		menu.CheckMenuItem(ID_FUNCTION_6, MF_CHECKED | MF_BYCOMMAND);
		m_Graph.VecFunctions[5].Visible=true;//и снова покажем
	}

}
// отображать или нет функцию
void CGRAPH_Dlg::ShowFunction_7()
{//показывать / не показывать рекомендуется менять только здесь,
//иначе может быть разноглачие с меню
	ASSERT(m_Graph.VecFunctions.size()>6);
	//узнаем состояние менюшки
	UINT state=menu.GetMenuState(ID_FUNCTION_7,MF_BYCOMMAND);

	if (state & MF_CHECKED)
	{//до этого было вабрано показывать
		menu.CheckMenuItem(ID_FUNCTION_7, MF_UNCHECKED | MF_BYCOMMAND);
		m_Graph.VecFunctions[6].Visible=false;//а теперь спрячем
	}
    else
	{//до этого было выбрано не показывать
		menu.CheckMenuItem(ID_FUNCTION_7, MF_CHECKED | MF_BYCOMMAND);
		m_Graph.VecFunctions[6].Visible=true;//и снова покажем
	}

}
// отображать или нет функцию
void CGRAPH_Dlg::ShowFunction_8()
{//показывать / не показывать рекомендуется менять только здесь,
//иначе может быть разноглачие с меню
	ASSERT(m_Graph.VecFunctions.size()>7);
	//узнаем состояние менюшки
	UINT state=menu.GetMenuState(ID_FUNCTION_8,MF_BYCOMMAND);

	if (state & MF_CHECKED)
	{//до этого было вабрано показывать
		menu.CheckMenuItem(ID_FUNCTION_8, MF_UNCHECKED | MF_BYCOMMAND);
		m_Graph.VecFunctions[7].Visible=false;//а теперь спрячем
	}
    else
	{//до этого было выбрано не показывать
		menu.CheckMenuItem(ID_FUNCTION_8, MF_CHECKED | MF_BYCOMMAND);
		m_Graph.VecFunctions[7].Visible=true;//и снова покажем
	}

}
//************************************************************************************

//показать окно настройки вида функций
void CGRAPH_Dlg::ShowDlgFuncOptions()
{
	DlgFuncOptions.ShowWindow(SW_NORMAL);
}
	
//в менюшке нажали на паузу
void CGRAPH_Dlg::Pause (void)
{
	CString str;
	menu.GetMenuString(ID_PAUSE,str,MF_BYCOMMAND);
	if(str==_T("Пауза"))
	{//надо сделать паузу
		if(m_Graph.SetRegime(GRG_PAUSE))
		{
			menu.ModifyMenu(ID_PAUSE, MF_BYCOMMAND, ID_PAUSE, _T("Продолжить"));
		}
	}
	else
	{//надо опять воспроизводить
		if(DlgScroll.IsWindowVisible())
		{
			AfxMessageBox(_T("Закройте окно прокрутки и повторите команду"));
			//this->GRAPH_WINDOW->DlgScroll.CloseWindow();
			return;
		}
		if(m_Graph.SetRegime(GRG_PLAY))
		{
			menu.ModifyMenu(ID_PAUSE, MF_BYCOMMAND, ID_PAUSE, _T("Пауза"));
		}
	}
}
/*************************** Stop ****************************************
*					в менюшке нажали на стоп
***************************************************************************/
void CGRAPH_Dlg::Stop (void)
{
	//Для большей информативности напишем пауза
	menu.ModifyMenu(ID_PAUSE, MF_BYCOMMAND, ID_PAUSE, _T("Пауза"));
	//Функции остановлены, мы больше не можем манипулировать ими
	menu.EnableMenuItem(ID_PAUSE,MF_BYCOMMAND | MF_GRAYED);
	menu.EnableMenuItem(ID_STOP,MF_BYCOMMAND | MF_GRAYED);
	
	m_Graph.SetRegime(GRG_STOP);
}

/*************************** Start ****************************************
*					в менюшке нажали на старт
***************************************************************************/
void CGRAPH_Dlg::Restart (void)
{
	//Снова можем управлять
	menu.EnableMenuItem(ID_PAUSE,MF_BYCOMMAND | MF_ENABLED);
	menu.EnableMenuItem(ID_STOP,MF_BYCOMMAND | MF_ENABLED);

	//перезапустим график
	m_Graph.SetRegime(GRG_RESTART);

	//поменяем название диалогового окна на что-то нейтральное
	SetWindowText(_T("  График"));
}

//*****************************************************************************************

//показать окно с прокруткой графика
void CGRAPH_Dlg::ShowDlgScroll(void)
{
	DlgScroll.ShowWindow(SW_NORMAL);
}

//показать окно настройки графика
void CGRAPH_Dlg::ShowDlgGraphOpt(void)
{
	DlgGraphOpt.ShowWindow(SW_NORMAL);
}

//показать окно области значений
void CGRAPH_Dlg::ShowDlgCodomain(void)
{
	DlgCodomain.ShowWindow(SW_NORMAL);
	DlgCodomain.SetFocus();
}
//******************************************************************************************

//сохранить график
void CGRAPH_Dlg::Save (void)
{
	// создание стандартной панели выбора файла Open
	CFileDialog DlgOpen(FALSE,(LPCTSTR)_T("ser"),_T("first.ser"),OFN_HIDEREADONLY,(LPCTSTR)_T("Файлы графика (*.ser) |*.ser||"));
	while(1)
	{//на случай, если пользователь с первой попытки не сможет выбрать
		// отображение стандартной панели выбора файла Open
		if(DlgOpen.DoModal()==IDOK)
		{
			CString str;
			str=DlgOpen.GetFileName();
			CFileStatus lStatus;
			if(CFile::GetStatus(str,lStatus))
			{//файл с таким именем уже существует
				if(AfxMessageBox(_T("Файл с таким именем уже существует. Перезаписать?"), MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2)==IDNO)
				{//пользователь не захотел перезаписывать этот файл, начну с начала
					continue;
				}
			}
			if(m_Graph.SaveGraph(str.GetBuffer()))
			{//сохранение прошло успешно
				m_pError->SetERROR(_T("График успешно сохранен в файл \"")+str+_T("\""),ERP_GOOD | ER_WRITE_TO_LOG);
			}
		}
		break;
	}
}

/*******************************    SaveForMathcad(void)   ***********************
*	Функция сохраняет данные из ПЕРВОГО_ВИДИМОГО графика в файл формата для mathcad
**********************************************************************************/
void CGRAPH_Dlg::SaveForMathcad(void)
{
	// создание стандартной панели выбора файла Open
	CFileDialog DlgOpen(FALSE,(LPCTSTR)_T("dat"),_T("for_mathcad.dat"),OFN_HIDEREADONLY,(LPCTSTR)_T("Файлы данных для mathcad (*.dat) |*.dat||"));
	// отображение стандартной панели выбора файла Open
	if(DlgOpen.DoModal()==IDOK)
	{
		CString str;
		str=DlgOpen.GetFileName();
		
		if(m_Graph.SaveGraphMathcad(str.GetBuffer(), -1))
		{//сохранение прошло успешно
			m_pError->SetERROR(_T("График успешно сохранен для Mathcad в файл \"")+str+_T("\""),ERP_GOOD | ER_WRITE_TO_LOG);
		}
	}
}

/*******************************    Load (void)   ******************************
*		Загружает график из файла
**********************************************************************************/
void CGRAPH_Dlg::Load (void)
{
	// создание стандартной панели выбора файла Open
	CFileDialog DlgOpen(TRUE,(LPCTSTR)_T("ser"),NULL,0,(LPCTSTR)_T("Файлы графика (*.ser) |*.ser| Все файлы (*.*) |*.*||"),this);
	// отображение стандартной панели выбора файла Open
	if(DlgOpen.DoModal()==IDOK)
	{
		CString open_file_name, open_file_dir;
		open_file_name=DlgOpen.GetFileName();
		open_file_dir=DlgOpen.GetPathName();
		
		if(!m_Graph.VecFunctions.empty() && AfxMessageBox(_T("Добавить информацию к существующему графику?"), MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2)==IDYES)
		{//на графике уже есть функции и пользователь не хочет, чтобы они исчезали
			m_Graph.LoadGraphToExists(open_file_name.GetBuffer());
		}
		else
		{
			m_Graph.LoadGraph(open_file_name.GetBuffer());
			//поменяем название диалогового окна, чтобы оно отображало имя открытого файла
			if(open_file_dir.GetLength()>80)
			{//укоротим слишком длинное имя папки
				open_file_dir=_T("...")+open_file_dir.Right(80);
			}
			SetWindowText(_T("   ")+open_file_dir);
		}
	}
}

/***************************** SaveBMP(void) ***************************************************
*		Сохраняет клиенскую область окна (где и рисуется график) в файл *.bmp 
*************************************************************************************************/
void CGRAPH_Dlg :: SaveBMP(void)
{
	//и его имя
	CString BMP_file_name;
	

	// создание стандартной панели выбора файла Open
	CFileDialog DlgOpen(FALSE,(LPCTSTR)_T("bmp"),_T("graph_1.bmp"),OFN_HIDEREADONLY,(LPCTSTR)_T("Рисунок (*.bmp) |*.bmp||"));
	// отображение стандартной панели выбора файла Open

	if(DlgOpen.DoModal()==IDOK)
	{
		BMP_file_name=DlgOpen.GetFileName();
		//если пользователь не проставил расширение
		if(BMP_file_name.Find('.')==BMP_file_name.GetLength())
			BMP_file_name=BMP_file_name+_T(".bmp");

		if(m_Graph.SaveGraphBMP(BMP_file_name.GetBuffer()))
		{//сохранение прошло успешно
			m_pError->SetERROR(_T("Картинка графика успешно сохранена в файл \"")+BMP_file_name+_T("\""),ERP_GOOD | ER_WRITE_TO_LOG);
		}
	}
}



/********************************* OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu) **********
*		Эта функция вызывается, когда пользователь ткнул на любок меню в этом окне
*****************************************************************************************************/
void CGRAPH_Dlg::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu)
{
	CDialog::OnMenuSelect(nItemID, nFlags, hSysMenu);
	
	//Нам нужно отобразить галочки в правильном виде, чтобы они правильно показали свойство
	//Visible каждой из функций
	if(nItemID==0)
	{//Это как раз меню "Вид"
		/*Это выпадающий список по видимым функциям, нам нужно проверить, видима ли функция,
		, и если да - поставить напротив нее галочку
		*/
		for(unsigned char i=0; i<8; i++)
		{
			UINT ID;
			switch (i)
			{
			case 0: ID=ID_FUNCTION_1; break;
			case 1: ID=ID_FUNCTION_2; break;
			case 2: ID=ID_FUNCTION_3; break;
			case 3: ID=ID_FUNCTION_4; break;
			case 4: ID=ID_FUNCTION_5; break;
			case 5: ID=ID_FUNCTION_6; break;
			case 6: ID=ID_FUNCTION_7; break;
			case 7: ID=ID_FUNCTION_8; break;
			}
			if(i>=m_Graph.VecFunctions.size())
			{//такой функции вообще не существует
				menu.EnableMenuItem(ID, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
			}
			else
			{//функция есть, нужно посмотреть, видима ли она
				if(m_Graph.VecFunctions[i].Visible)
				{	menu.CheckMenuItem(ID, MF_CHECKED | MF_BYCOMMAND); }
				else
				{	menu.CheckMenuItem(ID, MF_UNCHECKED | MF_BYCOMMAND);}
			}
		}
	}
}

/*********************    OnSize(UINT nType, int cx, int cy)   ********************
*		Эта функция вызывается, когда поменяли размеры окна. Нам нужно поменять размеры
*	графика
**********************************************************************************/
void CGRAPH_Dlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	//нужно установить в крафике размеры
	m_Graph.SetGraphSize(cy, cx);
}

/**********************   OnSizing(UINT fwSide, LPRECT pRect)   ********************
*		Это сообщение вызывается, когда мы еще только пытаемся поменять размер
*	а значит есть возможность ввести допустимые границы
*************************************************************************************/
void CGRAPH_Dlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	//отсечем размеры, которые не допустимы
	if((pRect->right-pRect->left)<G_MINGRAPHWIDTH)
	{
		pRect->right=pRect->left+G_MINGRAPHWIDTH;
	}
	INT8U lCorrect=50;	//корректировка на меню и заголовок окна
	if((pRect->bottom-pRect->top-lCorrect)<G_MINGRAPHHEIGHT)
	{
		pRect->bottom=pRect->top+lCorrect+G_MINGRAPHHEIGHT;
	}
	CDialog::OnSizing(fwSide, pRect);
}

/****************    FuncNameSelect    *********************************************
*		График прислал сообщение о том, что пользователь сделал двойной щелче на 
*	одном из имен функций
***********************************************************************************/
LRESULT CGRAPH_Dlg::FuncNameSelect(WPARAM FuncNum, LPARAM l)
{
	//отображу информацию об указанной функции
	DlgFuncOptions.m_PrimaryFunctionNum=(INT32S)FuncNum;
	ShowDlgFuncOptions();

	return 0;
}

/****************************	DestroyGRAPH(CGRAPH_Dlg* pDlg)	*****************
*		Эта функция определена в классе CGRAPH_Dlg как friеnd и поэтому имеет доступ
*	к закрытому деструктору
*********************************************************************************/
void DestroyGRAPH(CGRAPH_Dlg* pDlg)
{
	//pDlg->SendMessage(WM_CLOSE);
	//pDlg->~CUSART_Dlg();
	delete pDlg;
}

/*******************************     OnPrintGraph    *******************************
*		В менюшке нажали на распечатывание графика.
***********************************************************************************/
void CGRAPH_Dlg::OnPrintGraph()
{
	//вызову одноименную функцию графика
	m_Graph.PrintGraph();
}
