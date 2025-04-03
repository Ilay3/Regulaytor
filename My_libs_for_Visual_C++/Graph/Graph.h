/*********************************************************************************************************************************
*		���� ������ �������� �� ���������� ������� �� ������
*****************************************************************************************/

/**********************		���� ����������� ���������	***********************************
*	21.11.2006 - ������ �� ��� Y ��������� ������������ ������� �����
*	4.1.2007 - ������ �� ��� Y ��� ��-��������, ��������� � ������� �����
*	5.1.2007 - ��������� ��������� �������� AlwaysDown � �������
*	17.1.2007 - m_GraphName ���������� � public, ��� ���������� ����� �������� �������
*	22.1.2007 - ��������� ��������� ������ ������
*				+ ������ ������� �� ��� � �������� ��������� � ��� ����� �������
*	1.02.2007 - ������� �������� ������� ����� ������� � ����� ������� �� ���������
*				+ ����������� ������������ � �������� ������������� ������� � ��������� �� ���������
*				+ �������� ������ ��������� �������������� ��� ������������� ��������� ������� � 
*					���������� �������� AlwaysDown
*	27.02.2007 - ������� ���������������� ����������� ������� ������ �� �������� ����������
*	5.11.2007	- ��� ������� ������ �������� ����� GraphicOptions. ����� ������������ ����������
*					������� ��� ������������ �����.
*	7.11.07 - ��������� ������� SaveGraphMathcad
*	24.11.07 - ��������� ������� GetVisibleFunctionsCount(), �������� ����������� ����� �������
*	3.12.07 - �������� ���������� ������ � ������� GetMoveAxisOffsetTime
*	17.01.08 - ���� ���������� ���������, ������ ������ - ����, �������� ��������� ���� ���������
*				����� �� ������� (� ������� � �����) ����� �������� � �������. ������ ���� 
*				�������������� ������ ������ �� ���� ������� � �������� �������. ���������� 
*				������ ������ ��������� ��������� ����������.
*	11.02.08 - ������ ����� �� ������� ��������������� ���� � ������, ������� ������� �� ����������
*				�� �������. ����� ������� � ProporCoef!=1 ������ ����� ��������� ����������.
*	10.03.08 - �������� � ������� �� ���� ����� ��������������� ����� ������
*	11.03.08 - ������ ������ ������� ver 1.1, ��� ���� ��������� � ������� ���������� �������, � 
*				����� ����������� �������� �������. �������� ������ ������ ����� �������. ����� ��
*				������������ ��� ��������������� � ���������������� ���������. ������� ��������� 
*				�������� ����������.
*	4.04.08 - ��� ��������� ����� ��������� ������� �� � ����� ���������������
*	5.04.08 - ����� ��������, �������� �� ������� ������� ������� ������ ���������������.
*				� ���������� ������� ��������� ��������� ���.
*	7.04.08 - ��������� ������� ConvertToGraph_X � ConvertToGraph_Y. ���������� ��������� �����
*				��� ���������� ������� ���������
*	30.04.08 - ������ ��������� �� ������, ����� ����� ������������ ������� ��� �, � �������� ����
*				��������� �� ������ ���� �� ����� �������� �������� �� ����.
*	7.04.08 - ��������� ������� GetMaxGraphScreenValue � GetMinGraphScreenValue
*	4.06.08 - ������ ������� ���������, ��� ������� ��� ����� �������� ������� ����������� ��������
*				���� ��� ������, �.�. ������� ���������� ������ � ����������. ����� ������ ���������.
*	10.07.08 - ���������� ���� ������ � DrawBarWide
*	21.07.08 - �������� UNICODE, ������ �� ��� �� ������������, ��������� ������� FromFuncNameToFuncNum
*				��������� ����� � DrawXLegend, ����� ������ ���������� �� � ����� ������� ���� �������
*	22.07.08 - ��������� ����������� ���������� ����������������, � ����� ���� ������ ������.
*	17.09.08 - ��������� ��� � ���������� ������� ����������� ������ �� ��������� AlwaysDown
*	24.10.08 - ���������� ������ � ������� DrawBarTight_2
*	8.12.08 - ��������� ���, �� �������� ��� ������ ����� ��������� ����� �� ������ �������� ����
*	11.12.08 - ������� PrintGraph ������������ ����.
*	11.01.09 - ��� ��������� ������� ������ ����� ���������� ������� �� ������� ����.
*	12.01.09 - ����� ������� �� Arial.
*	13.01.09 - ������� ���������, ������ ����� ��������� ��������� ������� ������� � ��� ���������������
*	28.01.09 - ������� ���������� �������� ������� ������������ ����.
*	10.08.09 - ������� DrawBarWidth ������������ ColorEx � ������ ������� �����
*	27.12.24 - �������� ����� ����� ��������
*********************************************************************************************/

