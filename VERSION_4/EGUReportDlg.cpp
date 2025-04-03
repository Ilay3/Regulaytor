// EGUReportDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VERSION_4.h"
#include "EGUReportDlg.h"


// CEGUReportDlg dialog

IMPLEMENT_DYNAMIC(CEGUReportDlg, CDialog)

//CEGUReportDlg::CEGUReportDlg(CWnd* pParent /*=NULL*/)
//	: CDialog(CEGUReportDlg::IDD, pParent)
//{

//}

CEGUReportDlg::~CEGUReportDlg()
{
}

void CEGUReportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEGUReportDlg, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CEGUReportDlg message handlers

BOOL CEGUReportDlg::OnInitDialog()
{
	int size;


	CDialog::OnInitDialog();

	CRect rect;
	GetClientRect(&rect);
	if(!m_pLIMGraph.Create(WS_VISIBLE | WS_CHILD, rect, this, 1100))
		AfxMessageBox(L"Ошибка создания объекта CLimGraphCtrl!");


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CEGUReportDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	//return CDialog::OnEraseBkgnd(pDC);
	return FALSE;
}

void CEGUReportDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
}

void CEGUReportDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (m_pLIMGraph.m_hWnd == (HWND)0) return;

	CRect rect;
	GetClientRect(&rect);

	m_pLIMGraph.MoveWindow(0, 0, rect.right,  rect.Height());
	m_pLIMGraph.ReconstructGraph();
}


void CEGUReportDlg::Update()
{
	int size;

	size = pGLAVNIY_CLASS->m_VectNonLinear.size();
	if (size == 0) {
		m_pLIMGraph.m_pGraph->ClearData();
		m_pLIMGraph.m_pGraph->GetDataGraph(0)->SetXAxisValue(0.0f, 20.0f);
		m_pLIMGraph.m_pGraph->GetDataGraph(1)->SetXAxisValue(0.0f, 20.0f);
		m_pLIMGraph.m_pGraph->GetDataGraph(0)->SetYAxisValue(0.0f, 25.0f);
		m_pLIMGraph.m_pGraph->GetDataGraph(1)->SetYAxisValue(0.0f, 25.0f);
		m_pLIMGraph.m_pGraph->Reconstruct();
		m_pLIMGraph.UpdateGraph();
		return;
	}

	m_pLIMGraph.m_pGraph->ClearData();

	float xmax = m_pLIMGraph.m_pGraph->GetDataGraph(0)->GetXAxisMax();
	float ymax = m_pLIMGraph.m_pGraph->GetDataGraph(0)->GetYAxisMax();
	if (xmax < pGLAVNIY_CLASS->m_VectNonLinear[size-1].fX) {
		m_pLIMGraph.m_pGraph->GetDataGraph(0)->SetXAxisValue(0.0f, pGLAVNIY_CLASS->m_VectNonLinear[size-1].fX);
		m_pLIMGraph.m_pGraph->GetDataGraph(1)->SetXAxisValue(0.0f, pGLAVNIY_CLASS->m_VectNonLinear[size-1].fX);
		m_pLIMGraph.m_pGraph->Reconstruct();
	}
	if (ymax < pGLAVNIY_CLASS->m_VectNonLinear[size-1].fY) {
		m_pLIMGraph.m_pGraph->GetDataGraph(0)->SetYAxisValue(0.0f, pGLAVNIY_CLASS->m_VectNonLinear[size-1].fY);
		m_pLIMGraph.m_pGraph->GetDataGraph(1)->SetYAxisValue(0.0f, pGLAVNIY_CLASS->m_VectNonLinear[size-1].fY);
		m_pLIMGraph.m_pGraph->Reconstruct();
	}

	for (int i = 0; i < size; i++) {
		m_pLIMGraph.m_pGraph->AddData(0, pGLAVNIY_CLASS->m_VectNonLinear[i].fY, pGLAVNIY_CLASS->m_VectNonLinear[i].fX);
	}

	m_pLIMGraph.m_pGraph->AddData(1, pGLAVNIY_CLASS->m_VectNonLinear[0].fY, pGLAVNIY_CLASS->m_VectNonLinear[0].fX);
	m_pLIMGraph.m_pGraph->AddData(1, pGLAVNIY_CLASS->m_VectNonLinear[size-1].fY, pGLAVNIY_CLASS->m_VectNonLinear[size-1].fX);

	m_pLIMGraph.UpdateGraph();

/*	m_pLIMGraph.m_pGraph->GetDataGraph(0)->SetVisible(TRUE);
	m_pLIMGraph.m_pGraph->GetDataGraph(1)->SetVisible(TRUE);

	size = pGLAVNIY_CLASS->m_VectNonLinear.size();
	if (size == 0) return;
	
	for (int i = 0; i < size; i++) {
		m_pLIMGraph.m_pGraph->SetData(0, pGLAVNIY_CLASS->m_VectNonLinear[i].fY, pGLAVNIY_CLASS->m_VectNonLinear[i].fX);
	}

	m_pLIMGraph.m_pGraph->SetData(1, pGLAVNIY_CLASS->m_VectNonLinear[0].fY, 0.0f);
	m_pLIMGraph.m_pGraph->SetData(1, pGLAVNIY_CLASS->m_VectNonLinear[size-1].fY, 1.0f);

	m_pLIMGraph.m_pGraph->m_DataGraphIx  = 0;
	m_pLIMGraph.UpdateGraph();				  */
}