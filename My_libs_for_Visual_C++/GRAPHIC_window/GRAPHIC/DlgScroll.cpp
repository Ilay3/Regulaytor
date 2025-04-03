// DlgScroll.cpp : implementation file
//

#include "stdafx.h"
//#include "commtest.h"
#include "DlgScroll.h"
#include "GRAPH_Dlg.h"


// CDlgScroll dialog

IMPLEMENT_DYNAMIC(CDlgScroll, CDialog)
CDlgScroll::CDlgScroll(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgScroll::IDD, pParent)
{
}

CDlgScroll::~CDlgScroll()
{
}

void CDlgScroll::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER, m_Slider);
	DDX_Control(pDX, IDC_SPIN, m_Spin);
	DDX_Control(pDX, IDC_CURRENT, m_Current);
}


BEGIN_MESSAGE_MAP(CDlgScroll, CDialog)
	ON_WM_HSCROLL()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN, OnDeltaposSpin)
	ON_WM_SHOWWINDOW()
	ON_WM_CLOSE()
	ON_EN_CHANGE(IDC_CURRENT, OnEnChangeCurrent)
END_MESSAGE_MAP()


// переопределим для защиты
void CDlgScroll::CreateMAIN(UINT str, CGRAPH_Dlg * parent)
{
	//Указатель на родителя
	this->GRAPH_WINDOW=parent;
	CDialog::Create(str,parent);
}

BOOL CDlgScroll::OnInitDialog()
{
	CDialog::OnInitDialog();

	//покажем надпись времени
	m_Font.DeleteObject();  //на всякий случай
	m_Font.CreateFont		/*Выставим шрифт побольше*/
		(
			-25,                        // nHeight
			0,                         // nWidth
			0,                         // nEscapement
			0,                         // nOrientation
			FW_NORMAL,                 // nWeight
			FALSE,                     // bItalic
			FALSE,                     // bUnderline
			0,                         // cStrikeOut
			DEFAULT_CHARSET,              // nCharSet
			OUT_DEFAULT_PRECIS,        // nOutPrecision
			CLIP_DEFAULT_PRECIS,       // nClipPrecision
			DEFAULT_QUALITY,           // nQuality
			DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
			_T("Arial"));                 // lpszFacename

	m_Current.SetFont(&m_Font,true);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgScroll::OnShowWindow(BOOL bShow, UINT nStatus)
{	
	CString str;
	
	/*if(GRAPH_WINDOW->m_Graph.Function_1.pPlotXYItems.empty())
	{
		AfxMessageBox("Вы не можете показывать окно прокрутки, пока график пуст");
		this->ShowWindow(SW_HIDE);
		//this->DestroyWindow();
		return;
	}*/
	
	CDialog::OnShowWindow(bShow, nStatus);

	GRAPH_WINDOW->Pause(); //только во время паузы можем прокручивать
	
	/*Нужно получить время, до которого выполнялся показ графика*/
	//время, до которого можно прокручивать
	if(GRAPH_WINDOW->m_Graph.VecFunctions[0].pPlotXYItems.empty())
	{//еще ничего и не показано
		m_EndTime=GRAPH_WINDOW->m_Graph.m_pGraphicOptions->var_VisibleTime;
	}
	else
	{
		m_EndTime=(GRAPH_WINDOW->m_Graph.VecFunctions[0].pPlotXYItems.back()).x;
	}

	//А теперь определимся с шагом прокрутки.
	if(GRAPH_WINDOW->m_Graph.m_pGraphicOptions->var_VisibleTime>=static_cast<float>(5))
	{// и так много показываем
		m_Scale=1;	//не уменьшать, шаг останется по 1 секунде
	}
	else if(GRAPH_WINDOW->m_Graph.m_pGraphicOptions->var_VisibleTime>=static_cast<float>(0.5))
	{
		m_Scale=static_cast<float>(0.1);	//шаг по 0,1 секунде
	}
	else if(GRAPH_WINDOW->m_Graph.m_pGraphicOptions->var_VisibleTime>=static_cast<float>(0.05))
	{
		m_Scale=static_cast<float>(0.01);	//шаг по 0,01 секунде
	}
	else
	{//ну уж очень мелко
		m_Scale=static_cast<float>(0.001);
	}
	
	//поставим диапазон и начальные значения на элементах управления
	m_Spin.SetRange32(/*GRAPH_WINDOW->m_Graph.Function_1.VisibleTime*/0,static_cast<int>(m_EndTime/m_Scale)+1);
	m_Spin.SetPos32(static_cast<int>(m_EndTime/m_Scale));
	m_Slider.SetRange(/*GRAPH_WINDOW->m_Graph.Function_1.VisibleTime*/0,static_cast<int>(m_EndTime/m_Scale)+1,true);
	m_Slider.SetPos(static_cast<int>(m_EndTime/m_Scale));
	
	//покажем изначальное время
	ShowTime(m_EndTime/m_Scale);
}

void CDlgScroll::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if(GRAPH_WINDOW->m_Graph.VecFunctions[0].pPlotXYItems.empty())
	{//еще ничего и не показано
		return;
	}
	m_Spin.SetPos32(m_Slider.GetPos());//выставим значения на стрелочках
	//покажем новое время
	ShowTime(static_cast<float>(m_Slider.GetPos()));
}

void CDlgScroll::OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	
	/*Все элементы диалога д/б связаны, т.е. изменение в одном из них должно влечь изменение в других*/
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	
	if(GRAPH_WINDOW->m_Graph.VecFunctions[0].pPlotXYItems.empty())
	{//еще ничего и не показано
		return;
	}
	UpdateData(true);
	signed int nPos=pNMUpDown->iPos;//m_Spin.GetPos32();
	nPos+=pNMUpDown->iDelta;//новая позиция
	/*if(nPos<0 || nPos>m_EndTime)
	{//не укладываемся в рамки времени
		return;
	}*/
	//m_Spin.SetPos32(nPos);
	m_Slider.SetPos(nPos);
	
	//покажем новое время
	ShowTime(static_cast<float>(nPos));
}



void CDlgScroll::OnClose()
{
	CDialog::OnClose();
	//this->ShowWindow(SW_HIDE);
//не работает	GRAPH_WINDOW->Pause();//продолжим воспроизведение
}

void CDlgScroll::OnEnChangeCurrent()
{//перерисум график под новую позицию
	FP32 temp=(FP32)m_Slider.GetPos()*m_Scale;
	GRAPH_WINDOW->m_Graph.SetX_Offset(temp);
}


//показывает текущее время прокрутки
void CDlgScroll::ShowTime(float time)
{
	time*=m_Scale;
	CString str;
	str.Format(_T("%.2f"),time);
	str.TrimRight('0');
	str.TrimRight('.');
	str.Append(_T(" c."));
	m_Current.SetWindowText(str);
}
