#include "StdAfx.h"
#include "LimGraph.h"


using namespace Gdiplus;

CLIMGraph::CLIMGraph(void)
{
}


CLIMGraph::CLIMGraph(CRect* rect, CWnd* pWnd)
{
	pParent = pWnd;


	m_dXAxisMin     = 0.0f;
	m_dYAxisMin     = 0.0f;
	m_dXAxisMax     = 1.0f;
	m_dYAxisMax     = 10.0f;

	m_clrBkTrend	= RGB(255, 255, 255);
	m_clrBkGnd		= RGB(255, 255, 255);
	m_clrBkGraph	= RGB(255, 255, 255);
	m_clrTitle      = RGB(255,   0, 255);
	m_clrBkTitle    = RGB(  0,  25, 255);
	m_clrGrid       = RGB(  0,   0,   0);

	m_nGridMainX    = 4;
	m_nGridMainY    = 10;
	m_nGridSecX     = 5;
	m_nGridSecY     = 5;

	m_bGridBig      = TRUE;
	m_bGridSmall    = FALSE;

	m_sTitle        = L"Заголовок";


	BeginFromStart = 0.0f;
	TimeFromStart = 16.0f;
	VisibleTime   = 16.0f;

// графики по умолчанию

	COLORREF *clr;
    UINT nl;

	CDataGraph NONLINEAR;
	NONLINEAR.SetYAxisValue(0.0f, 25.0f);
	NONLINEAR.SetXAxisValue(0.0f, 20.0f);
	NONLINEAR.SetSerieName(_T("Нелинейность датчика положения"));
	NONLINEAR.SetXAxisName(_T(""));
	NONLINEAR.SetColor(RGB(255, 0, 0));
	NONLINEAR.SetActive(TRUE);
	NONLINEAR.SetVisible(TRUE);
	AddDataGraph(NONLINEAR);

	CDataGraph LINEAR;
	LINEAR.SetYAxisValue(0.0f, 25.0f);
	LINEAR.SetXAxisValue(0.0f, 20.0f);
	LINEAR.SetSerieName(_T(""));
	LINEAR.SetXAxisName(_T(""));
	LINEAR.SetColor(RGB(0, 0, 255));
	LINEAR.SetActive(TRUE);
	LINEAR.SetVisible(TRUE);
	AddDataGraph(LINEAR);

	m_eState = SEL_NONE;
	m_eAct   = ACT_NONE;
	m_iCYMove = m_iYMoveFirst = 0;

	m_DataGraphIx = -1;
	m_DataPointIx = -1;

	m_DataGraphIx = 0;	  // по умолчанию - Пропорциональный коэффициент
}


CLIMGraph::~CLIMGraph(void)
{
	int idx = 0;
	COLORREF clr;
	CDataGraph* pDataGraph;


	pDataGraph = GetDataGraph(idx++);
	pDataGraph->Clear();

	pDataGraph = GetDataGraph(idx++);
	pDataGraph->Clear();

	m_Data.clear();
}


void CLIMGraph::AddData(int idx, float val, float fidx)
{	
	AddPoint(idx, fidx, val);
}


void CLIMGraph::SetData(int idx, float val, float fidx)
{	
	SetY(idx, (int)fidx, val);
}


void CLIMGraph::SetTitle(CString str)
{
	m_sTitle = str;
	Reconstruct();
}


BOOL CLIMGraph::AddDataGraph(CDataGraph data)
{
	if(m_Data.size() >= m_Data.max_size()) {
		return FALSE;
	} else   {
		m_Data.push_back(data);
	}
	return TRUE;
}

unsigned int CLIMGraph::GetSize(void)
{
	return (unsigned int) m_Data.size();
}

unsigned int CLIMGraph::GetDataSize(int pos)
{
	CDataGraph* pDataGraph;


	pDataGraph = GetDataGraph(pos);
	if (pDataGraph == NULL) return 0;
	return pDataGraph->GetSize();
}


CDataGraph* CLIMGraph::GetDataGraph(unsigned int pos)
{
	if (pos <= GetSize()) {
		return &m_Data[pos];
	} else {
		return NULL;
	}
}


void CLIMGraph::AddPoint(int pos, float x, float y)
{
	CDataGraph* pDataGraph;


	pDataGraph = GetDataGraph(pos);
	if (pDataGraph == NULL) return;

	pDataGraph->Add(x, y);
}

void CLIMGraph::DelPoint(int gpos, int ppos)
{
	CDataGraph* pDataGraph;


	pDataGraph = GetDataGraph(gpos);
	if (pDataGraph == NULL) return;

	pDataGraph->Del(ppos);
}

float CLIMGraph::GetX(int pos_data, int pos_point)
{
	CDataGraph* pDataGraph;


	pDataGraph = GetDataGraph(pos_data);
	if (pDataGraph == NULL) return 0.0f;
	return pDataGraph->GetX(pos_point);
}

float CLIMGraph::GetY(int pos_data, int pos_point)
{
	CDataGraph* pDataGraph;


	pDataGraph = GetDataGraph(pos_data);
	if (pDataGraph == NULL) return 0.0f;
	return pDataGraph->GetY(pos_point);
}

