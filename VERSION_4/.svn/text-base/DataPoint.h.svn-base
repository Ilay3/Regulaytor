#pragma once
#include <vector>
#include <math.h>
using namespace std;

class CDataPoint
{
public:
	//конструктор по умолчанию
	CDataPoint(void);

	//конструктор копирования
	CDataPoint(const CDataPoint& pt);

	//конструктор с параметрами
	CDataPoint(float x, float y);

	// деструктор
	~CDataPoint(void);

public:
	//метод приведения к типу CPoint (целая точка)
	CPoint ConvertToCPoint(void);

	//получение значение X
	float GetX(void);
	//получение значение Y
	float GetY(void);
	//задание значение Y
	void  SetY(float val);

	//операция умножения (увеличения в n раз)
	CDataPoint  operator * (unsigned int n);

	//операция присваивания
	CDataPoint& operator = (const CDataPoint& pt);

	//операция сложения двух точек
	CDataPoint  operator + (CDataPoint& pt);

	//оператор вычитания двух точек
	CDataPoint  operator - (CDataPoint& pt);

	//операция сложения с записью результата
	void operator += (CDataPoint& pt);

	//операция вычитания с записью результата
	void operator -= (CDataPoint& pt);

	//операция вычисления нормы вектора, заданного точкой
	float operator ! (void);

protected:
	float X;
	float Y;
};

//тип контейнера точек

typedef vector<CDataPoint, allocator<CDataPoint>> VECTOR_POINT;