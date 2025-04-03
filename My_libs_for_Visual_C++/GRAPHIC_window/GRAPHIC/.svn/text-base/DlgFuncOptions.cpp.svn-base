// DlgFuncOptions.cpp : implementation file
//

#include "stdafx.h"
#include "GRAPH_Dlg.h"
#include "DlgFuncOptions.h"


// CDlgFuncOptions dialog

IMPLEMENT_DYNAMIC(CDlgFuncOptions, CDialog)

CDlgFuncOptions::CDlgFuncOptions(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFuncOptions::IDD, pParent)
{
	m_PrimaryFunctionNum=-1;	//пока я не выбирал никаких функций
}

CDlgFuncOptions::~CDlgFuncOptions()
{
}

// переопределим для защиты
void CDlgFuncOptions::CreateMAIN(UINT str, CGRAPH_Dlg * parent)
{
	//Указатель на родителя
	this->GRAPH_WINDOW=parent;
	this->Create(IDD,(CWnd *) parent);
}

void CDlgFuncOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_CURRENT_FUNC, m_CurrentFunc);
	DDX_Control(pDX, IDC_FUNC_TYPE, m_ChartType);
	DDX_Control(pDX, IDC_PROPOR_COEF, m_ProporCoef);
	DDX_Control(pDX, IDC_UNIT, m_Unit);
	DDX_Control(pDX, IDC_ALWAYS_DOWN, m_AlwaysDown);
}


BEGIN_MESSAGE_MAP(CDlgFuncOptions, CDialog)
	ON_WM_SHOWWINDOW()
	ON_CBN_SELENDOK(IDC_CURRENT_FUNC, &CDlgFuncOptions::OnCbnSelendokCurrentFunc)
	ON_CBN_SELENDOK(IDC_FUNC_TYPE, &CDlgFuncOptions::OnCbnSelendokFuncType)
	ON_EN_CHANGE(IDC_PROPOR_COEF, &CDlgFuncOptions::OnEnChangeProporCoef)
	ON_BN_CLICKED(IDC_COLOR, &CDlgFuncOptions::OnBnClickedColor)
	ON_BN_CLICKED(IDC_FUNC_NAME, &CDlgFuncOptions::OnBnClickedName)
	ON_BN_CLICKED(IDC_ALWAYS_DOWN, &CDlgFuncOptions::OnBnClickedAlwaysDown)
	ON_EN_CHANGE(IDC_UNIT, &CDlgFuncOptions::OnEnChangeUnit)
	ON_NOTIFY(UDN_DELTAPOS,IDC_LINE_WIDTH_SPIN,OnDeltaPosLineWidth)
END_MESSAGE_MAP()

