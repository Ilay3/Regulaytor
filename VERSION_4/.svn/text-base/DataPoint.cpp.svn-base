#include "StdAfx.h"
#include "DataPoint.h"

CDataPoint::CDataPoint(void)
{
	X = 0.0f;
	Y = 0.0f;
}

CDataPoint::CDataPoint(const CDataPoint& pt)
{
	X = pt.X;
	Y = pt.Y;
}

CDataPoint::CDataPoint(float x, float y)
{
	X = x;
	Y = y;
}

CDataPoint::~CDataPoint(void)
{
}

CPoint CDataPoint::ConvertToCPoint(void)
{
	return CPoint((int)X, (int)Y);
}

float CDataPoint::GetX(void)
{
	return X;
}

float CDataPoint::GetY(void)
{
	return Y;
}

void CDataPoint::SetY(float val)
{
	Y = val;
}

CDataPoint CDataPoint::operator*(unsigned int n)
{
	return CDataPoint(X * n, Y * n);
}

CDataPoint& CDataPoint::operator= (const CDataPoint& pt)
{
	X = pt.X;
	Y = pt.Y;

	return *this;
}

CDataPoint  CDataPoint::operator + (CDataPoint& pt)
{
	return CDataPoint (X + pt.X, Y + pt.Y);
}

CDataPoint  CDataPoint::operator - (CDataPoint& pt)
{
	return CDataPoint (X - pt.X, Y - pt.Y);
}

void CDataPoint::operator += (CDataPoint& pt)
{
	X += pt.X;
	Y += pt.Y;
}

void CDataPoint::operator -= (CDataPoint& pt)
{
	X -= pt.X;
	Y -= pt.Y;
}

float CDataPoint::operator ! (void)
{
	return fabs(X) + fabs(Y);
}
