/*********************************************************************************************************************************
*		Этот модуль отвечает за прорисовку графика на экране
*****************************************************************************************/

/**********************		ЛИСТ РЕГИСТРАЦИИ ИЗМЕНЕНИЙ	***********************************
*	21.11.2006 - теперь по оси Y нормально отображаются большие числа
*	4.1.2007 - Теперь на оси Y все по-честному, выводятся и дробные числа
*	5.1.2007 - Добавлена поддержка свойства AlwaysDown у функций
*	17.1.2007 - m_GraphName перемещена в public, для динамичной смены названия графика
*	22.1.2007 - добавлена поддержка класса ошибок
*				+ теперь масштаб по оси Х проходит корректно и для малых величин
*	1.02.2007 - удалены ненужные функции имени функции и имени графика по умолчанию
*				+ существенно переработана и улучшена инициализация графика и настройки по умолчанию
*				+ столбики теперь корректно отрисовываются при отрицательных значениях функции и 
*					включенном свойстве AlwaysDown
*	27.02.2007 - большой пропорциональный коэффициент функции теперь не вызывает исключение
*	5.11.2007	- Имя графика теперь задается через GraphicOptions. Более качественная прорисовка
*					времени при динамической сетке.
*	7.11.07 - добавлена функция SaveGraphMathcad
*	24.11.07 - добавлена функция GetVisibleFunctionsCount(), улучшено отображение имени функции
*	3.12.07 - добавлен обработчик ошибки в функцию GetMoveAxisOffsetTime
*	17.01.08 - мега глобальная переделка, теперь график - окно, которому поступают свои сообщения
*				точки на графике (и функции в целом) можно выделять и двигать. График стал 
*				отрисовываться заново только по тику таймера и перестал мерцать. Предыдущая 
*				версия отныне считается глобально устаревшей.
*	11.02.08 - теперь линия на графике прорисовывается даже к точкам, которые реально не отображены
*				на графике. Точки функций с ProporCoef!=1 теперь также нормлаьно выделяются.
*	10.03.08 - черточки и подписи на осях стали прорисовываться менее глючно
*	11.03.08 - Введен формат графика ver 1.1, где есть изменения в формате сохранения функций, а 
*				также сохраняется название графика. Возможно менять ширину линии графика. Числа по
*				вертикальной оси прорисовываются с интеллектуальной точностью. Удалены некоторые 
*				ненужные переменные.
*	4.04.08 - При плавающей сетке последняя подпись по Х стала прорисоваваться
*	5.04.08 - Узкие столбики, уходящие за верхнюю границу графика теперь прорисовываются.
*				В прорисовке широких столбиков исправлен баг.
*	7.04.08 - Добавлены функции ConvertToGraph_X и ConvertToGraph_Y. Исправлено несколько багов
*				при прорисовке широких столбиков
*	30.04.08 - Теперь программа не падает, когда нужно отрисовывать подпись оси Х, а графиков нету
*				Программа не падает если не нужно рисовать черточек на осях.
*	7.04.08 - Добавлены функции GetMaxGraphScreenValue и GetMinGraphScreenValue
*	4.06.08 - Первый вариант переделки, при которой при смене размеров графика динамически меняются
*				весь его каркас, т.е. надписи становятся меньше и компактнее. Много мелких улучшений.
*	10.07.08 - исправлена пара глюков в DrawBarWide
*	21.07.08 - добавлен UNICODE, однако он еще не тестировался, добавлена функция FromFuncNameToFuncNum
*				Исправлен глючё в DrawXLegend, когда график расположен не в левом верхнем углу диалога
*	22.07.08 - полностью перелопатил визуальное позиционирование, а такжу куча других чисток.
*	17.09.08 - исправлен баг в прорисовке фунцкии непрерывной линией со свойством AlwaysDown
*	24.10.08 - исправлены ошибки в функции DrawBarTight_2
*	8.12.08 - Исправлен баг, по которому при первом клике появлялся фокус из левого верхнего угла
*	11.12.08 - Функция PrintGraph перекочевала сюда.
*	11.01.09 - При выделении графика курсор стало невозможно вывести за пределы окна.
*	12.01.09 - Шрифт изменен на Arial.
*	13.01.09 - Большая доработка, теперь стало возможным выделение участка графика и его масштабирование
*	28.01.09 - Функция сохранения картинки графика перекочевала сюда.
*	10.08.09 - Функция DrawBarWidth поддерживает ColorEx и разную толщину линии
*	27.12.24 - Изменены цвета линий графиков
*********************************************************************************************/

#pragma once
#include "FunctionClass\CDPoint.h"			//класс точки функции
#include "FunctionClass\FunctionClass.h"	//класс функции
#include "ERRORS\ERRORS.h"					//чтобы поддерживались ошибки
#include "GraphicOptions.h"					//здесь передаются настройки в класс графика
#include "types_visual_c++.h"				//мои типы данных

