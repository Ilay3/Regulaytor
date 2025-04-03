#pragma once
#include "DataGraph.h"

using namespace Gdiplus;

class CLIMGraph
{
public:
	CLIMGraph(void);
	CLIMGraph(CRect* rect, CWnd* pWnd);
	~CLIMGraph(void);

	CWnd *pParent;

	CDC      *m_pDC;
//	CCriticalSection m_CritSection;

	CDC       m_dcBackground ;
	CBitmap*  m_pBitmapOldBackground ;
	CBitmap   m_bitmapBackground ;

	CRect     Rect_Graph;
	CRect     Rect_Area;
	CRect     Rect_Legend;

	COLORREF  m_clrBkGnd;			// ���� ���� 
	COLORREF  m_clrBkGraph;			// ���� ���� ���� �������
	COLORREF  m_clrTitle;			// ���� ������ ���������
	COLORREF  m_clrBkTitle;			// ���� ���� ���������
	COLORREF  m_clrBkTrend;			// ���� ���� ���� �������
	COLORREF  m_clrGrid;			// ���� ���� ���� �������

	CRect     m_rWnd;		   // �� ����
	CRect     m_rTitle;		   // ���� ��� ���������
	CRect     m_rLegend;	   // ���� ��� �������
	CRect     m_rGraph;		   // ���� ��� ����� �������
	CRect     m_rScaleX;	   // ���� ��� ����� �
	CRect     m_rScaleY;	   // ���� ��� ����� Y
	CRect     m_rMarkX;			// ���� ��� ������ �
	CRect     m_rMarkY;			// ���� ��� ������ Y
	CRect     m_rDimX;		   // ���� ��� ��.�����. �� �
	CRect     m_rDimY;		   // ���� ��� ��.�����. �� Y
	CRect     m_rTitleX;	   // ���� ��� ������� ����� �
	CRect     m_rTitleY;	   // ���� ��� ������� ����� Y
	CRect     m_rCurValGraph;  // ���� ��� ����� �������� XY ������� ��� ��������

	
	
	CString   m_sTitle;

	int       m_nGridMainX;
	int       m_nGridMainY;
	int       m_nGridSecX;
	int       m_nGridSecY;

	BOOL      m_bGridBig;
	BOOL      m_bGridSmall;


    float             m_eXMin;
    float             m_eXMax;
    float             m_eYMin;
    float             m_eYMax;
	float             m_eXRatio;
    float             m_eYRatio;

	CFont m_fontScale;
	CFont m_fontScaleVert;
	CFont m_fontLegend;
	


	CRect r_wnd;
	CRect r_graph;
	CRect r_legend;
	int m_iGraphX;				//������������ ������� � ����
	int m_iGraphY;				//
	int m_iGraphWidth;			//
	int m_iGraphHeight;			//

	int m_iOriginX;				//������������ ������ �� �������
	int m_iOriginY;				//

	float m_dXAxisMin;			//start value of X	in %
	float m_dYAxisMin;			//start value of Y	in %
	float m_dXAxisMax;
	float m_dYAxisMax;

	float m_dXCurMin;			//
	float m_dYCurMin;			//
	float m_dXCurMax;
	float m_dYCurMax;

	float m_dXMin;			//start value of X	in %
	float m_dYMin;			//start value of Y	in %
	float m_dXMax;
	float m_dYMax;

	float m_dPixelsPerY;		//scaling
	float m_dPixelsPerX;		//scaling	

	float  VisibleTime;
	float  TimeFromStart;
	float  BeginFromStart;

	void   ClearData();



	void         SetTitle(CString str);




	CPoint m_ptCursor;

	void AddData(int idx, float val, float fidx);
	void SetData(int idx, float val, float fidx);

	unsigned int GetSize(void);
	unsigned int GetDataSize(int pos);
	CDataGraph*  GetDataGraph(unsigned int pos);
	BOOL         AddDataGraph(CDataGraph data);
	void         AddPoint(int pos, float x, float y);
	void         DelPoint(int gpos, int ppos);

	//��������� �������� X �������� ����� �������
	float        GetX(int pos_data, int pos_point);
	//��������� �������� Y �������� ����� �������
	float        GetY(int pos_data, int pos_point);
	//������� �������� Y �������� ����� �������
	void         SetY(int pos_data, int pos_point, float val);

	void         SetSerieName(int pos, CString str);
	CString      GetSerieName(int pos);

	void         SetXAxisName(int pos, CString str);
	CString      GetXAxisName(int pos);

	void         SetYAxisName(int pos, CString str);
	CString      GetYAxisName(int pos);

	void         SetXAxisUnit(int pos, CString str);
	CString      GetXAxisUnit(int pos);

	void         SetYAxisUnit(int pos, CString str);
	CString      GetYAxisUnit(int pos);

	// Render the picture
	void Render(CDC *dc, CRect *rect, BOOL bPrint);
    
	// Draw methods :
	void drawBackground(CDC *pDC);
	void drawGrid(CDC *pDC);
	// Graphs methods :
	void drawLineGraph(CDC* memDC);
	void drawTip(CDC* memDC);
	void drawBar(CDC* memDC);

	// Full redraw
	void Reconstruct();




	BOOL DoGraph(CDataGraph *graph);
	void SetXAxisScale(CDataGraph *graph, float min, float max);
	void SetYAxisScale(CDataGraph *graph, float min, float max);
	void DoPlotXY(CDataGraph *graph, CDC *memDC);
	LONG ConvertToGraphCoords(float x, float y);
	void DrawConnectLine(CDC *memDC, CDataGraph * graph, UINT FromX, UINT FromY, UINT ToX, UINT ToY);
	void DrawEndLine(CDC *memDC, CDataGraph * graph, UINT X, UINT Y);
	void PlotPoints(CDC *memDC, CDataGraph * graph, UINT x, UINT y, UINT prevx, UINT prevy);

	void  drawXAxisScale(CDC *memDC);
	void  drawYAxisScale(CDC *memDC);

	void  DoPlotSelGraphXY(CDataGraph *graph, CDC *memDC);


	void UpdateGraphRect(CRect *rect);

	void FindPoint(CPoint point);
	int  FindPointIx(CDataGraph *pGraph, CPoint point);

	// Graph setup methods :
	// Initialise the graph
	void DrawBackground(COLORREF color);
	void DrawGraphAreaGradient(COLORREF color, int Decay, enum target) {}

	enum {
		SEL_NONE = 0,
		SEL_LBTN
	};
	int  m_eState;

	enum {
		ACT_NONE = 0,
		ACT_YMOVE
	};
	int  m_eAct;

	bool OnLButtonDown(UINT nFlags, CPoint point);
	bool OnLButtonUp(UINT nFlags, CPoint point);
	bool OnMouseMove(UINT nFlags, CPoint point);

	int m_iCYMove, m_iYMoveFirst;


	int  m_DataGraphIx;
	int  m_DataPointIx;
public:
	VECTOR_DATA  m_Data;
};
