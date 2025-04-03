// Codomain.cpp : implementation file
//

#include "stdafx.h"
#include "DlgCodomain.h"
#include "GRAPH_Dlg.h"
#include "Graph\GraphicOptions.h"		//здесь передаются настройки в класс графика


// CDlgCodomain dialog

IMPLEMENT_DYNAMIC(CDlgCodomain, CDialog)
CDlgCodomain::CDlgCodomain(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCodomain::IDD, pParent)
{
}

CDlgCodomain::~CDlgCodomain()
{
}

void CDlgCodomain::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_UPLIMIT, CUpLimit);
	DDX_Control(pDX, IDC_DOWNLIMIT, CDownLimit);
	DDX_Control(pDX, IDC_VISIBLE_TIME, CVisibleTime);
}


BEGIN_MESSAGE_MAP(CDlgCodomain, CDialog)
	ON_EN_CHANGE(IDC_UPLIMIT, OnEnChangeUplimit)
	ON_EN_CHANGE(IDC_DOWNLIMIT, OnEnChangeDownlimit)
	ON_EN_CHANGE(IDC_VISIBLE_TIME, OnEnChangeVisibleTime)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// переопределим для защиты
void CDlgCodomain::CreateMAIN(UINT str, CGRAPH_Dlg * parent)
{
	//Указатель на родителя
	this->GRAPH_WINDOW=parent;
	CDialog::Create(str,parent);
}

//************************************************************************************

/*********************	OnEnChangeUplimit()	*****************************************
*	Поменяли значение в окошке, теперь его нужно перенести в переменные функций
*************************************************************************************/
void CDlgCodomain::OnEnChangeUplimit()
{
	CString str;
	CUpLimit.GetWindowText(str);
	float tmp=(float)_wtof(str.GetBuffer());
	if(tmp>GRAPH_WINDOW->m_Graph.m_pGraphicOptions->var_yMin)
	{//поменяем значение только, если после этого пределы не перевернутся наоборот
		GRAPH_WINDOW->m_Graph.m_pGraphicOptions->var_yMax=tmp;
	}
}

void CDlgCodomain::OnEnChangeDownlimit()
{
	CString str;
	CDownLimit.GetWindowText(str);
	float tmp=(float)_wtof(str.GetBuffer());
	if(tmp<GRAPH_WINDOW->m_Graph.m_pGraphicOptions->var_yMax)
	{//поменяем значение только, если после этого пределы не перевернутся наоборот
		GRAPH_WINDOW->m_Graph.m_pGraphicOptions->var_yMin=tmp;
	}
}

void CDlgCodomain::OnEnChangeVisibleTime()
{
	CString str;
	CVisibleTime.GetWindowText(str);
	float tmp=(float)_wtof(str.GetBuffer());
	if(tmp>0)
	{//если значение больше 0, то ему можно верить
		GRAPH_WINDOW->m_Graph.m_pGraphicOptions->var_VisibleTime=tmp;
	}
}

void CDlgCodomain::OnShowWindow(BOOL bShow, UINT nStatus)
{
	//произведем начальную загрузку с соответствии с действительными значениями в классе графика
	CString str;
	str.Format(_T("%.1f"),GRAPH_WINDOW->m_Graph.m_pGraphicOptions->var_yMax);
	str.TrimRight('0');	//|
	str.TrimRight('.');	//| только для красоты отображения
	CUpLimit.SetWindowText(str);

	str.Format(_T("%.1f"),GRAPH_WINDOW->m_Graph.m_pGraphicOptions->var_yMin);
	str.TrimRight('0');	//|
	str.TrimRight('.');	//| только для красоты отображения
	CDownLimit.SetWindowText(str);
	
	str.Format(_T("%.4f"),GRAPH_WINDOW->m_Graph.m_pGraphicOptions->var_VisibleTime);
	str.TrimRight('0');	//|
	str.TrimRight('.');	//| только для красоты отображения
	CVisibleTime.SetWindowText(str);
}