void CLIMGraph::SetY(int pos_data, int pos_point, float val)
{
	CDataGraph* pDataGraph;


	pDataGraph = GetDataGraph(pos_data);
	if (pDataGraph == NULL) return;;
	pDataGraph->SetY(pos_point, val);
}


void CLIMGraph::SetSerieName(int pos, CString str)
{
	CDataGraph* pDataGraph;


	pDataGraph = GetDataGraph(pos);
	if (pDataGraph == NULL) return;
	pDataGraph->SetSerieName(str);
}

CString CLIMGraph::GetSerieName(int pos)
{
	CDataGraph* pDataGraph;


	pDataGraph = GetDataGraph(pos);
	if (pDataGraph == NULL) return L"";
	return pDataGraph->GetSerieName();
}

void CLIMGraph::SetXAxisName(int pos, CString str)
{
	CDataGraph* pDataGraph;


	pDataGraph = GetDataGraph(pos);
	if (pDataGraph == NULL) return;
	pDataGraph->SetXAxisName(str);
}

CString CLIMGraph::GetXAxisName(int pos)
{
	CDataGraph* pDataGraph;


	pDataGraph = GetDataGraph(pos);
	if (pDataGraph == NULL) return L"";
	return pDataGraph->GetXAxisName();
}

void CLIMGraph::SetYAxisName(int pos, CString str)
{
	CDataGraph* pDataGraph;


	pDataGraph = GetDataGraph(pos);
	if (pDataGraph == NULL) return;
	pDataGraph->SetYAxisName(str);
}

CString CLIMGraph::GetYAxisName(int pos)
{
	CDataGraph* pDataGraph;


	pDataGraph = GetDataGraph(pos);
	if (pDataGraph == NULL) return L"";
	return pDataGraph->GetYAxisName();
}

void CLIMGraph::SetXAxisUnit(int pos, CString str)
{
	CDataGraph* pDataGraph;


	pDataGraph = GetDataGraph(pos);
	if (pDataGraph == NULL) return;
	pDataGraph->SetXAxisUnit(str);
}

CString CLIMGraph::GetXAxisUnit(int pos)
{
	CDataGraph* pDataGraph;


	pDataGraph = GetDataGraph(pos);
	if (pDataGraph == NULL) return L"";
	return pDataGraph->GetXAxisUnit();
}

void CLIMGraph::SetYAxisUnit(int pos, CString str)
{
	CDataGraph* pDataGraph;


	pDataGraph = GetDataGraph(pos);
	if (pDataGraph == NULL) return;
	pDataGraph->SetYAxisUnit(str);
}

CString CLIMGraph::GetYAxisUnit(int pos)
{
	CDataGraph* pDataGraph;


	pDataGraph = GetDataGraph(pos);
	if (pDataGraph == NULL) return L"";
	return pDataGraph->GetYAxisUnit();
}


/////////////////////////////////////////////////////
void CLIMGraph::SetXAxisScale(CDataGraph *graph, float min, float max)
{
	//swap min and max if they are the wrong way around
	float temp, scale;

	if (max < min)
	{
		temp = min;
		min  = max;
		max  = temp;
	}
	//if min and max are the same (especially if they are both zero
	//it can be a problem - so give them a bit of room
	if(min==max)
	{
		max += (float)0.1;
	}
	
	m_dXAxisMax = max;
	m_dXAxisMin = min;

	//
	int lmargin = 0;
	int rmargin = 0;

	temp = max - min;										//the spread of the x-axiz
	scale = ((float)(m_iGraphWidth - (lmargin + rmargin)))/temp;	//calc pixels/x
	m_dPixelsPerX = scale;
	
	//where would the x-origin be located?
	if( (min < 0) && (max >  0) )
	{
		m_iOriginX = (int)(fabs(min) * m_dPixelsPerX + lmargin);
	}
	else if ( (min < 0) && (max <= 0) )
	{
		m_iOriginX = (m_iGraphWidth) - rmargin;
	}
	else if (min >= 0 && (max >= 0))
	{
		m_iOriginX = lmargin;
	}
}
 
void CLIMGraph::SetYAxisScale(CDataGraph *graph, float min, float max)
{
	float temp,scale;
	//swap min and max if they are the wrong way around
	if (max < min)
	{
		temp=min;
		min=max;
		max=temp;
	}

	//if min and max are the same (especially if they are both zero
	//it can be a problem - so give them a bit of room
	if(min==max)
	{
		max+= (float)0.001;
	}

	m_dYAxisMin = min;
	m_dYAxisMax = max;

	//calculate scaling
	int bmargin = 0;
	int tmargin = 0;

	temp = max - min;										//the spread of the x-axiz
	scale = ((float)(m_iGraphHeight - (bmargin + tmargin)))/temp;//calc pixels/x
	m_dPixelsPerY = scale;

	//where should the Y origin be?

	if (min <0 && max > 0)//if Y passes through zero
	{
		//from the bottom of the graph
		m_iOriginY = (int)(fabs(min)*m_dPixelsPerY + bmargin);
	}
	else if(min < 0 && max <= 0)//if Y values are all negative
	{
		m_iOriginY = (r_graph.bottom - r_graph.top) + bmargin;
	}
	else if (min >= 0 && max >= 0)//if Y values are all positive
	{
		m_iOriginY = bmargin;
	}		
}


//////////////////////////////////////////////////////