#pragma once
#include "FunctionClass\CDPoint.h"			//����� ����� �������
#include "FunctionClass\FunctionClass.h"	//����� �������
#include "ERRORS\ERRORS.h"					//����� �������������� ������
#include "GraphicOptions.h"					//����� ���������� ��������� � ����� �������
#include "types_visual_c++.h"				//��� ���� ������

#include <math.h>							//����������
#include <vector>							//��� ��������� �������
#include <algorithm>						//��������� ���������
#include <functional>	
#include <fstream>							//��� ������ � �������
#include <iosfwd>
#include <afx.h>
#include <FLOAT.h>							//��� ���������� float

using namespace std;

/*******************************************************************************************
*		������������� ������� ������
*********************************************************************************************/
class CGRAPH_Dlg;	//������������ ����
/*
��� ������ ������ � ������� �������, �����
	- ����� ������� ���������� "CGraph" � ������� ��� ��� "CREATE"
	- ��������� ��������� �������� � ��������� CGraphicOptions, ������� �����������
		���������� �� ���������!
	- ������� � ��������� �������
	- ��������� � CGraph ������ ������ ������� "StartTime(TRUE)"
	- �� � ��������� ������
*/


#define MSG_GR_FUNC_NAME_SEL (WM_USER+154)	//��� ��������� ������������, ����� ��������� ��������� ������

//��� ��������� ���������, ���������� �� �����, ���������� �� �������
struct CFuncPointSel
{
	size_t  m_FuncNum;			//����� ���������� �������
	size_t	m_PointNum;			//����� ���� ����� �� ������� pFunc
	
	CFuncPointSel(size_t FuncNum, size_t PointNum)
	{//�����������, ����� ������ ������ ���������
		m_FuncNum=FuncNum;
		m_PointNum=PointNum;
	}

	//������� ����������� ��������� �:
	// 1 - � ������� ������� �������
	// 2 - � ������� ������� �����
	bool operator <(const CFuncPointSel &b) const
	{
		if(m_FuncNum < b.m_FuncNum) return true;
		else if(m_FuncNum > b.m_FuncNum) return false;

		if(m_PointNum < b.m_PointNum) return true;
		else if(m_PointNum > b.m_PointNum) return false;

		return false;	//����� � �������� �� ����� ����
	}
};

//��� ��������� ���������, ���������� �� �������, ���������� �� �������
struct CFuncFuncSel
{
	size_t m_FuncNum;	//����� ���������� �������
	CFuncFuncSel(size_t FuncNum)
	{//�����������, ����� ������ ������ ���������
		m_FuncNum=FuncNum;
	}
};

//��� ��������� ���������, ���������� �� �����, ��� ���� ���������� ����� �������
struct CFuncNameRect
{
	size_t	m_FuncNum;		//����� �������
	CRect	m_FuncNameRect;	//���� �� �����
	CFuncNameRect(size_t FuncNum, CRect FuncNameRect)
	{
		m_FuncNum=FuncNum;
		m_FuncNameRect=FuncNameRect;
	}
};

class AFX_EXT_CLASS CGraph: public CWnd
{
	//DECLARE_DYNAMIC(CGraph);
public:
	void SetYLineAtLeft(bool AtLeft);

