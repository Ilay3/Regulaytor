#pragma once
#include <vector>
#include <math.h>
using namespace std;


class CDPoint
{//����� ������������ ����� � ������  � ����
public:
	float x;
	float y;
	//����������� �� ���������
	CDPoint () {x=0. ; y=0.;}
	//����������� �����������
	CDPoint ( const CDPoint& pt) 
	{
		x=pt.x;
		y=pt.y;
	}
	//����������� � �����������
	CDPoint ( float xx , float yy)
	{
		x=xx;
		y=yy;
	}
	//�������� ��������� ( ���������� � n ���)
	CDPoint operator*(UINT n)
	{
		return CDPoint(x*n,y*n);
	}

	//�������� ������������
	CDPoint& operator= (const CDPoint& pt)
	{
		x=pt.x;
		y=pt.y;
		return * this;
	}

	//�������� �������� ���� �����
	CDPoint operator + (CDPoint & pt)
	{
		return CDPoint (x+pt.x,y+pt.y);
	}

	//�������� ��������� ���� �����
	CDPoint operator - (CDPoint& pt)
	{
		return CDPoint (x-pt.x,y-pt.y);
    }

	//����� ���������� � ���� CPoint (����� �����)
	CPoint  ToInt()
	{
		return CPoint (int(x),int(y));
	}

	//�������� �������� � ������� ����������
	void operator +=(CDPoint & pt) {x+=pt.x; y+=pt.y;}

	//�������� ��������� � ������� ����������
	void operator -=(CDPoint & pt) {x-=pt.x; y-=pt.y;}

	//�������� ���������� ����� �������, ��������� ������
	double operator ! () {return fabs(x)+fabs(y);}
};

//��� ���������� �����
typedef vector < CDPoint , allocator < CDPoint > > VECPTS ;