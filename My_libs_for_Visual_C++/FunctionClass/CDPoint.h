#pragma once
#include <vector>
#include <math.h>
using namespace std;


class CDPoint
{//класс вещественных точек и работы  с ними
public:
	float x;
	float y;
	//конструктор по умолчанию
	CDPoint () {x=0. ; y=0.;}
	//конструктор копирования
	CDPoint ( const CDPoint& pt) 
	{
		x=pt.x;
		y=pt.y;
	}
	//конструктор с параметрами
	CDPoint ( float xx , float yy)
	{
		x=xx;
		y=yy;
	}
	//операция умножения ( увеличения в n раз)
	CDPoint operator*(UINT n)
	{
		return CDPoint(x*n,y*n);
	}

	//операция присваивания
	CDPoint& operator= (const CDPoint& pt)
	{
		x=pt.x;
		y=pt.y;
		return * this;
	}

	//операция сложения двух точек
	CDPoint operator + (CDPoint & pt)
	{
		return CDPoint (x+pt.x,y+pt.y);
	}

	//оператор вычитания двух точек
	CDPoint operator - (CDPoint& pt)
	{
		return CDPoint (x-pt.x,y-pt.y);
    }

	//метод приведения к типу CPoint (целая точка)
	CPoint  ToInt()
	{
		return CPoint (int(x),int(y));
	}

	//операция сложения с записью результата
	void operator +=(CDPoint & pt) {x+=pt.x; y+=pt.y;}

	//операция вычитания с записью результата
	void operator -=(CDPoint & pt) {x-=pt.x; y-=pt.y;}

	//операция вычисления нормы вектора, заданного точкой
	double operator ! () {return fabs(x)+fabs(y);}
};

//тип контейнера точек
typedef vector < CDPoint , allocator < CDPoint > > VECPTS ;