	/****************************************************
	*		�����������
	*****************************************************/
	CGraph();
	virtual BOOL Create(CWnd *pParentWnd, CGraphicOptions *pGraphicOptions, ERROR_Class *pError, UINT colorscheme);
	~CGraph();

	void SetYAxisScale(FP32 min, FP32 max);
	void SetXAxisScale(FP32 min,FP32 max);
	void SetGraphSizePos(int xPos, int yPos, int Width, int Height);
	void SetDefaultColorScheme(void);
	void SetColorScheme(int Scheme, BOOL bRedraw=FALSE);
	COLORREF GetMultiPlotPenColor(UINT PenNumber);
	bool CreateGraphFont(CString FaceName);
	

private:
	CWnd* m_pParent;
	ERROR_Class* m_pError;	//��������� ������
public:
	//	���������� � ������� ��� ����� ������������� ��������� �������
	CGraphicOptions *m_pGraphicOptions;

private:
#define GRG_NOTINITIALIZED	0x00 //������ �� ���������������
#define GRG_STOP			0x01 //������ ����������
#define	GRG_PAUSE			0x02 //������ �� �����
#define	GRG_PLAY			0x03 //������ � ������ ��������������� � �������� �������
#define GRG_RESTART			0x04 //����� ������������� ������
protected:
	INT8U	m_Regime;
	INT64U	m_StartTime;				//����� � ������������� ������ ���������� ������� (�� ��������� �����) (GetTickCount())
public:
	bool	SetRegime(INT8U Regime);
	INT8U	GetRegime();
	//������ �������� �� ������� ����� ������� ���������� ������� � �������� ������ �������
	//� ��������
	FP32 GetOffsetTime();
	//������������� �������� ����� ������� ������������ ������� (���� �� GRG_PLAY)
	bool SetX_Offset(FP32 x_offset);


protected:
	INT16U m_CountVisibleGraphs;	//����� ��������� ������� PaintGraph ���� ������� ���������� 
									//��������, ������� ������ � �� ��������� ������������
	INT16U m_thisFuncNumber;		//����� ���� ������� DrawFunction ��� �� �����, ����� �� �����
									//������ �������������, ������ ��� ���������� ��������� ��
									//���������� � �������

private:
	CPoint	ConvertToGraphCoords(FP32 x, FP32 y);	//��������������� ���������� ���������� � �������
	UINT	ConvertToGraph_X(FP32 x);				//���������� �� ��� �
	UINT	ConvertToGraph_Y(FP32 y);				//���������� �� ��� Y
	//���� ���������� ��������� ����� h_x,h_y � �������� �� ������� �������
	CPoint	ConvertToGraphCoordsEx(FP32 v_x, FP32 v_y, FP32 h_x, FP32 h_y);	
	bool	ConvertToReal_X(int x, FP32* pRealX, bool Cutting=false);
	bool	ConvertToReal_Y(int y, FP32* pRealY, bool Cutting=false);
	bool	IsPointVisible(FP32 x, FP32 y);			//����� �� ����� �� ������
	FP32 ConstrainY (FP32 y);
	
