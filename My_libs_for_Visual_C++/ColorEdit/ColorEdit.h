#pragma once

class CColorEdit : public CEdit
{
// Construction
public:
	CColorEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReadOnlyEdit)
	//}}AFX_VIRTUAL

// Implementation
public:			
	void SetTextColor(COLORREF rgb);
	void SetBackColor(COLORREF rgb);
	virtual ~CColorEdit();

	// Generated message map functions
protected:
	//text and text background colors
	COLORREF m_crText;
	COLORREF m_crBackGnd;
	//background brush
	CBrush m_brBackGnd;
	//{{AFX_MSG(CReadOnlyEdit)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
