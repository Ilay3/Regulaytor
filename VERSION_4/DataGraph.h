#pragma once
#include "DataPoint.h"

// ����� ����� �����
class CDataGraph
{
public:
	CDataGraph(void);
	~CDataGraph(void);

	//���������� ������ ��������
	BOOL Add(float x, float y);
	// �������� �� ���������
	void Del(int pos);

	//�������
	void Clear();

	//��������� ���������� ������� � �������
	unsigned int GetSize(void);

	//��������� �������� X �������� ����� �������
	float GetX(unsigned int pos);
	//��������� �������� Y �������� ����� �������
	float GetY(unsigned int pos);
	//������� �������� Y �������� ����� �������
	void  SetY(unsigned int pos, float val);

	//���������� �������� Y � ����� ����������� ����� �������
	void  SetXY(float y);

	//��������� �������� Y � ����� ����������� ����� �������
	float GetXY(void);

	void SetSerieName(CString str);
	CString& GetSerieName(void);

	void SetXAxisName(CString str);
	CString& GetXAxisName(void);

	void SetYAxisName(CString str);
	CString& GetYAxisName(void);

	void SetXAxisUnit(CString str);
	CString& GetXAxisUnit(void);

	void SetYAxisUnit(CString str);
	CString& GetYAxisUnit(void);

	////////////////////////////////////
	void  SetYAxisValue(float vmin, float vmax);
	float GetYAxisMin  (void);
	float GetYAxisMax  (void);
	void  SetYCurValue (float vmin, float vmax);
	float GetYCurMin   (void);
	float GetYCurMax   (void);

	void  SetXAxisValue(float vmin, float vmax);
	float GetXAxisMin  (void);
	float GetXAxisMax  (void);
	void  SetXCurValue (float vmin, float vmax);
	float GetXCurMin   (void);
	float GetXCurMax   (void);
	////////////////////////////////////

	void  SetColor(COLORREF clr);
	COLORREF  GetColor(void);

	void  SetActive(BOOL state);
	BOOL  IsActive(void);

	void  SetVisible(BOOL state);
	BOOL  IsVisible(void);

protected:
	VECTOR_POINT m_Point;


	//���� ����������� �� ������
	BOOL b_ShowSerie;				

	//���� ����������� �� ������
	BOOL b_ShowLegend;				

	CString   szSerieName;
	CString   szXAxisName;
	CString   szYAxisName;
	CString   szXAxisUnit;
	CString   szYAxisUnit;

	float     m_YValue;

	float     m_dXAxisMax;
	float     m_dXAxisMin;
	float     m_dXCurMax;   // %
	float     m_dXCurMin;	// %

	float     m_dYAxisMax;
	float     m_dYAxisMin;
	float     m_dYCurMax;	// %
	float     m_dYCurMin;	// %

	BOOL      m_bActive;
	BOOL      m_bVisible;

	COLORREF  m_clrGraph;				//���� ����� �������
};

//��� ���������� ������

typedef vector<CDataGraph, allocator<CDataGraph>> VECTOR_DATA;