	/*****************************************************************************
	*		� ��� ��� ������� ���������� � ���������� ����������� ����� �������
	*****************************************************************************/
	void DrawFunctionName();						//����� �������
	bool DrawYAxisNumbers();						//����� �� ��� Y
	void DrawXAxisNumbers();						//����� �� ��� �
	bool DrawTicks();								//�������� �� ����
	bool DrawBackGround();							//���
	bool Draw_X_Axis();								//��� �
	bool Draw_Y_Axis();								//��� Y
	bool DrawGrid();								//�����
	void DrawYLegend();								//������� ��� Y
	void DrawXLegend();								//������� ��� X
	bool DrawGraphTitle();							//��� �������
	void DrawFunction(CFunctionClass* pFunction);	//���� �������
	//������������ ����� ��������������� �����
	void PlotPoints(CFunctionClass* pFunction,size_t PointNum, bool IsFocused=false);
	//������ �������� ������� � ����� �� �������
	void DrawBarTight(CFunctionClass* pFunction,size_t ThisPoint, bool IsFocused=false);
	//��������� �������� ��������� �� ���������, ���� ������ �������������
	void DrawBarTight_2(CFunctionClass* pFunction,size_t ThisPoint, bool IsFocused=false);
	//������ ������ �������, ������� �� ������� ����� ��������� �������
	void DrawBarWide(CFunctionClass* pFunction,size_t ThisPoint, bool IsFocused=false);
	//����� ����� �������
	void DrawConnectLine(CFunctionClass* pFunction,size_t ThisPoint, bool IsFocused=false);
	//������������� ����� �� �������
	void DrawDot (CFunctionClass* pFunction,size_t PointNum, bool IsFocused=false);
	//������ ������������� �������� �� ����� ������� ��� ��������� ����
	void GetMoveAxisOffsetTime(float &offset,float& step);

	
	/*****************************************************************************
	*		� ���� ����� ������ ����������� (� ������ ����� ������ �����������)
	*	������������� ��������� ��������� (�������� �������� ���� ������� �� ������)
	*	� �.�., ����� ����� �� ����� ����� ���������� �� 10 ��� �� �������� ����
	*	� �� �� �������, � ������������ ����������.
	*****************************************************************************/
	UINT m_RightMargin;			//|
	bool CalcRightMargin();		//|
	UINT m_LeftMargin;			//|
	bool CalcLeftMargin();		//|
	UINT m_BottomMargin;		//| ���������� � �������� �� ��������������
	bool CalcBottomMargin();	//| ������ �������
	UINT m_TopMargin;			//|
	bool CalcTopMargin();		//|
	CRect CalcDataArea(void);


	UINT m_iOriginX;			//| ���������� � �������� ������ �������
	UINT m_iOriginY;			//|
	FP32 m_dPixelsPerY;			//| ���������� �� ����
	FP32 m_dPixelsPerX;			//|
	FP32 m_dXAxisMin;			//| ������� ������������ �� �������������� ���
	FP32 m_dXAxisMax;			//|
	FP32 m_PixelsPerX_Tick;		//| ���������� �������� ����� ������� ����� �� ����
	FP32 m_PiselsPerY_Tick;		//|
	bool CalcAxys(void);		//| ��������������� �������� � �����

	//��, ��� �������� ������
	COLORREF m_crYTickColor;
	COLORREF m_crXTickColor;
	COLORREF m_crYLegendTextColor;
	COLORREF m_crXLegendTextColor;
	COLORREF m_crGraphTitleColor;
	COLORREF m_crGraphPenColor;
	COLORREF m_crGraphBkColor;
	COLORREF m_crGridColor;
	
	//��, ��� �������� �������
	CFont m_GraphFont;
	CString m_szFontFace;
	int m_iFontSize;
	int m_iCharHeight;
	int m_iCharWidth;


	
	
public:
	//������ ������� �������
	void SetGraphSize(int height, int width);
	void SetGraphPos(int x,int y);

	//������ ����� �������
	vector<CFunctionClass> VecFunctions;

	//���������� ������ �������
	bool SaveGraph(const TCHAR* FileName);

	//���������� ������ ������� � ������� ��� mathcad
	bool SaveGraphMathcad(const TCHAR* FileName, INT16S FuncNum=-1);

	//���������� ������ ������� � ������� BMP
	bool SaveGraphBMP(const TCHAR* FileName);

	//�������� ������ �������
	bool LoadGraph(const CHAR* FileName);
	bool LoadGraph(const WCHAR* FileName);
	//�� �� �����, �� ����� ������ ����������� � ������� (��������� ����� �������)
	bool LoadGraphToExists(const CHAR* FileName);
	bool LoadGraphToExists(const WCHAR* FileName);

	//������ ���������� ������� �������
	INT16U GetVisibleFunctionsCount();