void CLIMGraph::Render(CDC *dc, CRect *rect, BOOL bPrint)
{
	if (rect->IsRectEmpty())
		return;


	CMemDC memDC(*dc, *rect);

	if ((m_dcBackground.GetSafeHdc() == NULL) || (m_bitmapBackground.m_hObject == NULL))
	{
		m_dcBackground.CreateCompatibleDC(&memDC.GetDC());
		m_bitmapBackground.CreateCompatibleBitmap(&memDC.GetDC(), rect->Width(), rect->Height());
		m_pBitmapOldBackground = m_dcBackground.SelectObject(&m_bitmapBackground);

		UpdateGraphRect(rect);

		drawBackground(&m_dcBackground);
//		drawXAxisScale(&m_dcBackground);
//		drawYAxisScale(&m_dcBackground);
	}
	memDC.GetDC().BitBlt(0, 0, rect->Width(), rect->Height(), &m_dcBackground, 0, 0, SRCCOPY);

	drawLineGraph(&memDC.GetDC());	

	drawXAxisScale(&memDC.GetDC());
	drawYAxisScale(&memDC.GetDC());
	drawTip(&memDC.GetDC());
}

void CLIMGraph::DrawBackground(COLORREF color)
{
	m_clrBkGnd = color;
	Reconstruct();
}

void CLIMGraph::drawBackground(CDC *pDC)
{
	Graphics	 graphics(pDC->m_hDC);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
	graphics.SetSmoothingMode    (SmoothingModeAntiAlias);
	Color clr, clr1, clr2;

	Rect wrect;
	wrect.X      = m_rWnd.left;
	wrect.Y      = m_rWnd.top;
	wrect.Width  = m_rWnd.Width();
	wrect.Height = m_rWnd.Height();
 	clr.SetFromCOLORREF(RGB(240, 255, 240));
	SolidBrush wbrush(clr);
	graphics.FillRectangle(&wbrush, wrect);

	Rect grect;
	clr1.SetFromCOLORREF(RGB(210, 210, 255));
	clr2.SetFromCOLORREF(RGB(250, 250, 255));

	grect.X      = m_rGraph.left;
	grect.Y      = m_rGraph.top;
	grect.Width  = m_rGraph.Width();
	grect.Height = m_rGraph.Height();
	LinearGradientBrush brush(grect, clr1, clr2, LinearGradientModeVertical);

	clr.SetFromCOLORREF(m_clrGrid);
	Pen rectPen(clr, 1.0f);
	graphics.DrawRectangle(&rectPen, grect);

	graphics.FillRectangle(&brush, grect);

	drawGrid(pDC);
}

void CLIMGraph::drawGrid(CDC* pDC)
{
		// Сетка Крупная
		{
			double hX = ((double)m_rGraph.Width()) / ((double)m_nGridMainX);
			double hY = ((double)m_rGraph.Height()) / ((double)m_nGridMainY);

			LOGBRUSH logBrush;
			logBrush.lbStyle = BS_SOLID;
			logBrush.lbColor = m_clrGrid;
			CPen pen;
			pen.CreatePen(PS_DOT|PS_GEOMETRIC, 1, &logBrush);
			CPen *ppen = pDC->SelectObject(&pen);

	//		pDC->Rectangle(&m_rMarkX);
	//		pDC->Rectangle(&m_rMarkY);

	//		pDC->Rectangle(&m_rGraph);

			// сетка по вертикали
			for (int i = 1; i<m_nGridMainX; i++) {
				pDC->MoveTo(m_rGraph.left + (int)((double)i*hX), m_rGraph.top);
				pDC->LineTo(m_rGraph.left + (int)((double)i*hX), m_rGraph.bottom);
			}

			// сетка по горизонтали
			for (int j = 1; j<m_nGridMainY; j++) {
				pDC->MoveTo(m_rGraph.left,  m_rGraph.top + (int)(((double)j)*hY));
				pDC->LineTo(m_rGraph.right, m_rGraph.top + (int)(((double)j)*hY));
			}

			pDC->SelectObject(ppen);
/*
			pen.DeleteObject();
			pen.CreatePen(PS_SOLID, 1, m_clrGrid);
			ppen = pDC->SelectObject(&pen);

			// штрихи по горизонтали шкалы
			for (int i = 0; i<m_nGridMainX+1; i++) {
				pDC->MoveTo(m_rMarkX.left + (int)((double)i*hX), m_rMarkX.top);
				pDC->LineTo(m_rMarkX.left + (int)((double)i*hX), m_rMarkX.bottom);
			}

			// штрихи по вертикали шкалы
			for (int j = 0; j<m_nGridMainY+1; j++) {
				pDC->MoveTo(m_rMarkY.left,  m_rMarkY.top + (int)(((double)j)*hY));
				pDC->LineTo(m_rMarkY.right, m_rMarkY.top + (int)(((double)j)*hY));
			}
			pDC->SelectObject(ppen); */
		}
}