#include <math.h>							//математика
#include <vector>							//для хранилища функций
#include <algorithm>						//различные алгоритмы
#include <functional>	
#include <fstream>							//для работы с файлами
#include <iosfwd>
#include <afx.h>
#include <FLOAT.h>							//для диапазонов float

using namespace std;

/*******************************************************************************************
*		ЗАДЕКЛАРИРУЕМ ВНЕШНИЕ КЛАССЫ
*********************************************************************************************/
class CGRAPH_Dlg;	//родительское окно
/*
ДЛЯ НАЧАЛА РАБОТЫ С КЛАССОМ ГРАФИКА, НУЖНО
	- ЕССНО СОЗДАТЬ ПЕРЕМЕННУЮ "CGraph" И ВЫЗВАТЬ ДЛЯ НЕЕ "CREATE"
	- Начальные настройки задаются в структуре CGraphicOptions, которая обязательно
		передается по указателю!
	- Создать и настроить функции
	- ЗАПУСТИТЬ В CGraph ТАЙМЕР НАЧАЛА ОТСЧЕТА "StartTime(TRUE)"
	- НУ И ДОБАВЛЯТЬ ДАННЫЕ
*/


#define MSG_GR_FUNC_NAME_SEL (WM_USER+154)	//это сообщение отправляется, когда поступула очередная ошибка

//Это локальная структура, отвечающая за точку, выделенную на графике
struct CFuncPointSel
{
	size_t  m_FuncNum;			//номер выделенной функцию
	size_t	m_PointNum;			//номер этой точки на функции pFunc
	
	CFuncPointSel(size_t FuncNum, size_t PointNum)
	{//конструктор, чтобы работа велась побыстрее
		m_FuncNum=FuncNum;
		m_PointNum=PointNum;
	}

	//Меньшей оказывается структура с:
	// 1 - с меньшим номером функции
	// 2 - с меньшим номером точки
	bool operator <(const CFuncPointSel &b) const
	{
		if(m_FuncNum < b.m_FuncNum) return true;
		else if(m_FuncNum > b.m_FuncNum) return false;

		if(m_PointNum < b.m_PointNum) return true;
		else if(m_PointNum > b.m_PointNum) return false;

		return false;	//этого в принципе не может быть
	}
};

//Это локальная структура, отвечающая за функцию, выделенную на графике
struct CFuncFuncSel
{
	size_t m_FuncNum;	//номер выделенной функцию
	CFuncFuncSel(size_t FuncNum)
	{//конструктор, чтобы работа велась побыстрее
		m_FuncNum=FuncNum;
	}
};

//это локальная структура, отвечающая за ректы, где были отрисованы имена функций
struct CFuncNameRect
{
	size_t	m_FuncNum;		//номер функции
	CRect	m_FuncNameRect;	//рект ее имени
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
	*		КОНСТРУКТОР
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
	ERROR_Class* m_pError;	//хранилище ошибок
public:
	//	Переменная в которой нам будут импортированы настройки графика
	CGraphicOptions *m_pGraphicOptions;

private:
#define GRG_NOTINITIALIZED	0x00 //график не инициализирован
#define GRG_STOP			0x01 //график остановлен
#define	GRG_PAUSE			0x02 //график на паузе
#define	GRG_PLAY			0x03 //график в режиме воспроизведения в реальном времени
#define GRG_RESTART			0x04 //нужно перезапустить график
protected:
	INT8U	m_Regime;
	INT64U	m_StartTime;				//время в миллисекундах начала построения графика (по системным часам) (GetTickCount())
public:
	bool	SetRegime(INT8U Regime);
	INT8U	GetRegime();
	//выдает смещение во времени между началом построения графика и моментом вызова функции
	//в СЕКУНДАХ
	FP32 GetOffsetTime();
	//устанавливает смещение левой границы визуализиции графика (если не GRG_PLAY)
	bool SetX_Offset(FP32 x_offset);


protected:
	INT16U m_CountVisibleGraphs;	//когда поступает команда PaintGraph сюда пишется количество 
									//графиков, которые видимы и их предстоит перерисовать
	INT16U m_thisFuncNumber;		//когда идет команда DrawFunction она не знает, какой по счету
									//график прорисовывает, однако эта переменная подскажет ей
									//НАЧИНАЕТСЯ С ЕДИНИЦЫ

private:
	CPoint	ConvertToGraphCoords(FP32 x, FP32 y);	//преобразовывает физические координаты в пикселы
	UINT	ConvertToGraph_X(FP32 x);				//координата по оси Х
	UINT	ConvertToGraph_Y(FP32 y);				//координата по оси Y
	//ищет координаты невидимой точки h_x,h_y в проекции на границу графика
	CPoint	ConvertToGraphCoordsEx(FP32 v_x, FP32 v_y, FP32 h_x, FP32 h_y);	
	bool	ConvertToReal_X(int x, FP32* pRealX, bool Cutting=false);
	bool	ConvertToReal_Y(int y, FP32* pRealY, bool Cutting=false);
	bool	IsPointVisible(FP32 x, FP32 y);			//видна ли точка на экране
	FP32 ConstrainY (FP32 y);
	