	//�������, ���������� ���������� � ��������
	FP32 GetMaxGraphValue(FP32 FromTime=-FLT_MAX, FP32 ToTime=FLT_MAX);
	FP32 GetMinGraphValue(FP32 FromTime=-FLT_MAX, FP32 ToTime=FLT_MAX);
	FP32 GetMaxGraphScreenValue();	//|
	FP32 GetMinGraphScreenValue();	//| ������� ��� �������� �������
	FP32 GetMaxGraphTime();
	FP32 GetMinGraphTime();


	/****************************************************
	*					���������
	****************************************************/
	//� ������� ������������ ������� ����� ������������ ����
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	bool m_bRedraw;		//����� ���������� ��������� �� ������� - � 
						//���� ��������� ����������� � WM_PAINT, �.�.
						//����������� ��� �� ������
	CDC SecondDC;		//���� � ���� ������������ � ������� ������
	CBitmap SecondBitmap;//��� ���� �� �����
	static const UINT REDRAW_TMR_ID=0x00;

	/****************************************************
	*				��������� ����� � �.�.
	****************************************************/
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	//������ ������ ���������� ���� �� ���� ������ ������������
	CRect GetFocusRect(CPoint point_1,CPoint point_2);

	//������ �����, ������� � ������ ������ �������� � ������� ���������
	vector<CFuncPointSel> mas_VisiblePoints;
	//������ �����, ������� � ������ ������ �������� �� ������� ��������
	vector<CFuncPointSel> mas_SelectionPoints;
	//������ �������, ������� � ������ ������ �������� �� ������� ��������
	vector<CFuncFuncSel>  mas_SelectionFunctoins;	
	//������ ������, � ������� ���������� ����� �������
	vector<CFuncNameRect> mas_FuncNameRects;

	//����������, �������� �� ������ ����� ����������
	bool IsPointSelected(size_t FuncNum, size_t Num);
	//����������, �������� �� ������ ������� ����������
	bool IsFunctionSelected(size_t FuncNum);
	//������� (��� ���������) ���������� �����
	void DeleteSelectedPoints(bool Inverse=false);
	//������� (��� ���������) ���������� �������
	void DeleteSelectedFunctions(bool Inverse=false);
	//��������� ���������� ����� �� ��������
	void MoveSelectedPointsUp();
	void MoveSelectedPointsDown();
	void MoveSelectedPointsLeft();
	void MoveSelectedPointsRight();
	//���������� ���������� ������� �� ��������
	void MoveSelectedFunctionsUp();
	void MoveSelectedFunctionsDown();
	void MoveSelectedFunctionsLeft();
	void MoveSelectedFunctionsRight();

	#define INVALID_FUNC_NUM	0xFFFF	//�������� ��� �������� ���������� GetFuncNum
	//�� ����� ��������� �� ������� - �� ������ �� ����� � VecFunctions
	INT16U FromFuncPtrToFuncNum(CFunctionClass* ptr);
	//�� ����� ��� �������, �� ������ �� ����� � VecFunctions
	INT16U FromFuncNameToFuncNum(const TCHAR* FuncName);

	DECLARE_MESSAGE_MAP()

	//���������� �������������� ��� ��������� ������ ������� ����
	RECT	m_OldScreenRect;//���� ���� ���������� ������ ���� ��� ������ ClipCursor
	bool	m_bStartFocus;	//���� true, �� ��������� ��������
	CPoint	m_DownPoint;	//�����, ��� � ����� �� ����
	CRect	m_LastRect;		//�����, ����� ��������� ������ �����
	CPoint	m_ThisPoint;	//��� ������ ��������� ����

	//���������� �������������� ��� ��������� ����� ������� ����
	RECT	m_LOldScreenRect;
	#define ZOOM_NOT_ACTIVE		0x00	//�� ��� ������ ������ � ���������
	#define ZOOM_FOCUSING		0x01	//������������ ����� �����
	#define ZOOM_FOCUS_ENDED	0x02	//������������ ������� ����� ������� � ������ �������� �� ������
	INT8U	m_ZoomState;
	CRect	m_ZoomRect;
	CPoint	m_LDownPoint;
	CPoint	m_LThisPoint;
	bool	m_bCursorInFocus;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	//������� ���������� �������
	bool PrintGraph(void);
};
