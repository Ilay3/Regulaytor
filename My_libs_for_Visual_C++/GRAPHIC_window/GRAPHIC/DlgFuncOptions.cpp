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
	m_PrimaryFunctionNum=-1;	//���� � �� ������� ������� �������
}

CDlgFuncOptions::~CDlgFuncOptions()
{
}

// ������������� ��� ������
void CDlgFuncOptions::CreateMAIN(UINT str, CGRAPH_Dlg * parent)
{
	//��������� �� ��������
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
*		�������������		�������������		�������������		�������������
***********************************************************************************/
BOOL CDlgFuncOptions::OnInitDialog()
{
	CDialog::OnInitDialog();

	//�������� ����������� �� ����� ��������� ������ � ���� ������������ ������������������ �������
	m_ProporCoef.SetLimitText(15);
	m_pCurrentFunc=NULL;	//������� ��������� �� ������� ������������� �������

	//�������������� ������ � ���������� ����� �������
	m_ChartType.AddString(_T("�������� �������� � ������ ������"));
	m_ChartType.AddString(_T("�������� �������� �� ��������� ���� � ������"));
	m_ChartType.AddString(_T("������� ��������"));
	m_ChartType.AddString(_T("����������� �����"));
	m_ChartType.AddString(_T("��������� �������� � ������ ������"));
	m_ChartType.AddString(_T("����������� ����� + �������� � ������ ������"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgFuncOptions::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	//���� �� ����� ��� ����� ��������� �� �������������
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

	//������ ��� ������ � �������, ������� �������� �� ���������� �������
	m_CurrentFunc.DeleteString(0);
	m_CurrentFunc.DeleteString(0);
	m_CurrentFunc.DeleteString(0);
	m_CurrentFunc.DeleteString(0);
	m_CurrentFunc.DeleteString(0);
	m_CurrentFunc.DeleteString(0);
	m_CurrentFunc.DeleteString(0);
	m_CurrentFunc.DeleteString(0);
//	m_CurrentFunc.Clear();

	//������� ����� ����� �������
	for(size_t i=0; i<GRAPH_WINDOW->m_Graph.VecFunctions.size();i++)
	{
		m_CurrentFunc.AddString(GRAPH_WINDOW->m_Graph.VecFunctions[i].szFunctionName);
	}

	//������ ����� ������� - ���������� � ����� ������� ����������
	if(m_PrimaryFunctionNum!=-1)
	{
		if(m_PrimaryFunctionNum<(INT32S)GRAPH_WINDOW->m_Graph.VecFunctions.size())
		{//����� ��������
			m_CurrentFunc.SetCurSel(m_PrimaryFunctionNum);
			OnCbnSelendokCurrentFunc();	//������� ���������� � ��������
		}
		m_PrimaryFunctionNum=-1;	//����� �������� ���� ����� �� ������
	}
	else if(!GRAPH_WINDOW->m_Graph.mas_SelectionFunctoins.empty())
	{//� ������ ���������� �������
		m_CurrentFunc.SetCurSel((int)GRAPH_WINDOW->m_Graph.mas_SelectionFunctoins[0].m_FuncNum);
		OnCbnSelendokCurrentFunc();	//������� ���������� � ��������
	}
	else if(!GRAPH_WINDOW->m_Graph.VecFunctions.empty())
	{//� ������ ���������� �������
		m_CurrentFunc.SetCurSel(0);
		OnCbnSelendokCurrentFunc();	//������� ���������� � ��������
	}
	else
	{//������ ��� �������
		//������ ����������� ��������������
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
*		������������ ������ ����� �������
*****************************************************************************************/
void CDlgFuncOptions::OnCbnSelendokCurrentFunc()
{
	//������� ���������� ��������
	int i=m_CurrentFunc.GetCurSel();

	//���������� ��������� �� ����� ��������������� �������
	m_pCurrentFunc=&GRAPH_WINDOW->m_Graph.VecFunctions[i];
	
	//��������� ��� ������ ��� ��������� �������
	switch (m_pCurrentFunc->ChartType)
	{
	case G_BARCHART_TIGHT:
		m_ChartType.SetCurSel(0);	// �������� �������� � ������ ������	
		break;
	case G_BARCHART_TIGHT_2:
		m_ChartType.SetCurSel(1);	// �������� �������� �� ��������� ���� � ������
		break;
	case G_BARCHART_WIDE:
		m_ChartType.SetCurSel(2);	// ������� �������� � ������ ������	
		break;
	case G_LINECHART:
		m_ChartType.SetCurSel(3);	//����������� ������� �����
		break;
	case G_DOTCHART:
		m_ChartType.SetCurSel(4);	//��������� �������� � ������ ������
		break;
	case G_LINEDOTCHART:
		m_ChartType.SetCurSel(5);	//����������� ����� + �������� � ������ ������
		break;
	default:
		AfxMessageBox(_T("������� ����� ������������ ���"));
	}

	//��������� �� ����������� ��������������� �� ������
	CString str;
	str.Format(_T("%.2f"),m_pCurrentFunc->ProporCoef);
	str.TrimRight('0');	//|
	str.TrimRight('.');	//| ������ ��� ������� ������� �������������� ���� � �����
	m_ProporCoef.SetWindowText(str);

	//�������� ������ ����� �������
	str.Format(_T("%i"),m_pCurrentFunc->LineWidth);
	GetDlgItem(IDC_LINE_WIDTH)->SetWindowText(str);

	//��������� �� ������� ���������
	m_Unit.SetWindowText(m_pCurrentFunc->szYLegend);

	//��������� �������� Always Down
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
*		������������ ������ ����� ��� ���������� �������
***************************************************************************************/
void CDlgFuncOptions::OnCbnSelendokFuncType()
{
	if(m_pCurrentFunc==0)
	{//�������� �������� �������������� �������
		AfxMessageBox(_T("������� �� �������!!!"));
		return;
	}

	//������� ���������� ��������
	int i=m_ChartType.GetCurSel();
	switch (i)
	{
	case 0: 
		m_pCurrentFunc->ChartType=G_BARCHART_TIGHT;		//�������� �������� � ������ ������	
		break;
	case 1: 
		m_pCurrentFunc->ChartType=G_BARCHART_TIGHT_2;	//�������� �������� �� ��������� ���� � ������
		break;
	case 2: 
		m_pCurrentFunc->ChartType=G_BARCHART_WIDE;		//������� �������� � ������ ������	
		break;
	case 3: 
		m_pCurrentFunc->ChartType=G_LINECHART;			//����������� ������� �����
		break;
	case 4: 
		m_pCurrentFunc->ChartType=G_DOTCHART;			//��������� �������� � ������ ������
		break;
	case 5:
		m_pCurrentFunc->ChartType=G_LINEDOTCHART;		//��������� �������� � ������ ������
		break;
	default:
		AfxMessageBox(_T("� ������ ������� ������������ �������"));
	}
}

/**************************	OnEnChangeProporCoef()	************************************
*		�� � ���� �������� ������� ����������� ������� �������, ������, ���� ��� ���������
*	���������, ����� �������� �������� ���������� � ��������� �������
****************************************************************************************/
void CDlgFuncOptions::OnEnChangeProporCoef()
{
	if(m_pCurrentFunc==0)
	{//�������� �������� �������������� �������
		AfxMessageBox(_T("������� �� �������!!!"));
		return;
	}

	CString str;
	m_ProporCoef.GetWindowText(str);
	float tmp=(float)_wtof(str);
	if(tmp>0)
	{//���������� ��������
		m_pCurrentFunc->ProporCoef=tmp;
	}
}

/***************************	OnBnClickedColor()	******************************************
*		������� ���� ����������� ���� �������
*********************************************************************************************/
void CDlgFuncOptions::OnBnClickedColor()
{
	if(m_pCurrentFunc==0)
	{//�������� �������� �������������� �������
		AfxMessageBox(_T("������� �� �������!!!"));
		return;
	}

	//������ ������� ����������� ������ ����� � ���������, ��� �� ������
	CColorDialog cc(m_pCurrentFunc->Color,NULL,this);
	 if (cc.DoModal()==IDOK)
	 {
		m_pCurrentFunc->Color=cc.GetColor();
	 }

}

/***************************	OnBnClickedName()	******************************************
*		����� �������� ��� �������
*********************************************************************************************/
void CDlgFuncOptions::OnBnClickedName()
{
	CDlgFuncName lDlgFuncName;		//��� ����� � �������� ����������� ������
	if(lDlgFuncName.DoModal(m_pCurrentFunc->szFunctionName)==IDOK)
	{//������������ ���� ����� ��� �������
		m_pCurrentFunc->szFunctionName=lDlgFuncName.m_FuncName;

		//������ ����� �������� ��� ������� � � ������
		//������� ���������� ��������
		int i=m_CurrentFunc.GetCurSel();
		m_CurrentFunc.DeleteString(i);
		m_CurrentFunc.InsertString(i,lDlgFuncName.m_FuncName);
		m_CurrentFunc.SetCurSel(i);
	}
}

/*********************	OnEnChangeUnit()	*****************************************************
*	����� �� ������ ������� ��������� ��������, �� ������ ��� ������ � ���� �������
************************************************************************************************/
void CDlgFuncOptions::OnEnChangeUnit()
{
	if(m_pCurrentFunc==0)
	{//�������� �������� �������������� �������
		AfxMessageBox(_T("������� �� �������!!!"));
		return;
	}

	CString str;
	m_Unit.GetWindowText(str);
	m_pCurrentFunc->szYLegend=str;	//������������ �����������
}

/**************************    OnBnClickedAlwaysDown()   *****************************
*		�������� �������� Always Down
**************************************************************************************/
void CDlgFuncOptions::OnBnClickedAlwaysDown()
{
	//��������� �������� Always Down
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
*		�������� ������� ����� �������
************************************************************************************/
void CDlgFuncOptions::OnDeltaPosLineWidth(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_UPDOWN* pNMUpDown=(NM_UPDOWN*)pNMHDR;
	int nPos=m_pCurrentFunc->LineWidth-pNMUpDown->iDelta;
	if(nPos>0 && nPos<=10)
	{//��������� ���������� ���������� ��������
		m_pCurrentFunc->LineWidth=nPos;
		CString str;str.Format(_T("%i"),nPos);
		GetDlgItem(IDC_LINE_WIDTH)->SetWindowText(str);
	}
	*pResult=0;
}