	/*****************************************************************************
	*		А вот эти функции собственно и занимаются прорисовкой моего графика
	*****************************************************************************/
	void DrawFunctionName();						//имена функция
	bool DrawYAxisNumbers();						//числа по оси Y
	void DrawXAxisNumbers();						//числа по оси Х
	bool DrawTicks();								//черточки по осям
	bool DrawBackGround();							//фон
	bool Draw_X_Axis();								//ось Х
	bool Draw_Y_Axis();								//ось Y
	bool DrawGrid();								//сетку
	void DrawYLegend();								//подпись оси Y
	void DrawXLegend();								//подпись оси X
	bool DrawGraphTitle();							//имя графика
	void DrawFunction(CFunctionClass* pFunction);	//одну функцию
	//отрисовывает точку соответствующим типом
	void PlotPoints(CFunctionClass* pFunction,size_t PointNum, bool IsFocused=false);
	//рисует узенький столбик к точке на графике
	void DrawBarTight(CFunctionClass* pFunction,size_t ThisPoint, bool IsFocused=false);
	//несколько узеньких столбиков по соседству, хотя должны накладываться
	void DrawBarTight_2(CFunctionClass* pFunction,size_t ThisPoint, bool IsFocused=false);
	//рисует широки столбик, шириной от центров между соседними точками
	void DrawBarWide(CFunctionClass* pFunction,size_t ThisPoint, bool IsFocused=false);
	//линия между точками
	void DrawConnectLine(CFunctionClass* pFunction,size_t ThisPoint, bool IsFocused=false);
	//прорисовывает точку на графике
	void DrawDot (CFunctionClass* pFunction,size_t PointNum, bool IsFocused=false);
	//выдает относительное смещение от левой стороны для плавающих осей
	void GetMoveAxisOffsetTime(float &offset,float& step);

	
	/*****************************************************************************
	*		Я буду перед каждой прорисовкой (и только перед каждой прорисовкой)
	*	пересчитывать различные константы (например смещения осей графика от границ)
	*	и т.д., чтобы потом во время самой прорисовки по 10 раз не вызывать одни
	*	и те же функции, а использовать переменные.
	*****************************************************************************/
	UINT m_RightMargin;			//|
	bool CalcRightMargin();		//|
	UINT m_LeftMargin;			//|
	bool CalcLeftMargin();		//|
	UINT m_BottomMargin;		//| расстояние в пикселах до прямоугольника
	bool CalcBottomMargin();	//| самого графика
	UINT m_TopMargin;			//|
	bool CalcTopMargin();		//|
	CRect CalcDataArea(void);


	UINT m_iOriginX;			//| координаты в пикселах начала графика
	UINT m_iOriginY;			//|
	FP32 m_dPixelsPerY;			//| разрешение по осям
	FP32 m_dPixelsPerX;			//|
	FP32 m_dXAxisMin;			//| границы визуализации по горизонтальной оси
	FP32 m_dXAxisMax;			//|
	FP32 m_PixelsPerX_Tick;		//| количество пикселей между линиями сетки по осям
	FP32 m_PiselsPerY_Tick;		//|
	bool CalcAxys(void);		//| предварительные операции с осями

	//Всё, что касается цветов
	COLORREF m_crYTickColor;
	COLORREF m_crXTickColor;
	COLORREF m_crYLegendTextColor;
	COLORREF m_crXLegendTextColor;
	COLORREF m_crGraphTitleColor;
	COLORREF m_crGraphPenColor;
	COLORREF m_crGraphBkColor;
	COLORREF m_crGridColor;
	
	//Всё, что касается шрифтов
	CFont m_GraphFont;
	CString m_szFontFace;
	int m_iFontSize;
	int m_iCharHeight;
	int m_iCharWidth;


	
	
public:
	//задает размеры графика
	void SetGraphSize(int height, int width);
	void SetGraphPos(int x,int y);

	//Массив наших функций
	vector<CFunctionClass> VecFunctions;

	//сохранение данных графика
	bool SaveGraph(const TCHAR* FileName);

	//Сохранение данных графика в формате для mathcad
	bool SaveGraphMathcad(const TCHAR* FileName, INT16S FuncNum=-1);

	//Сохранение данных графика в формате BMP
	bool SaveGraphBMP(const TCHAR* FileName);

