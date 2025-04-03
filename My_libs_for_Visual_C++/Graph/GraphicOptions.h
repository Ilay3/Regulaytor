/****************************************************************************************
*****************************************************************************************
**		���� ���� ������������ ����������� ����� ���������������� ���������� � 
**	������� �������. 
******************************************************************************************
*****************************************************************************************/

/**********************   ����� ����������� ���������  *********************************
*	04.05.07 - ������
*	5.11.07 - �������� �������� m_GraphName
*	4.06.08 - ��������� m_bShowGrid, m_bShowTicks, m_bYLineAtLeft
*	22.07.2008 - ������� var_X_view
***************************************************************************************/

#pragma once
#include "types_visual_c++.h"				//���� � ����� ������

/*****************************************************************
*		��������� ���������
******************************************************************/
//colorschemes
#define G_DEFAULTSCHEME		0
#define G_WHITESCHEME		1
#define G_REDSCHEME			2
#define G_BLUESCHEME		3
#define G_GREENSCHEME		4
#define G_MAGENTASCHEME		5
#define G_YELLOWSCHEME		6
#define G_CYANSCHEME		7

//default graphsize
#define G_MINGRAPHWIDTH		350
#define G_MINGRAPHHEIGHT	200

//default axies scaling
#define G_DEFAULTXMIN		-100
#define G_DEFAULTXMAX		100
#define G_DEFAULTYMIN		-100
#define G_DEFAULTYMAX		100

//miscellaneous
#define G_TICKLENGTH 10 //size of those little ticks on the axes

/*
Function related  defines and stuff
*/


#define G_BARCHARTWIDTH 5 //default width of a bar chart

struct CGraphicOptions
{
	//��� ������� (��, ��� ����� �������� ������)
	CString m_GraphName;

	//��� ����� ��������� ��������� ����� �� ������ , �.�. ����� ����������� ������ �����
	//�������, � �� ������� �����
	bool m_oMoveGrid;

	//���������� ������������ ������� �� ����� ������� (������� ��� �)
	INT8S m_oHorisontalTicks;

	//���������� ������������ ������� �� ����� ������� (������� ��� �)
	INT8S m_oVerticalTicks;

	bool m_bShowGrid;		//���������� �� �����
	bool m_bShowTicks;		//���������� �� ������ �� ����
	bool m_bYLineAtLeft;	//��� Y �����?


	FP32 var_X_view;		//����� ������� ������������ �������
	FP32 var_VisibleTime;	//����� �� ������ �� ����� �������, ������������� �� ������

	//������� ������ ������� ������� ������� �������
	FP32 var_yMin;
	//�� �� �����, �� �������
	FP32 var_yMax;

	int var_Y_pos;			// ���������� ������ �������� ���� ������� �� ������������ ����
	int var_X_pos;			// 

	int var_GraphHeight;	//
	int var_GraphWidth;		// ������� �������� �������

	//�����������
	CGraphicOptions()
	{
		m_GraphName="Graph not named";
		m_oHorisontalTicks=10;
		m_oVerticalTicks=10;
		var_X_view=0;
		var_VisibleTime=10;
		var_yMin=0;
		var_yMax=100;
		m_oMoveGrid=false;
		m_bShowGrid=true;			//���������� �����
		m_bShowTicks=true;			//���������� ����� �� ����
		m_bYLineAtLeft=true;		//��� Y �����

		var_Y_pos=0;
		var_X_pos=0;

		var_GraphHeight=G_MINGRAPHHEIGHT;
		var_GraphWidth=G_MINGRAPHWIDTH;
	}
};