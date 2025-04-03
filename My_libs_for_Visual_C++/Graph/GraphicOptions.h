/****************************************************************************************
*****************************************************************************************
**		Этот файл осуществляет взаимосвязь между пользовательской программой и 
**	классом графика. 
******************************************************************************************
*****************************************************************************************/

/**********************   ЛИСТР РЕГИСТРАЦИИ ИЗМЕНЕНИЙ  *********************************
*	04.05.07 - создан
*	5.11.07 - добавлен параметр m_GraphName
*	4.06.08 - добавлены m_bShowGrid, m_bShowTicks, m_bYLineAtLeft
*	22.07.2008 - добавил var_X_view
***************************************************************************************/

#pragma once
#include "types_visual_c++.h"				//файл с моими типами

/*****************************************************************
*		РАЗЛИЧНЫЕ КОНСТАНТЫ
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
	//Имя графика (то, что будет писаться вверху)
	CString m_GraphName;

	//Эта опция позволяет двигаться сетке на экране , т.е. сетка высвечивает только целые
	//секунды, а не дробные числа
	bool m_oMoveGrid;

	//количество вертикальных штрихов на сетке графика (деления оси Х)
	INT8S m_oHorisontalTicks;

	//количество вертикальных штрихов на сетке графика (деления оси У)
	INT8S m_oVerticalTicks;

	bool m_bShowGrid;		//показывать ли сетку
	bool m_bShowTicks;		//показывать ли штрихи на осях
	bool m_bYLineAtLeft;	//ось Y слева?


	FP32 var_X_view;		//левая граница визуализации графика
	FP32 var_VisibleTime;	//время от начала до конца графика, показываемого на экране

	//текущая нижняя граница видимой области графика
	FP32 var_yMin;
	//то же самое, но верхняя
	FP32 var_yMax;

	int var_Y_pos;			// координаты левого верхнего угла графика на родительском окне
	int var_X_pos;			// 

	int var_GraphHeight;	//
	int var_GraphWidth;		// размеры картинки графика

	//конструктор
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
		m_bShowGrid=true;			//показывать сетку
		m_bShowTicks=true;			//показывать метки на осях
		m_bYLineAtLeft=true;		//ось Y слева

		var_Y_pos=0;
		var_X_pos=0;

		var_GraphHeight=G_MINGRAPHHEIGHT;
		var_GraphWidth=G_MINGRAPHWIDTH;
	}
};