	//открытие данных графика
	bool LoadGraph(const CHAR* FileName);
	bool LoadGraph(const WCHAR* FileName);
	//то же самое, но новый график добавляется к старому (создается новая функция)
	bool LoadGraphToExists(const CHAR* FileName);
	bool LoadGraphToExists(const WCHAR* FileName);

	//Выдает количество видимых функций
	INT16U GetVisibleFunctionsCount();

	//ФУНКЦИИ, СОБИРАЮЩИЕ ИНФОРМАЦИЮ О ГРАФИКАХ
	FP32 GetMaxGraphValue(FP32 FromTime=-FLT_MAX, FP32 ToTime=FLT_MAX);
	FP32 GetMinGraphValue(FP32 FromTime=-FLT_MAX, FP32 ToTime=FLT_MAX);
	FP32 GetMaxGraphScreenValue();	//|
	FP32 GetMinGraphScreenValue();	//| подсчет для видимого времени
	FP32 GetMaxGraphTime();
	FP32 GetMinGraphTime();


	/****************************************************
	*					ОТРИСОВКА
	****************************************************/
	//в моменты срабатывания таймера нужно перерисовать окно
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	bool m_bRedraw;		//когда происходит отрисовка по таймеру - я 
						//буду запрещать перерисовку в WM_PAINT, т.к.
						//изображение еще не готово
	CDC SecondDC;		//сюда я буду отрисовывать в фоновом режиме
	CBitmap SecondBitmap;//для этих же целей
	static const UINT REDRAW_TMR_ID=0x00;

	/****************************************************
	*				Выделение мышью и т.п.
	****************************************************/
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	//всегда выдает нормальные рект по двум точкам пространства
	CRect GetFocusRect(CPoint point_1,CPoint point_2);

	//массив точек, которые в данный момент попадают в область отрисовки
	vector<CFuncPointSel> mas_VisiblePoints;
	//массив точек, которые в данный момент выделены на графике курсором
	vector<CFuncPointSel> mas_SelectionPoints;
	//массив функций, которые в данный момент выделены на графике курсором
	vector<CFuncFuncSel>  mas_SelectionFunctoins;	
	//массив ректов, в которых отрисованы имена функций
	vector<CFuncNameRect> mas_FuncNameRects;

	//определяет, является ли данная точка выделенной
	bool IsPointSelected(size_t FuncNum, size_t Num);
	//определяет, является ли данная функция выделенной
	bool IsFunctionSelected(size_t FuncNum);
	//удаляет (или оставляет) выделенные точки
	void DeleteSelectedPoints(bool Inverse=false);
	//удаляет (или оставляет) выделенные функции
	void DeleteSelectedFunctions(bool Inverse=false);
	//пермещает выделенные точки по сторонам
	void MoveSelectedPointsUp();
	void MoveSelectedPointsDown();
	void MoveSelectedPointsLeft();
	void MoveSelectedPointsRight();
	//перемещает выделенные функции по сторонам
	void MoveSelectedFunctionsUp();
	void MoveSelectedFunctionsDown();
	void MoveSelectedFunctionsLeft();
	void MoveSelectedFunctionsRight();

	#define INVALID_FUNC_NUM	0xFFFF	//неверный код возврата семейством GetFuncNum
	//на входе указатель на функцию - на выходе ее номер в VecFunctions
	INT16U FromFuncPtrToFuncNum(CFunctionClass* ptr);
	//на входе имя функции, на выходе ее номер в VecFunctions
	INT16U FromFuncNameToFuncNum(const TCHAR* FuncName);

	DECLARE_MESSAGE_MAP()

	//Координаты прямоугольника при выделении ПРАВОЙ кнопкой мыши
	RECT	m_OldScreenRect;//сюда буду запоминать старый рект при вызове ClipCursor
	bool	m_bStartFocus;	//если true, то выделение началось
	CPoint	m_DownPoint;	//точка, где я нажал на мышь
	CRect	m_LastRect;		//нужно, чтобы закрасить старые линии
	CPoint	m_ThisPoint;	//где сейчас находится мышь

	//Координаты прямоугольника при выделении ЛЕВОЙ кнопкой мыши
	RECT	m_LOldScreenRect;
	#define ZOOM_NOT_ACTIVE		0x00	//всё еще только готово к выделению
	#define ZOOM_FOCUSING		0x01	//пользователь ведет мышью
	#define ZOOM_FOCUS_ENDED	0x02	//пользователь выделил мышью область и просто шатается по экрану
	INT8U	m_ZoomState;
	CRect	m_ZoomRect;
	CPoint	m_LDownPoint;
	CPoint	m_LThisPoint;
	bool	m_bCursorInFocus;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	//Функция распечатки графика
	bool PrintGraph(void);
};
