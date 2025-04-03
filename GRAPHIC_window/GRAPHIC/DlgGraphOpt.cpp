#include "stdafx.h"
#include "DlgGraphOpt.h"
#include "GRAPH_Dlg.h"


// CDlgGraphOpt dialog

IMPLEMENT_DYNAMIC(CDlgGraphOpt, CDialog)

CDlgGraphOpt::CDlgGraphOpt(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgGraphOpt::IDD, pParent)
	, m_HorTickCount(0)
	, m_VerTickCount(0)
{

}

CDlgGraphOpt::~CDlgGraphOpt()
{
}

void CDlgGraphOpt::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COLOR_SCHEME, m_ColorScheme);
	DDX_Control(pDX, IDC_GRAPH_NAME, m_GraphName);
	DDX_Control(pDX, IDC_SPIN_HOR_TICKS, m_HorTicksSpin);
	DDX_Control(pDX, IDC_SPIN_VER_TICKS, m_VerTicksSpin);
	DDX_Text(pDX, IDC_HOR_TICK_COUNT, m_HorTickCount);
	DDX_Text(pDX, IDC_VER_TICK_COUNT, m_VerTickCount);
	DDX_Control(pDX, IDC_X_TITLE, m_edtX_Title);
}


BEGIN_MESSAGE_MAP(CDlgGraphOpt, CDialog)
	ON_WM_SHOWWINDOW()
	ON_CBN_SELENDOK(IDC_COLOR_SCHEME, &CDlgGraphOpt::OnCbnSelendokColorScheme)
	ON_EN_CHANGE(IDC_GRAPH_NAME, &CDlgGraphOpt::OnEnChangeGraphName)
	ON_NOTIFY(UDN_DELTAPOS,IDC_SPIN_HOR_TICKS,OnDeltaPosHorTickCount)
	ON_NOTIFY(UDN_DELTAPOS,IDC_SPIN_VER_TICKS,OnDeltaPosVerTickCount)
	ON_EN_CHANGE(IDC_X_TITLE, &CDlgGraphOpt::OnEnChangeXTitle)
END_MESSAGE_MAP()


/********************************************************************************
*		СОЗДАНИЕ		СОЗДАНИЕ		СОЗДАНИЕ		СОЗДАНИЕ
*********************************************************************************/
BOOL CDlgGraphOpt::Create(UINT ID, CGRAPH_Dlg* pParentWnd)
{
	ASSERT(pParentWnd);

	GRAPH_WINDOW=pParentWnd;
	return CDialog::Create(ID, pParentWnd);
}

/*******************************************************************************
*		ИНИЦИАЛИЗАЦИЯ		ИНИЦИАЛИЗАЦИЯ		ИНИЦИАЛИЗАЦИЯ
*******************************************************************************/
BOOL CDlgGraphOpt::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ColorScheme.AddString(_T("По умолчанию"));
	m_ColorScheme.AddString(_T("Для печати"));
	m_ColorScheme.AddString(_T("Красные тона"));
	m_ColorScheme.AddString(_T("Голубые тона"));
	m_ColorScheme.AddString(_T("Зелёные тона"));
	m_ColorScheme.AddString(_T("Фиолетовые тона"));
	m_ColorScheme.AddString(_T("Коричневые тона"));
	m_ColorScheme.AddString(_T("Морские тона"));

	//Надпись о цветовой схеме "по умолчанию"
	m_ColorScheme.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/********************************************************************************
*		ПОКАЗ ОКНА		ПОКАЗ ОКНА		ПОКАЗ ОКНА		ПОКАЗ ОКНА
********************************************************************************/
void CDlgGraphOpt::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	//покажу имеющееся название графика
	m_GraphName.SetWindowText(GRAPH_WINDOW->m_Graph.m_pGraphicOptions->m_GraphName);

	//покажу подпись оси Х
	m_edtX_Title.SetWindowText(GRAPH_WINDOW->m_Graph.VecFunctions[0].szXLegend);

	//покажу количество черточек по осям
	m_HorTickCount=GRAPH_WINDOW->m_Graph.m_pGraphicOptions->m_oHorisontalTicks;
	m_VerTickCount=GRAPH_WINDOW->m_Graph.m_pGraphicOptions->m_oVerticalTicks;
	UpdateData(false);
}