void CLIMGraph::Reconstruct() 
{
	if ((m_pBitmapOldBackground) && (m_bitmapBackground.GetSafeHandle()) && (m_dcBackground.GetSafeHdc()))
	{
		m_dcBackground.SelectObject(m_pBitmapOldBackground);
		m_dcBackground.DeleteDC();
		m_bitmapBackground.DeleteObject();
	}

	if (m_DataGraphIx != -1) {
		m_dYAxisMax    = GetDataGraph(m_DataGraphIx)->GetYAxisMax();
		m_dYAxisMin    = GetDataGraph(m_DataGraphIx)->GetYAxisMin();
		BeginFromStart = GetDataGraph(m_DataGraphIx)->GetXAxisMax();
		TimeFromStart  = GetDataGraph(m_DataGraphIx)->GetXAxisMin();
	}  else {
		m_dYAxisMax    = 1.0f; // по молчанию
		m_dYAxisMin    = 0.0f;
		BeginFromStart = 0.0f;
		TimeFromStart  = 1.0f;
	}
}

void CLIMGraph::UpdateGraphRect(CRect *rect)
{
	memcpy(&m_rWnd, rect, sizeof(CRect));

	memcpy(&m_rGraph, &m_rWnd, sizeof(CRect));
	m_rGraph.left    += (int)((float)m_rWnd.Height() * 0.14f);
	m_rGraph.top     += (int)((float)m_rWnd.Height() * 0.10f);
	m_rGraph.right   -= (int)((float)m_rWnd.Height() * 0.05f);
	m_rGraph.bottom  -= (int)((float)m_rWnd.Height() * 0.1f);

	if (m_rGraph.Width() < 10) {
		m_rGraph.right = m_rGraph.left + 10;
	}
	if (m_rGraph.Height() < 10) {
		m_rGraph.bottom = m_rGraph.top + 10;
	}
	double hX = ((double)m_rGraph.Width()) / ((double)m_nGridMainX);
	double hY = ((double)m_rGraph.Height()) / ((double)m_nGridMainY);
	m_rGraph.right  = m_rGraph.left + (int)(hX * ((double)m_nGridMainX));
 	m_rGraph.bottom = m_rGraph.top  + (int)(hY * ((double)m_nGridMainY));

	m_iGraphX		= m_rGraph.left;				//расположение графика в окне
	m_iGraphY		= m_rGraph.top;				//
	m_iGraphWidth	= m_rGraph.Width();			//
	m_iGraphHeight	= m_rGraph.Height();			//

	m_rMarkX.left		=	m_rGraph.left;
	m_rMarkX.top		=	m_rGraph.bottom;
	m_rMarkX.right		=	m_rGraph.right;
	m_rMarkX.bottom		=	m_rMarkX.top + (int)((float)m_rGraph.Height()*0.01f);

	m_rMarkY.left		=	m_rGraph.left - (int)((float)m_rGraph.Height()*0.01f);
	m_rMarkY.top		=	m_rGraph.top;
	m_rMarkY.right		=	m_rGraph.left;
	m_rMarkY.bottom		=	m_rGraph.bottom;

	m_rScaleX.left    =	m_rGraph.left;
	m_rScaleX.top     =	m_rMarkX.bottom;
	m_rScaleX.right   =	m_rGraph.right;
	m_rScaleX.bottom  =	m_rScaleX.top + (int)((float)m_rGraph.Height()*0.05f);

	m_rScaleY.left    =	m_rMarkY.left - (int)((float)m_rGraph.Height()*0.15f);
	m_rScaleY.top     =	m_rGraph.top;
	m_rScaleY.right   =	m_rMarkY.left;
	m_rScaleY.bottom  =	m_rGraph.bottom;

	m_rTitleX.left    =	m_rGraph.left;
	m_rTitleX.top     =	m_rScaleX.bottom;
	m_rTitleX.right   =	m_rGraph.right;
	m_rTitleX.bottom  =	m_rTitleX.top + (int)((float)m_rGraph.Height()*0.05f);
 
	m_rTitleY.left    =	m_rGraph.left;
	m_rTitleY.top     =	m_rWnd.top;
	m_rTitleY.right   =	m_rGraph.right;
	m_rTitleY.bottom  =	m_rGraph.top;
  /*
	m_rTitleY.left    =	m_rScaleY.left;
	m_rTitleY.top     =	m_rScaleY.top;
	m_rTitleY.right   =	m_rScaleY.right;
	m_rTitleY.bottom  =	m_rScaleY.bottom;
  */

	// Убрать ...............................................................................
	// горизонтальный фонт
	m_fontScale.DeleteObject();
	LOGFONT lf;

	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	lf.lfWeight = FW_NORMAL;
	lf.lfHeight = (int)((float)m_rGraph.Height()*0.05f);
	if (lf.lfHeight == 0)
		lf.lfHeight = 1;
	m_fontScale.CreateFontIndirect(&lf);


	// вертикальный фонт
	m_fontScaleVert.DeleteObject();
	lf.lfEscapement = 900;
	m_fontScaleVert.CreateFontIndirect(&lf);

	// вертикальный фонт
	m_fontLegend.DeleteObject();
	lf.lfHeight = (int)((float)m_rGraph.Height()*0.04f);
	if (lf.lfHeight == 0)
		lf.lfHeight = 1;
	lf.lfEscapement = 0;
	m_fontLegend.CreateFontIndirect(&lf);
}

void CLIMGraph::drawLineGraph(CDC* memDC)
{
	int size = GetSize();
	int csize;

	CDataGraph *pGraph;

//	memDC->IntersectClipRect(&m_rGraph);
	for (int i=0; i<size; i++) {
		pGraph = GetDataGraph(i);
		csize = pGraph->GetSize();
		if (pGraph->IsVisible() && csize > 0) {
			DoGraph(pGraph);
			DoPlotXY(pGraph, memDC);
		}
	}
}