/**********************************************************************************
*		ИНИЦИАЛИЗАЦИЯ		ИНИЦИАЛИЗАЦИЯ		ИНИЦИАЛИЗАЦИЯ		ИНИЦИАЛИЗАЦИЯ
***********************************************************************************/
BOOL CDlgFuncOptions::OnInitDialog()
{
	CDialog::OnInitDialog();

	//поставим ограничение на длину вводимого текста в поле коэффициента пропорциональности функции
	m_ProporCoef.SetLimitText(15);
	m_pCurrentFunc=NULL;	//обнулим указатель на текущую редактируемую функцию

	//инициализируем список с названиями типов функций
	m_ChartType.AddString(_T("Узенькие столбики к точкам данных"));
	m_ChartType.AddString(_T("Узенькие столбики по соседству друг с другом"));
	m_ChartType.AddString(_T("Широкие столбики"));
	m_ChartType.AddString(_T("Непрерывная линия"));
	m_ChartType.AddString(_T("Маленькие кружочки в точках данных"));
	m_ChartType.AddString(_T("Непрерывная линия + кружочки в точках данных"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgFuncOptions::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	//Если до этого они вдруг оказались не подсвеченными
	m_CurrentFunc.EnableWindow(true);
	m_ProporCoef.EnableWindow(true);
	m_ChartType.EnableWindow(true);
	m_Unit.EnableWindow(true);
	m_AlwaysDown.EnableWindow(true);
	GetDlgItem(IDC_STATIC)->EnableWindow(true);
	GetDlgItem(IDC_LINE_WIDTH)->EnableWindow(true);
	GetDlgItem(IDC_FUNC_NAME)->EnableWindow(true);
	GetDlgItem(IDC_LINE_WIDTH_SPIN)->EnableWindow(true);
	GetDlgItem(IDC_COLOR)->EnableWindow(true);

	//удалим все строки с именами, которые остались от предыдущих показов
	m_CurrentFunc.DeleteString(0);
	m_CurrentFunc.DeleteString(0);
	m_CurrentFunc.DeleteString(0);
	m_CurrentFunc.DeleteString(0);
	m_CurrentFunc.DeleteString(0);
	m_CurrentFunc.DeleteString(0);
	m_CurrentFunc.DeleteString(0);
	m_CurrentFunc.DeleteString(0);
//	m_CurrentFunc.Clear();

	//добавим новые имена функций
	for(size_t i=0; i<GRAPH_WINDOW->m_Graph.VecFunctions.size();i++)
	{
		m_CurrentFunc.AddString(GRAPH_WINDOW->m_Graph.VecFunctions[i].szFunctionName);
	}

	//Теперь нужно выбрать - информацию о какой функции отображать
	if(m_PrimaryFunctionNum!=-1)
	{
		if(m_PrimaryFunctionNum<(INT32S)GRAPH_WINDOW->m_Graph.VecFunctions.size())
		{//номер валидный
			m_CurrentFunc.SetCurSel(m_PrimaryFunctionNum);
			OnCbnSelendokCurrentFunc();	//обновлю информацию в эдитиках
		}
		m_PrimaryFunctionNum=-1;	//чтобы повторно этот фокус не прошёл
	}
	else if(!GRAPH_WINDOW->m_Graph.mas_SelectionFunctoins.empty())
	{//о первой веделенной функции
		m_CurrentFunc.SetCurSel((int)GRAPH_WINDOW->m_Graph.mas_SelectionFunctoins[0].m_FuncNum);
		OnCbnSelendokCurrentFunc();	//обновлю информацию в эдитиках
	}
	else if(!GRAPH_WINDOW->m_Graph.VecFunctions.empty())
	{//о первой попавшейся функции
		m_CurrentFunc.SetCurSel(0);
		OnCbnSelendokCurrentFunc();	//обновлю информацию в эдитиках
	}
	else
	{//вообще нет функций
		//закрою возможность редактирования
		m_CurrentFunc.EnableWindow(false);
		m_ProporCoef.EnableWindow(false);
		m_ChartType.EnableWindow(false);
		m_Unit.EnableWindow(false);
		m_AlwaysDown.EnableWindow(false);
		GetDlgItem(IDC_STATIC)->EnableWindow(false);
		GetDlgItem(IDC_LINE_WIDTH)->EnableWindow(false);
		GetDlgItem(IDC_FUNC_NAME)->EnableWindow(false);
		GetDlgItem(IDC_LINE_WIDTH_SPIN)->EnableWindow(false);
		GetDlgItem(IDC_COLOR)->EnableWindow(false);
	}
}

/***************************    OnCbnSelendokCurrentFunc    *****************************
*		Пользователь выбрал новую функцию
*****************************************************************************************/
void CDlgFuncOptions::OnCbnSelendokCurrentFunc()
{
	//позиция выбранного элемента
	int i=m_CurrentFunc.GetCurSel();

	//переставим указатель на вновь рассматриваемую функцию
	m_pCurrentFunc=&GRAPH_WINDOW->m_Graph.VecFunctions[i];
	
	//отобразим тип только что выбранной функции
	switch (m_pCurrentFunc->ChartType)
	{
	case G_BARCHART_TIGHT:
		m_ChartType.SetCurSel(0);	// узенькие столбики к точкам данных	
		break;
	case G_BARCHART_TIGHT_2:
		m_ChartType.SetCurSel(1);	// узенькие столбики по соседству друг с другом
		break;
	case G_BARCHART_WIDE:
		m_ChartType.SetCurSel(2);	// широкие столбики к точкам данных	
		break;
	case G_LINECHART:
		m_ChartType.SetCurSel(3);	//непрерывная ломаная линия
		break;
	case G_DOTCHART:
		m_ChartType.SetCurSel(4);	//маленькие кружочки в точках данных
		break;
	case G_LINEDOTCHART:
		m_ChartType.SetCurSel(5);	//Непрерывная линия + кружочки в точках данных
		break;
	default:
		AfxMessageBox(_T("Функция имеет неопознанный тип"));
	}

	//отобразим ее коэффициент масштабирования на экране
	CString str;
	str.Format(_T("%.2f"),m_pCurrentFunc->ProporCoef);
	str.TrimRight('0');	//|
	str.TrimRight('.');	//| просто для красоты обрежем неиспользуемые нули и точку
	m_ProporCoef.SetWindowText(str);

	//Отображу ширину линии функции
	str.Format(_T("%i"),m_pCurrentFunc->LineWidth);
	GetDlgItem(IDC_LINE_WIDTH)->SetWindowText(str);

	//отобразим ее единицу измерения
	m_Unit.SetWindowText(m_pCurrentFunc->szYLegend);

	//отобразим свойство Always Down
	if(m_pCurrentFunc->AlwaysDown)
	{
		m_AlwaysDown.SetCheck(BST_CHECKED);
	}
	else
	{
		m_AlwaysDown.SetCheck(BST_UNCHECKED);
	}

}

/****************************    OnCbnSelendokFuncType     *****************************
*		Пользователь выбрал новый тип прорисовки функции
***************************************************************************************/
void CDlgFuncOptions::OnCbnSelendokFuncType()
{
	if(m_pCurrentFunc==0)
	{//пытаемся поменять несуществующую функцию
		AfxMessageBox(_T("Функция не выбрана!!!"));
		return;
	}

	//позиция выбранного элемента
	int i=m_ChartType.GetCurSel();
	switch (i)
	{
	case 0: 
		m_pCurrentFunc->ChartType=G_BARCHART_TIGHT;		//узенькие столбики к точкам данных	
		break;
	case 1: 
		m_pCurrentFunc->ChartType=G_BARCHART_TIGHT_2;	//узенькие столбики по соседству друг с другом
		break;
	case 2: 
		m_pCurrentFunc->ChartType=G_BARCHART_WIDE;		//широкие столбики к точкам данных	
		break;
	case 3: 
		m_pCurrentFunc->ChartType=G_LINECHART;			//непрерывная ломаная линия
		break;
	case 4: 
		m_pCurrentFunc->ChartType=G_DOTCHART;			//маленькие кружочки в точках данных
		break;
	case 5:
		m_pCurrentFunc->ChartType=G_LINEDOTCHART;		//маленькие кружочки в точках данных
		break;
	default:
		AfxMessageBox(_T("В списке выбрана неопознанная позиция"));
	}
}

/**************************	OnEnChangeProporCoef()	************************************
*		Мы в окне поменяли масштаб отобнажения текущей функции, теперь, если эти изменения
*	корректны, нужно поменять значения собственно в структуре функции
****************************************************************************************/
void CDlgFuncOptions::OnEnChangeProporCoef()
{
	if(m_pCurrentFunc==0)
	{//пытаемся поменять несуществующую функцию
		AfxMessageBox(_T("Функция не выбрана!!!"));
		return;
	}

	CString str;
	m_ProporCoef.GetWindowText(str);
	float tmp=(float)_wtof(str);
	if(tmp>0)
	{//корректное значение
		m_pCurrentFunc->ProporCoef=tmp;
	}
}

/***************************	OnBnClickedColor()	******************************************
*		Выберем цвет отображения этой функции
*********************************************************************************************/
void CDlgFuncOptions::OnBnClickedColor()
{
	if(m_pCurrentFunc==0)
	{//пытаемся поменять несуществующую функцию
		AfxMessageBox(_T("Функция не выбрана!!!"));
		return;
	}

	//Просто покажем стандартный диалог цвета и посмотрим, что он вернет
	CColorDialog cc(m_pCurrentFunc->Color,NULL,this);
	 if (cc.DoModal()==IDOK)
	 {
		m_pCurrentFunc->Color=cc.GetColor();
	 }

}

/***************************	OnBnClickedName()	******************************************
*		Хотим поменять имя функции
*********************************************************************************************/
void CDlgFuncOptions::OnBnClickedName()
{
	CDlgFuncName lDlgFuncName;		//для этого я отображу специальный диалог
	if(lDlgFuncName.DoModal(m_pCurrentFunc->szFunctionName)==IDOK)
	{//пользователь ввел новое имя функции
		m_pCurrentFunc->szFunctionName=lDlgFuncName.m_FuncName;

		//ТЕПЕРЬ НУЖНО ПОМЕНЯТЬ ИМЯ ФУНКЦИИ И В СПИСКЕ
		//позиция выбранного элемента
		int i=m_CurrentFunc.GetCurSel();
		m_CurrentFunc.DeleteString(i);
		m_CurrentFunc.InsertString(i,lDlgFuncName.m_FuncName);
		m_CurrentFunc.SetCurSel(i);
	}
}

/*********************	OnEnChangeUnit()	*****************************************************
*	Когда мы меняем единицу измерения функциии, мы делаем это именно в этом эдитике
************************************************************************************************/
void CDlgFuncOptions::OnEnChangeUnit()
{
	if(m_pCurrentFunc==0)
	{//пытаемся поменять несуществующую функцию
		AfxMessageBox(_T("Функция не выбрана!!!"));
		return;
	}

	CString str;
	m_Unit.GetWindowText(str);
	m_pCurrentFunc->szYLegend=str;	//окончательно переместили
}

/**************************    OnBnClickedAlwaysDown()   *****************************
*		Изменили свойство Always Down
**************************************************************************************/
void CDlgFuncOptions::OnBnClickedAlwaysDown()
{
	//отобразим свойство Always Down
	if(m_AlwaysDown.GetCheck()==BST_CHECKED)
	{
		m_pCurrentFunc->AlwaysDown=true;
	}
	else
	{
		m_pCurrentFunc->AlwaysDown=false;
	}
}

/*************************    OnDeltaPosLineWidth     *******************************
*		Изменили толщину линии функции
************************************************************************************/
void CDlgFuncOptions::OnDeltaPosLineWidth(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_UPDOWN* pNMUpDown=(NM_UPDOWN*)pNMHDR;
	int nPos=m_pCurrentFunc->LineWidth-pNMUpDown->iDelta;
	if(nPos>0 && nPos<=10)
	{//физически допустимое количество пикселей
		m_pCurrentFunc->LineWidth=nPos;
		CString str;str.Format(_T("%i"),nPos);
		GetDlgItem(IDC_LINE_WIDTH)->SetWindowText(str);
	}
	*pResult=0;
}