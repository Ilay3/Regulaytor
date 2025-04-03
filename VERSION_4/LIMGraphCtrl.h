#pragma once
#include "LIMGraph.h"


// CLIMGraphCtrl

#define WM_LIMGRAPH_TO_PARAMS		(WM_USER+200)


class CLIMGraphCtrl : public CStatic
{
public:
	CLIMGraphCtrl(void);
	~CLIMGraphCtrl(void);

	CLIMGraph* m_pGraph;

public:
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT uID);

	void ReSize(CRect *rect);

	void UpdateGraph(void);
	void ReconstructGraph(void);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	void UpdateParams(void);
};