void CLIMGraph::drawTip(CDC* memDC)
{
	CString str;


	if (m_DataPointIx != -1) {
		CRect rect(m_ptCursor.x + 5, m_ptCursor.y - 25, m_ptCursor.x + 60, m_ptCursor.y - 5);

		Graphics	 graphics(memDC->m_hDC);
		graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
		graphics.SetSmoothingMode    (SmoothingModeAntiAlias);
		graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);
		Gdiplus::FontFamily    fontFamily(L"Arial");
		Gdiplus::Font          fontValue (&fontFamily, (REAL)rect.Height()*0.75f, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
		Gdiplus::StringFormat format;
		format.SetAlignment(Gdiplus::StringAlignmentCenter);
		format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
		Color clr;
		clr.SetFromCOLORREF(RGB(0, 0, 0));
		Gdiplus::SolidBrush  txtBrush(clr);


		CPen penDotBlack(PS_DOT, 1, RGB(128, 128, 128));
		CPen *ppenOld = memDC->SelectObject(&penDotBlack);
		

		if (rect.right > m_rGraph.right) {
			rect.right  = m_rGraph.right;
			rect.left   = rect.right - 55;
		}
		if (rect.left < m_rGraph.left) {
			rect.left   = m_rGraph.left;
			rect.right  = rect.left + 55;
		}
		if (rect.top < m_rGraph.top) {
			rect.top    = m_rGraph.top;
			rect.bottom = rect.top + 20;
		}
 		if (rect.bottom > m_rGraph.bottom) {
			rect.bottom = m_rGraph.bottom;
			rect.top    = rect.bottom - 20;
		}

		memDC->FillSolidRect(&rect, RGB(255, 220, 200));

		str.Format(_T("%.3f"), GetY(m_DataGraphIx, m_DataPointIx));
//		memDC->SetTextColor(RGB(128, 128, 128));
//		memDC->DrawText(str, &rect, DT_SINGLELINE|DT_RIGHT|DT_VCENTER);
		RectF m_ValRect(rect.left, rect.top, rect.Width(), rect.Height());
		graphics.DrawString(str, -1, &fontValue, m_ValRect, &format , &txtBrush);
//		memDC->SelectObject(ppenOld);
	}
}

BOOL CLIMGraph::DoGraph(CDataGraph *graph)
{
	SetXAxisScale(graph, graph->GetXAxisMin(), graph->GetXAxisMax());
	SetYAxisScale(graph, graph->GetYAxisMin(), graph->GetYAxisMax()); 

	return TRUE;
}

void CLIMGraph::DoPlotXY(CDataGraph *graph, CDC *memDC)
{
	UINT prevx=0;
	UINT prevy=0;
	bool firstpoint = true;

	LONG result;
	UINT xstart;
	UINT ystart;

	UINT pixelx;
	UINT pixely;



	result = ConvertToGraphCoords(m_dXAxisMin, m_dYAxisMin);
	if (result == -1)
		return;

	xstart = LOWORD(result);									//the left hand side of the graph plot area on screen
	ystart = HIWORD(result);									//shouldbe the bottom of the graph plot area on screen
	


	CPen *op;
	CPen ps;
	ps.CreatePen(PS_SOLID, 1, graph->GetColor());
	op = memDC->SelectObject(&ps);

	float x;
	float y;

	for (UINT idx = 0; idx < graph->GetSize(); idx++)
	{
		
		x = graph->GetX(idx);
		y = graph->GetY(idx);

		if ((x < m_dXAxisMin) || (x > m_dXAxisMax))
		{//точка не попадает в поле графика
			continue; //NEXT !!!!!!
		}
		
		if (y < m_dYAxisMin)
		{
			y = m_dYAxisMin;
		}
		if (y > m_dYAxisMax)
		{
			y = m_dYAxisMax;
		}

		pixelx = (unsigned)(xstart + (x - m_dXAxisMin)*m_dPixelsPerX);
		pixely = (unsigned)(ystart - (y - m_dYAxisMin)*m_dPixelsPerY);

		if (firstpoint) {
			firstpoint = false;
			prevx = pixelx;
			prevy = pixely;
		}

		PlotPoints(memDC, graph, pixelx, pixely, prevx, prevy);

		//current point becomes previous point
		prevx = pixelx;
		prevy = pixely;
	}

	memDC->SelectObject(op);
}


void CLIMGraph::PlotPoints(CDC *memDC, CDataGraph * graph, UINT x, UINT y, UINT prevx, UINT prevy)
{
	DrawConnectLine(memDC, graph, prevx, prevy, x, y);
	DrawEndLine(memDC, graph, x, y);
}

void CLIMGraph::DrawConnectLine(CDC *memDC, CDataGraph * graph, UINT FromX, UINT FromY, UINT ToX, UINT ToY)
{
	Graphics	 graphics(memDC->m_hDC);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
	graphics.SetSmoothingMode    (SmoothingModeAntiAlias);
	Color clr;
	clr.SetFromCOLORREF(graph->GetColor());

	REAL wd = 1.0f;
	if (m_DataPointIx != -1) {
		wd = 2.0f;
	}
	Gdiplus::Pen  pen(clr, wd);

	graphics.DrawLine(&pen, (INT)FromX, (INT)FromY, (INT)ToX, (INT)ToY); 
}

