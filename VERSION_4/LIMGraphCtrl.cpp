#include "StdAfx.h"
#include "LIMGraphCtrl.h"

// CLIMGraphCtrl

CLIMGraphCtrl::CLIMGraphCtrl()
{
}

CLIMGraphCtrl::~CLIMGraphCtrl()
{
	delete m_pGraph;
}


BEGIN_MESSAGE_MAP(CLIMGraphCtrl, CStatic)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

BOOL CLIMGraphCtrl::Create(DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, UINT uID)
{
	BOOL  bResult;


	bResult = CWnd::Create(NULL, _T(""), dwStyle, rect, pParentWnd, uID);

	if (!bResult)
		return FALSE;

	return TRUE;
}



// CLIMGraphCtrl message handlers



int CLIMGraphCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rect;
	GetClientRect(&rect);
	m_pGraph = new CLIMGraph(&rect, this);

	return 0;
}

void CLIMGraphCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rect;
	GetClientRect(&rect);
	m_pGraph->Render(&dc, &rect, FALSE);
}

/////////////////////////////////////////////////////////////////////////////

BOOL CLIMGraphCtrl::OnEraseBkgnd(CDC* pDC) 
{
  //  to eliminate flicker....
	return 1;
}
/////////////////////////////////////////////////////////////////////////////


void CLIMGraphCtrl::ReSize(CRect *rect)
{
	MoveWindow(rect);
	ReconstructGraph();
}

void CLIMGraphCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	bool ret;


	//ret = m_pGraph->OnLButtonDown(nFlags, point);
	CWnd::OnLButtonDown(nFlags, point);
}

void CLIMGraphCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	bool ret;


	/*ret = m_pGraph->OnLButtonUp(nFlags, point);
	if (ret)
	{
		UpdateGraph();	
	}

	UpdateParams();*/
	CWnd::OnLButtonUp(nFlags, point);
}

void CLIMGraphCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnRButtonDown(nFlags, point);
}

void CLIMGraphCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	bool ret;


	ret = m_pGraph->OnMouseMove(nFlags, point);
	UpdateGraph();	

	CWnd::OnMouseMove(nFlags, point);
}


void CLIMGraphCtrl::UpdateGraph(void)
{
	CClientDC dc(this);
	CRect rect;
	GetClientRect(&rect);
	m_pGraph->Render(&dc, &rect, FALSE);
}

void CLIMGraphCtrl::ReconstructGraph(void)
{
	CClientDC dc(this);
	CRect rect;
	GetClientRect(&rect);
	m_pGraph->Reconstruct();
	m_pGraph->Render(&dc, &rect, FALSE);
}

void CLIMGraphCtrl::OnContextMenu(CWnd* pWnd, CPoint point)
{
	GetParent()->SendMessage(WM_CONTEXTMENU, (WPARAM)pWnd, MAKELPARAM(point.x, point.y));
}


void CLIMGraphCtrl::UpdateParams()
{
	//GetParent()->SendMessage(WM_LIMGRAPH_TO_PARAMS, m_pGraph->m_DataGraphIx, 0);
}