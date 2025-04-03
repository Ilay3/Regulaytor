// Codomain.cpp : implementation file
//

#include "stdafx.h"
#include "DlgCodomain.h"
#include "GRAPH_Dlg.h"
#include "Graph\GraphicOptions.h"		//����� ���������� ��������� � ����� �������


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


// ������������� ��� ������
void CDlgCodomain::CreateMAIN(UINT str, CGRAPH_Dlg * parent)
{
	//��������� �� ��������
	this->GRAPH_WINDOW=parent;
	CDialog::Create(str,parent);
}

//************************************************************************************

/*********************	OnEnChangeUplimit()	*****************************************
*	�������� �������� � ������, ������ ��� ����� ��������� � ���������� �������
*************************************************************************************/
void CDlgCodomain::OnEnChangeUplimit()
{
	CString str;
	CUpLimit.GetWindowText(str);
	float tmp=(float)_wtof(str.GetBuffer());
	if(tmp>GRAPH_WINDOW->m_Graph.m_pGraphicOptions->var_yMin)
	{//�������� �������� ������, ���� ����� ����� ������� �� ������������ ��������
		GRAPH_WINDOW->m_Graph.m_pGraphicOptions->var_yMax=tmp;
	}
}

void CDlgCodomain::OnEnChangeDownlimit()
{
	CString str;
	CDownLimit.GetWindowText(str);
	float tmp=(float)_wtof(str.GetBuffer());
	if(tmp<GRAPH_WINDOW->m_Graph.m_pGraphicOptions->var_yMax)
	{//�������� �������� ������, ���� ����� ����� ������� �� ������������ ��������
		GRAPH_WINDOW->m_Graph.m_pGraphicOptions->var_yMin=tmp;
	}
}

void CDlgCodomain::OnEnChangeVisibleTime()
{
	CString str;
	CVisibleTime.GetWindowText(str);
	float tmp=(float)_wtof(str.GetBuffer());
	if(tmp>0)
	{//���� �������� ������ 0, �� ��� ����� ������
		GRAPH_WINDOW->m_Graph.m_pGraphicOptions->var_VisibleTime=tmp;
	}
}

void CDlgCodomain::OnShowWindow(BOOL bShow, UINT nStatus)
{
	//���������� ��������� �������� � ������������ � ��������������� ���������� � ������ �������
	CString str;
	str.Format(_T("%.1f"),GRAPH_WINDOW->m_Graph.m_pGraphicOptions->var_yMax);
	str.TrimRight('0');	//|
	str.TrimRight('.');	//| ������ ��� ������� �����������
	CUpLimit.SetWindowText(str);

	str.Format(_T("%.1f"),GRAPH_WINDOW->m_Graph.m_pGraphicOptions->var_yMin);
	str.TrimRight('0');	//|
	str.TrimRight('.');	//| ������ ��� ������� �����������
	CDownLimit.SetWindowText(str);
	
	str.Format(_T("%.4f"),GRAPH_WINDOW->m_Graph.m_pGraphicOptions->var_VisibleTime);
	str.TrimRight('0');	//|
	str.TrimRight('.');	//| ������ ��� ������� �����������
	CVisibleTime.SetWindowText(str);
}