void CLIMGraph::DrawEndLine(CDC *memDC, CDataGraph * graph, UINT X, UINT Y)
{
	Graphics	 graphics(memDC->m_hDC);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
	graphics.SetSmoothingMode    (SmoothingModeAntiAlias);
	Color clr;
	clr.SetFromCOLORREF(graph->GetColor());
	Gdiplus::SolidBrush  brush(clr);
	REAL wd = 1.0f;
	if (m_DataPointIx != -1) {
		wd = 2.0f;
	}
	Gdiplus::Pen  pen(clr, wd);

	graphics.FillEllipse(&brush, (INT)X-2, (INT)Y-2, 4, 4); 
	graphics.DrawEllipse(&pen, (INT)X-2, (INT)Y-2, 4, 4); 
}


LONG CLIMGraph::ConvertToGraphCoords(float x, float y)
{
	
	LONG result = -1;
	//to be plottable on the graph the given x-value must be between
	//x-min and x-max
	if(x < m_dXAxisMin || x > m_dXAxisMax)
	{
		return result;
	}
	if( y <m_dYAxisMin || y > m_dYAxisMax)
	{
		return result;
	}

	//calc the abs difference between Xmin and x;
	float xdif = fabs(m_dXAxisMin - x);
	//calc the abs difference between Ymin and y;
	float ydif = fabs(m_dYAxisMin - y);

	int xpos = (int)(m_rGraph.left + (xdif*m_dPixelsPerX));				//from left
	int ypos = (int)(m_rGraph.top + m_rGraph.Height() - (ydif*m_dPixelsPerY));			//from bottom

	result = MAKELONG(xpos,ypos);	

	return result;
}

void CLIMGraph::ClearData()
{
	int size = GetSize();

	CDataGraph *pGraph;

	for (int i=0; i<size; i++) {
		pGraph = GetDataGraph(i);
		pGraph->Clear();
	}
	BeginFromStart = 0.0f;
	TimeFromStart  = 0.0f;
}


void CLIMGraph::drawXAxisScale(CDC *memDC)
{
	CString szXFormat;
	CString szScale;
	int   tmp;
	CRect rect;
	TEXTMETRIC textmetrics; 


	Graphics	 graphics(memDC->m_hDC);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
	graphics.SetSmoothingMode    (SmoothingModeAntiAlias);
	graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);
	Gdiplus::FontFamily    fontFamily(L"Arial");
	Gdiplus::Font          fontValue (&fontFamily, (REAL)m_rGraph.Height()*0.04f, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
	Gdiplus::StringFormat format;
	format.SetAlignment(Gdiplus::StringAlignmentCenter);
	format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
	Color clr;
	clr.SetFromCOLORREF(RGB(0, 0, 0));
	Gdiplus::SolidBrush  txtBrush(clr);


	CFont *oldFont = memDC->SelectObject(&m_fontScale);

	memDC->GetTextMetrics(&textmetrics);
 	int m_iCharHeight	= textmetrics.tmHeight;
 	int m_iCharWidth	= textmetrics.tmAveCharWidth;

	memDC->SetTextColor(RGB(0,0,0));
	memDC->SetBkMode(TRANSPARENT);

//надпись слева
	rect.top	= m_rScaleX.top;
	rect.bottom	= m_rScaleX.bottom;
	rect.top	+= 5;
	rect.bottom	+= 5;


	float tmp2=(m_dXAxisMax-m_dXAxisMin)/10.0f;
	//Чтобы не отображать кучу чисел после запятой
	if(tmp2>10) tmp=0;
	else if(tmp2>1) tmp=0;
	else if(tmp2>0.1f) tmp=0;
	else if(tmp2>0.01f) tmp=1;
	else if(tmp2>0.001f) tmp=2;
	else if(tmp2>0.0001f) tmp=3;
	else if(tmp2>0.00001f) tmp=4;
	else tmp=1;

	szXFormat.Format(_T("%%0.%uf"), tmp);
	
	float dX = (float)m_rScaleX.Width()/(float)m_nGridMainX;

	int di = 1;
	if ((8 * m_iCharWidth) > dX) {
		di = (8 * m_iCharWidth) / dX + 1;
	}
	for(int i=0;i<11;i+=di)
	{
		rect.left	= m_rScaleX.left + (int)((float)i*dX);
		rect.right	= rect.left + 4*m_iCharWidth;
		rect.left	= rect.left - 4*m_iCharWidth;


		tmp2 = m_dXAxisMin + (m_dXAxisMax-m_dXAxisMin)*(float)i/(float)m_nGridMainX;


		szScale.Format(szXFormat, tmp2);

//		memDC->Rectangle(&rect);	
//		memDC->DrawText(szScale, &rect, DT_NOCLIP|DT_CENTER|DT_VCENTER);
		RectF m_ValRect(rect.left, rect.top, rect.Width(), rect.Height());
		graphics.DrawString(szScale, -1, &fontValue, m_ValRect, &format , &txtBrush);
	}

	CDataGraph *pData;
	pData = GetDataGraph(m_DataGraphIx);

//	memDC->DrawText(_TEXT("Частота вращения, об/мин"), &m_rTitleX, DT_NOCLIP|DT_CENTER|DT_VCENTER);
	RectF m_TitRect(m_rTitleX.left, m_rTitleX.top, m_rTitleX.Width(), m_rTitleX.Height());
	graphics.DrawString(pData->GetXAxisName(), -1, &fontValue, m_TitRect, &format , &txtBrush);

	memDC->SelectObject(oldFont);
}

