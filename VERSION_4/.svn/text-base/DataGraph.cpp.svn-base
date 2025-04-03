#include "StdAfx.h"
#include "DataGraph.h"

CDataGraph::CDataGraph(void)
{
}

CDataGraph::~CDataGraph(void)
{
	m_Point.clear();
}

BOOL CDataGraph::Add(float x, float y)
{
	if(m_Point.size() >= m_Point.max_size())
	{
		return FALSE;
	}
	else
	{
		m_Point.push_back(CDataPoint(x, y));
	}

	return TRUE;
}

void CDataGraph::Del(int pos) {
//	m_Point.erase(m_Point.at(pos));
}

void CDataGraph::Clear()
{
	if (m_Point.size() > 0)
		m_Point.clear();
}

unsigned int CDataGraph::GetSize(void)
{
	return (unsigned int) m_Point.size();
}

float CDataGraph::GetX(unsigned int pos)
{
	if (pos <= GetSize())
		return m_Point[pos].GetX();
	else
		return 0.0f;
}


float CDataGraph::GetY(unsigned int pos)
{
	if (pos <= GetSize())
		return m_Point[pos].GetY();
	else
		return 0.0f;
}

void CDataGraph::SetY(unsigned int pos, float val)
{
	if (pos <= GetSize())
		m_Point[pos].SetY(val);
}


void CDataGraph::SetSerieName(CString str)
{
	szSerieName = str;
}

CString& CDataGraph::GetSerieName(void)
{
	return szSerieName;
}


void CDataGraph::SetXAxisName(CString str)
{
	szXAxisName = str;
}

CString& CDataGraph::GetXAxisName(void)
{
	return szXAxisName;
}


void CDataGraph::SetYAxisName(CString str)
{
	szYAxisName = str;
}

CString& CDataGraph::GetYAxisName(void)
{
	return szYAxisName;
}


void CDataGraph::SetXAxisUnit(CString str)
{
	szXAxisUnit = str;
}

CString& CDataGraph::GetXAxisUnit(void)
{
	return szXAxisUnit;
}


void CDataGraph::SetYAxisUnit(CString str)
{
	szYAxisUnit = str;
}

CString& CDataGraph::GetYAxisUnit(void)
{
	return szYAxisUnit;
}



void CDataGraph::SetYAxisValue(float vmin, float vmax)
{
	m_dYAxisMax = vmax;
	m_dYAxisMin = vmin;
}

float CDataGraph::GetYAxisMin(void)
{
	return m_dYAxisMin;
}

float CDataGraph::GetYAxisMax(void)
{
	return m_dYAxisMax;
}

void CDataGraph::SetYCurValue(float vmin, float vmax)
{
	m_dYCurMax = vmax;
	m_dYCurMin = vmin;
}

float CDataGraph::GetYCurMin(void)
{
	return m_dYCurMin;
}

float CDataGraph::GetYCurMax(void)
{
	return m_dYCurMax;
}

void CDataGraph::SetXAxisValue(float vmin, float vmax)
{
	m_dXAxisMax = vmax;
	m_dXAxisMin = vmin;
}

float CDataGraph::GetXAxisMin(void)
{
	return m_dXAxisMin;
}

float CDataGraph::GetXAxisMax(void)
{
	return m_dXAxisMax;
}

void CDataGraph::SetXCurValue(float vmin, float vmax)
{
	m_dXCurMax = vmax;
	m_dXCurMin = vmin;
}

float CDataGraph::GetXCurMin(void)
{
	return m_dXCurMin;
}

float CDataGraph::GetXCurMax(void)
{
	return m_dXCurMax;
}


void  CDataGraph::SetColor(COLORREF clr)
{
	m_clrGraph = clr;
}

COLORREF  CDataGraph::GetColor(void)
{
	return m_clrGraph;
}


void  CDataGraph::SetActive(BOOL state)
{
	m_bActive = state;
}

BOOL  CDataGraph::IsActive(void)
{
	return m_bActive;
}

void  CDataGraph::SetVisible(BOOL state)
{
	m_bVisible = state;
}

BOOL  CDataGraph::IsVisible(void)
{
	return m_bVisible;
}


//сохранение значения Y в точке пересечения палки курсора
void  CDataGraph::SetXY(float y)
{
	m_YValue = y;
}

//получение значения Y в точке пересечения палки курсора
float CDataGraph::GetXY(void)
{
	return m_YValue;
}