/****************    OnCbnSelendokColorScheme    *******************************
*		Пользователь выбрал цветовую гамму графика
********************************************************************************/
void CDlgGraphOpt::OnCbnSelendokColorScheme()
{
	//позиция выбранного элемента
	int i=m_ColorScheme.GetCurSel();

	//отобразим тип только что выбранной функции
	switch (i)
	{
	case G_DEFAULTSCHEME:
		m_ColorScheme.SetCurSel(i);	// узенькие столбики к точкам данных	
		break;
	case G_WHITESCHEME:
		m_ColorScheme.SetCurSel(i);	// узенькие столбики по соседству друг с другом
		break;
	case G_REDSCHEME:
		m_ColorScheme.SetCurSel(i);	// широкие столбики к точкам данных	
		break;
	case G_BLUESCHEME:
		m_ColorScheme.SetCurSel(i);	//непрерывная ломаная линия
		break;
	case G_GREENSCHEME:
		m_ColorScheme.SetCurSel(i);	//маленькие кружочки в точках данных
		break;
	case G_MAGENTASCHEME:
		m_ColorScheme.SetCurSel(i);	//Непрерывная линия + кружочки в точках данных
		break;
	case G_YELLOWSCHEME:
		m_ColorScheme.SetCurSel(i);	//Непрерывная линия + кружочки в точках данных
		break;
	case G_CYANSCHEME:
		m_ColorScheme.SetCurSel(i);	//Непрерывная линия + кружочки в точках данных
		break;
	default:
		AfxMessageBox(_T("Неопознанное цветовое решение"));
		return;
	}
	
	GRAPH_WINDOW->m_Graph.SetColorScheme(i);
}

/******************    OnEnChangeGraphName    *************************************
*		Пользователь поменял имя графика
**********************************************************************************/
void CDlgGraphOpt::OnEnChangeGraphName()
{
	m_GraphName.GetWindowText(GRAPH_WINDOW->m_Graph.m_pGraphicOptions->m_GraphName);
}

/******************    OnEnChangeXTitle     **************************************
*		Пользователь поменял подпись оси Х
*********************************************************************************/
void CDlgGraphOpt::OnEnChangeXTitle()
{
	if(!GRAPH_WINDOW->m_Graph.VecFunctions.empty())
	{//есть хотя бы одна функция, по которой можно рисовать подпись оси Х
		m_edtX_Title.GetWindowText(GRAPH_WINDOW->m_Graph.VecFunctions[0].szXLegend);
	}
}

/*********************************************************************************
*		Функции изменения количества отметок по осям
**********************************************************************************/
void CDlgGraphOpt::OnDeltaPosHorTickCount(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_UPDOWN* pNMUpDown=(NM_UPDOWN*)pNMHDR;
	int nPos=m_HorTickCount-pNMUpDown->iDelta;
	if(nPos>=0 && nPos<=20)
	{//физически допустимое количество тиков
		GRAPH_WINDOW->m_Graph.m_pGraphicOptions->m_oHorisontalTicks=(INT8S)nPos;
		m_HorTickCount=nPos;
		UpdateData(false);
	}
	*pResult=0;
}
void CDlgGraphOpt::OnDeltaPosVerTickCount(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_UPDOWN* pNMUpDown=(NM_UPDOWN*)pNMHDR;
	int nPos=m_VerTickCount-pNMUpDown->iDelta;
	if(nPos>=0 && nPos<=20)
	{//физически допустимое количество тиков
		GRAPH_WINDOW->m_Graph.m_pGraphicOptions->m_oVerticalTicks=(INT8S)nPos;
		m_VerTickCount=nPos;
		UpdateData(false);
	}
	*pResult=0;
}