void CLIMGraph::drawYAxisScale(CDC *memDC)
{
	CString szYFormat;
	CString szScale;
	int   tmp;
	CRect rect;
	TEXTMETRIC textmetrics; 


	Graphics	 graphics(memDC->m_hDC);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
	graphics.SetSmoothingMode    (SmoothingModeAntiAlias);
	graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);
	Gdiplus::FontFamily    fontFamily(L"Arial");
	Gdiplus::Font          fontValueScale (&fontFamily, (REAL)m_rGraph.Height()*0.04f, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
	Gdiplus::Font          fontValueTitle (&fontFamily, (REAL)m_rGraph.Height()*0.05f, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
	Gdiplus::StringFormat format;
	format.SetAlignment(Gdiplus::StringAlignmentCenter);
	format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
	Color clr;
	clr.SetFromCOLORREF(RGB(0, 0, 0));
	Gdiplus::SolidBrush  txtBrush(clr);





	CFont *oldFont = memDC->SelectObject(&m_fontScale);

	memDC->GetTextMetrics(&textmetrics);
 	int m_iCharHeight	= textmetrics.tmHeight;
 	int m_iCharWidth	= textmetrics.tmAveCharWidth;

	memDC->SetTextColor(RGB(0,0,0));
	memDC->SetBkMode(TRANSPARENT);

//надпись слева
	rect.left	= m_rScaleY.left;// - 7*m_iCharWidth;
	rect.right	= m_rScaleY.right;// - 1*m_iCharWidth; //allow for 6 characters

	float tmp2=(m_dYAxisMax-m_dYAxisMin)/10.0f;
	//Чтобы не отображать кучу чисел после запятой
	if(tmp2>10) tmp=1;
	else if(tmp2>1) tmp=2;
	else if(tmp2>0.1f) tmp=3;
	else if(tmp2>0.01f) tmp=4;
	else if(tmp2>0.001f) tmp=5;
	else if(tmp2>0.0001f) tmp=6;
	else if(tmp2>0.00001f) tmp=7;
	else tmp=1;
	
	szYFormat.Format(_T("%%0.%uf"), tmp);

	float dY = ((float)m_rScaleY.Height())/10.0f;

	for(int i=0; i<11; i++)
	{
		rect.top	= m_rScaleY.bottom - (int)((float)i*dY) - (int)((float)m_iCharHeight/2.0f);
		rect.bottom	= rect.top + m_iCharHeight;

		tmp2 = m_dYAxisMin + (m_dYAxisMax-m_dYAxisMin)*(float)i/10.0f;

		szScale.Format(szYFormat, tmp2);

//		memDC->Rectangle(&rect);
//		memDC->DrawText(szScale, &rect, DT_NOCLIP|DT_RIGHT|DT_VCENTER);
		RectF m_ValRect(rect.left, rect.top, rect.Width(), rect.Height());
		graphics.DrawString(szScale, -1, &fontValueScale, m_ValRect, &format , &txtBrush);
	}
	memDC->SelectObject(oldFont);

//	oldFont = memDC->SelectObject(&m_fontScaleVert);
//	oldFont = memDC->SelectObject(&m_fontScale);
//	memDC->Rectangle(&m_rTitleY);
//    memDC->SetTextAlign(TA_CENTER);

	
	CDataGraph *pData;
	pData = GetDataGraph(m_DataGraphIx);

	RectF m_TitRect(m_rTitleY.left, m_rTitleY.top, m_rTitleY.Width(), m_rTitleY.Height());
	graphics.DrawString(pData->GetSerieName(), -1, &fontValueTitle, m_TitRect, &format , &txtBrush);
//	memDC->TextOut(m_rTitleY.left, (m_rTitleY.top+m_rTitleY.bottom)/2, pData->GetSerieName());
//	memDC->TextOut((m_rTitleY.left + m_rTitleY.right)/2, m_rTitleY.top, pData->GetSerieName());

//	memDC->DrawText(_TEXT("Частота вращения, об/мин"), &m_rTitleY, DT_SINGLELINE|DT_NOCLIP|DT_BOTTOM/*|DT_VCENTER*/);
//	memDC->SelectObject(oldFont);

}


bool CLIMGraph::OnLButtonDown(UINT nFlags, CPoint point)
{
	bool  ret;
	HCURSOR hCurs;

	ret = false;	

	if (m_DataPointIx != -1) {
		if (PtInRect(&m_rWnd, point)) {
			m_eState = SEL_LBTN;
			m_eAct   = ACT_YMOVE;
		}

		switch (m_eAct) {
			case ACT_NONE:
				ret = true;
				break;

			case ACT_YMOVE:
				hCurs = LoadCursor(NULL, IDC_SIZENS);
				SetCursor(hCurs);
				m_iYMoveFirst = point.y;
				break;
		}
	}  else {
		hCurs = LoadCursor(NULL, IDC_CROSS);
		SetCursor(hCurs);
	}
	return ret;
}


bool CLIMGraph::OnLButtonUp(UINT nFlags, CPoint point)
{
	HCURSOR hCurs;
	bool  ret;


	ret = false;

	float dY = GetY(m_DataGraphIx, m_DataPointIx);

	if (m_eState == SEL_LBTN) {
		switch (m_eAct) {
			case ACT_NONE:
				break;

			case ACT_YMOVE:
/*				if (m_iCYMove !=0 )
				{
					//  assume the inside dimensions are correct.
					float eYDataRange = m_dYAxisMax - m_dYAxisMin;
					float eYPixelRange = (float)m_rGraph.Height();
					float eYRatio = eYDataRange / eYPixelRange;

					//  get distance of bottom edge of rect from inside bottom edge.
					float eYNew = dY + (m_iCYMove) * eYRatio;
					SetY(m_DataGraphIx, m_DataPointIx, eYNew);
				}
				m_iCYMove = 0;
				ret = true;*/	
				break;

		}
		m_eState = SEL_NONE;
		m_eAct   = ACT_NONE;
	}

	if (PtInRect(&m_rGraph, point)) {
		hCurs = LoadCursor(NULL, IDC_CROSS);
		SetCursor(hCurs);
	}

	return ret;
}

bool CLIMGraph::OnMouseMove(UINT nFlags, CPoint point)
{
	HCURSOR hCurs;
	bool  ret;


	ret = false;

	float dY = GetY(m_DataGraphIx, m_DataPointIx);

	switch (m_eAct) {
		case ACT_NONE:
			if (PtInRect(&m_rWnd, point)) {
				if (PtInRect(&m_rGraph, point)) {
					m_ptCursor = point;
					hCurs = LoadCursor(NULL, IDC_CROSS);
					SetCursor(hCurs);
				}
			}
			FindPoint(point);

			break;

		case ACT_YMOVE:
			m_iCYMove = m_iYMoveFirst - point.y;
			hCurs = LoadCursor(NULL, IDC_SIZENS);
			SetCursor(hCurs);

//			if (PtInRect(&m_rGraph, point)) {
				//  assume the inside dimensions are correct.
				float eYDataRange = m_dYAxisMax - m_dYAxisMin;
				float eYPixelRange = (float)m_rGraph.Height();
				float eYRatio = eYDataRange / eYPixelRange;

				//  get distance of bottom edge of rect from inside bottom edge.
				float eYNew = dY + (m_iCYMove) * eYRatio;
				eYNew = eYNew > m_dYAxisMax ? m_dYAxisMax : eYNew;
				eYNew = eYNew < m_dYAxisMin ? m_dYAxisMin : eYNew;
				SetY(m_DataGraphIx, m_DataPointIx, eYNew);
				m_iYMoveFirst = point.y;
				m_iCYMove = 0;
				m_ptCursor = point;
//			} else {
//				m_eAct = ACT_NONE;
//			}

			break;
	}
	
	return ret;
}


void CLIMGraph::FindPoint(CPoint point)
{
	int size = GetSize();
	int csize;
 	float yy = 0.0f;

	CDataGraph *pGraph;

	if (size > 0 && m_DataGraphIx != -1) {
		pGraph = GetDataGraph(m_DataGraphIx);
		csize = pGraph->GetSize();
		if (pGraph->IsVisible() && csize > 0) {
			DoGraph(pGraph);
			m_DataPointIx = FindPointIx(pGraph, point);
		}
	}
}

int CLIMGraph::FindPointIx(CDataGraph *pGraph, CPoint point)
{
	LONG result;
	UINT xstart;
	UINT ystart;

	UINT pixelx;
	UINT pixely;



//	float xperpixel = m_rGraph.Width/(graph->m_dXAxisMax - graph->m_dXAxisMin);
//	float yperpixel = m_rGraph.Height/(graph->m_dYAxisMax - graph->m_dYAxisMin);

	result = ConvertToGraphCoords(m_dXAxisMin, m_dYAxisMin);
	if (result == -1)
		return 0.0f;

	xstart = LOWORD(result);									//the left hand side of the graph plot area on screen
	ystart = HIWORD(result);									//shouldbe the bottom of the graph plot area on screen
	


	float x;
	float y;

	for (UINT idx = 0; idx < pGraph->GetSize(); idx++)
	{
		
		x = pGraph->GetX(idx);
		y = pGraph->GetY(idx);

		if ((x < m_dXAxisMin) || (x > m_dXAxisMax))
		{//точка не попадает в поле графика
			continue; //NEXT !!!!!!
		}
		
		if (y < m_dYAxisMin)
		{
			y = m_dYAxisMin;
		}
		if (y > m_dYAxisMax)
		{
			y = m_dYAxisMax;
		}

		pixelx = (unsigned)(xstart + (x - m_dXAxisMin)*m_dPixelsPerX);

		pixely = (unsigned)(ystart - (y - m_dYAxisMin)*m_dPixelsPerY);

		if (pixely + 3.0f >= point.y && pixely - 3.0f <= point.y && pixelx - 3.0f <= point.x && pixelx + 3.0f >= point.x) {
			return idx;
		}
	}

	return -1;
}
