#include "stdafx.h"

#include "Graph.h"
#include <math.h>			//для математики
#include <AtlConv.h>		//для перобразований unicode<->ANSI

BEGIN_MESSAGE_MAP(CGraph, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_TIMER()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

/************************ CGraph() *****************************************
*		Конструктор по-умолчанию
****************************************************************************/
CGraph::CGraph()
{
	m_pGraphicOptions=NULL;
	m_iCharHeight=0;
	m_iCharWidth=0;
	m_bStartFocus=false;			//пользователь пока еще ничего не выделяет
	m_bRedraw=false;				//еще нет готового изображения
	m_Regime=GRG_NOTINITIALIZED;	//еще не инициализровался
	m_ThisPoint.SetPoint(-1,-1);	//|чтобы при первом клике не поялялся фокус из
	m_LThisPoint.SetPoint(-1,-1);	//|чтобы при первом клике не поялялся фокус из
	m_bCursorInFocus=false;			//|		
	m_ZoomState=ZOOM_NOT_ACTIVE;	//|курсор для масштабирования пока не в фокусе
}

/*****************************************************************************
*		ДЕСТРУКТОР		ДЕСТРУКТОР		ДЕСТРУКТОР
*****************************************************************************/
CGraph::~CGraph()
{

}

/********************    Create    ****************************************************
*		Функция создания графика.
*		pParentWnd - указатель на окно родителя
*		pGraphicOptions - указатель на структуру опций графика
*		pError - указатель на класс регистрации ошибок
*		colorscheme - цветовая сцена, в которой будет отрисован график
**************************************************************************************/
BOOL CGraph::Create(CWnd *pParentWnd, CGraphicOptions *pGraphicOptions, ERROR_Class *pError, UINT colorscheme)
{
	//возьмем настройки, которые нам передали внешняя программа
	ASSERT(pGraphicOptions);
	m_pGraphicOptions=pGraphicOptions;

	//возьмем указатель на информатор ошибок
	ASSERT(pError);
	m_pError=pError;

	//указатель на родителя
	ASSERT(pParentWnd);
	m_pParent=pParentWnd;

	//типа какими цветами будет чё отрисовываться
	SetColorScheme(colorscheme);
	
	//проверим значения левого верхнего угла на допустимость
	SetGraphPos(pGraphicOptions->var_X_pos,pGraphicOptions->var_Y_pos);
	
	//Зададим размеры графика, а вернее перезададим,
	//ведь только там проверяется их правильность
	SetGraphSize(m_pGraphicOptions->var_GraphHeight,m_pGraphicOptions->var_GraphWidth);
	
	//изначально график в режиме останова
	SetRegime(GRG_STOP);

	m_StartTime=0;			//график еще не начал отрисовываться

	//местоположение окна графика
	CRect rect;
	rect.left=pGraphicOptions->var_X_pos;
	rect.top=pGraphicOptions->var_Y_pos;
	rect.right=rect.left+pGraphicOptions->var_GraphWidth;
	rect.bottom=rect.top+pGraphicOptions->var_GraphHeight;

	BOOL lResult=CWnd::Create(NULL, _TEXT("Graph"), WS_TABSTOP | WS_VISIBLE | WS_CHILD, rect, pParentWnd, 2159);
	CDC* pDC=GetDC();
	pDC->LPtoDP(&rect);
	//я должен инициализировать копию контекста при первом запуске
	SecondDC.CreateCompatibleDC(pDC);
	
	//таймер перерисовки окна
	SetTimer(REDRAW_TMR_ID,150,NULL);

	//шрифт, которым всё будет писаться
	if(!CreateGraphFont(_TEXT("Tahoma"))/*Courier*/)
		return false;

	return lResult;
}

/************************    OnPaint    ***************************************************
*		Реакция на сообщение WM_PAINT
******************************************************************************************/
void CGraph::OnPaint()
{
	//Здесь будет происходить отрисовка
	CPaintDC dc(this); // device context for painting

	if(m_bRedraw)
	{
		//размеры области графика
		CRect rect(0,0,m_pGraphicOptions->var_GraphWidth,m_pGraphicOptions->var_GraphHeight);
		dc.BitBlt(rect.left,     rect.top, 
                  rect.Width(),  rect.Height(),
                  &SecondDC,     rect.left, 
				  rect.top,      SRCCOPY);
	}

	//ВЫДЕЛЕНИЕ ЛЕВОЙ КНОПКОЙ МЫШИ
	if(m_ZoomState==ZOOM_FOCUSING || m_ZoomState==ZOOM_FOCUS_ENDED)
	{//ПОЛЬЗОВАТЕЛЬ ЧТО-ТО ВЫДЕЛЯЕТ МЫШЬЮ
		if(!m_bRedraw)
		{//я еще не закрасил старый прямоугольник
			//прорисую прямоугольник
			dc.DrawFocusRect(m_ZoomRect);
		}

		//!немного запутанно, но экономлю локальные переменные
		//чтобы при следующем вызове закрасить этот прямоугольник
		m_ZoomRect=GetFocusRect(m_LDownPoint,m_LThisPoint);
		
		//прорисую прямоугольник
		dc.DrawFocusRect(m_ZoomRect);
	}

	//ВЫДЕЛЕНИЕ ПРАВОЙ КНОПКОЙ МЫШИ
	if(m_bStartFocus && m_ThisPoint.x!=-1)
	{//ПОЛЬЗОВАТЕЛЬ ЧТО-ТО ВЫДЕЛЯЕТ МЫШЬЮ
		if(!m_bRedraw)
		{//я еще не закрасил старый прямоугольник
			//прорисую прямоугольник
			dc.DrawFocusRect(m_LastRect);
		}

		//!немного запутанно, но экономлю локальные переменные
		//чтобы при следующем вызове закрасить этот прямоугольник
		m_LastRect=GetFocusRect(m_DownPoint,m_ThisPoint);
		
		//прорисую прямоугольник
		dc.DrawFocusRect(m_LastRect);
	}
}

/*************************     OnTimer    ***********************************
*		Сработал таймер
****************************************************************************/
void CGraph::OnTimer(UINT nIDEvent)
{
	if(nIDEvent==REDRAW_TMR_ID && IsWindow(m_hWnd) && IsWindowVisible())
	{//НУЖНО ПЕРЕРИСОВАТЬ ГРАФИК
		m_bRedraw=false;	//изображение еще не готово

		//Перед перерисовкой заного пересчитаю все отступы и размеры
		bool lOK=true;
		lOK=lOK && CalcRightMargin();
		lOK=lOK && CalcLeftMargin();
		lOK=lOK && CalcBottomMargin();
		lOK=lOK && CalcTopMargin();
		lOK=lOK && CalcAxys();
		if(!lOK)
		{//там произошла какая-то ошибка, поэтому выйдем
			AfxMessageBox(_TEXT("В процессе прорисовки функций возникли ошибки"));
		}

		//размеры области графика
		CRect rect(0,0,m_pGraphicOptions->var_GraphWidth,m_pGraphicOptions->var_GraphHeight);

		CDC* pDC=GetDC();
		pDC->LPtoDP(&rect);
		
		//а если это не первый запус - то должен удалить старое изображение
		SecondBitmap.DeleteObject();

		//В принципе, если быстродействие сильно прижмет, то нижеследующую кучу
		//команд можно выполнять только при изменении размеров окна графика
		SecondBitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
		CBitmap *lOldBitmap=SecondDC.SelectObject(&SecondBitmap);
		SecondDC.SetMapMode(pDC->GetMapMode());

        SecondDC.SetWindowExt(pDC->GetWindowExt());
        SecondDC.SetViewportExt(pDC->GetViewportExt());

        pDC->DPtoLP(&rect);
        SecondDC.SetWindowOrg(rect.left, rect.top);
		SecondDC.FillSolidRect(rect, pDC->GetBkColor());

		ReleaseDC(pDC);

		//нарисуем фон
		DrawBackGround();
	
		//Нарисуем названия видимых функций
		DrawFunctionName();

		//Нарисем сетку
		DrawGrid();
		
		//Нарисуем ось X
		Draw_X_Axis();

		//Нарисуем ось Y
		Draw_Y_Axis();

		//нарисуем имя графика
		DrawGraphTitle();

		//нарисуем имя функции
		DrawFunctionName();

		//название оси Y
		DrawXLegend();

		//название оси Y
		//doing this is very similar to doing the graph title or the
		//x-legend but the Y axis is either at the LHS or set to match the
		//x-origin
		DrawYLegend();

		//числа по осям
		DrawXAxisNumbers();
		DrawYAxisNumbers();

		//нужно узнать, какое количество графиков предстоит прорисовать
		m_CountVisibleGraphs=0;	//сбросим от предыдущего раза
		for(size_t i=0; i<VecFunctions.size(); i++)
		{
			if(VecFunctions[i].Visible)
			{
				m_CountVisibleGraphs++;
			}
		}

		//Нарисуем те функции, которые видно
		m_thisFuncNumber=0;	//сбросим номер отображаемой функции
		//Я решил собирать информацию о точках, которые отображаются на графике, здесь я сделаю это
		mas_VisiblePoints.clear();	//очищу контейнер от предыдущей отрисовки
		for(size_t i=0; i<VecFunctions.size(); i++)
		{//пройдемся по всем доступным функциям
			if(VecFunctions[i].Visible)
			{//функцию надо показать
				m_thisFuncNumber++;		//чтобы дальше все знали, что отображают
				DrawFunction(&VecFunctions[i]);
			}
		}

		//нарисум черточки на осях
		//!!! только после того, как прорисован график, чтобы не накладывалось
		DrawTicks();

		m_bRedraw=true;		//можно перерисовывать
		Invalidate();		//чем я и воспользуюсь
	}

	CWnd::OnTimer(nIDEvent);
}

/////////////////// SetColorScheme ////////////////////////////////////////////
/*																			///
	SetColorScheme set the color scheme for the graph						///
	There are 8 colorschemes in total but default colour scheme is the		///
	same as as the WHITE colorscheme (which is really grey)					///
*/																			///
///////////////////////////////////////////////////////////////////////////////
void CGraph::SetColorScheme(int Scheme, BOOL bRedraw)
{
	/*
	This sets up the colors for various graph elements
	*/
	switch (Scheme)
	{
	case G_DEFAULTSCHEME:
		m_crYTickColor=RGB(0,0,0);
		m_crXTickColor=RGB(0,0,0);
		m_crYLegendTextColor=RGB(0,0,0);
		m_crXLegendTextColor=RGB(0,0,0);
		m_crGraphTitleColor=RGB(0,0,0);
		m_crGraphBkColor=RGB(192,192,192);
		m_crGridColor=RGB(220,220,220);	
		break;
	case G_WHITESCHEME:
		m_crYTickColor=RGB(0,0,0);
		m_crXTickColor=RGB(0,0,0);
		m_crYLegendTextColor=RGB(0,0,0);
		m_crXLegendTextColor=RGB(0,0,0);
		m_crGraphTitleColor=RGB(0,0,0);
		m_crGraphBkColor=RGB(255,255,255);
		m_crGridColor=RGB(153,102,102);	
		break;
	case G_REDSCHEME:
		m_crYTickColor=RGB(255,255,255);
		m_crXTickColor=RGB(255,255,255);
		m_crYLegendTextColor=RGB(255,200,200);
		m_crXLegendTextColor=RGB(255,150,150);
		m_crGraphTitleColor=RGB(255,255,255);
		m_crGraphBkColor=RGB(128,0,0);
		m_crGridColor=RGB(200,0,0);	
		break;
	case G_BLUESCHEME:
		m_crYTickColor=RGB(255,255,0);
		m_crXTickColor=RGB(255,255,0);
		m_crYLegendTextColor=RGB(200,200,255);
		m_crXLegendTextColor=RGB(150,150,255);
		m_crGraphTitleColor=RGB(255,255,255);
		m_crGraphBkColor=RGB(0,0,128);
		m_crGridColor=RGB(0,0,200);	
		break;
	case G_GREENSCHEME:
		m_crYTickColor=RGB(255,255,255);
		m_crXTickColor=RGB(255,255,255);
		m_crYLegendTextColor=RGB(200,255,200);
		m_crXLegendTextColor=RGB(150,255,150);
		m_crGraphTitleColor=RGB(0,255,255);
		m_crGraphBkColor=RGB(0,128,0);
		m_crGridColor=RGB(0,200,0);	
		break;
	case G_CYANSCHEME:
		m_crYTickColor=RGB(255,0,255);
		m_crXTickColor=RGB(255,0,255);
		m_crYLegendTextColor=RGB(200,255,200);
		m_crXLegendTextColor=RGB(150,255,150);
		m_crGraphTitleColor=RGB(192,192,192);
		m_crGraphBkColor=RGB(0,128,128);
		m_crGridColor=RGB(0,64,64);	
		break;
	case G_YELLOWSCHEME:
		m_crYTickColor=RGB(0,255,255);
		m_crXTickColor=RGB(0,255,255);
		m_crYLegendTextColor=RGB(200,200,0);
		m_crXLegendTextColor=RGB(255,255,0);
		m_crGraphTitleColor=RGB(192,192,192);
		m_crGraphBkColor=RGB(128,128,0);
		m_crGridColor=RGB(0,64,64);	
		break;
	case G_MAGENTASCHEME:
		m_crYTickColor=RGB(192,192,255);
		m_crXTickColor=RGB(100,100,255);
		m_crYLegendTextColor=RGB(255,0,0);
		m_crXLegendTextColor=RGB(255,128,128);
		m_crGraphTitleColor=RGB(128,255,128);
		m_crGraphBkColor=RGB(128,0,128);
		m_crGridColor=RGB(192,0,192);
		break;
	default:
		break;

	}
	if(bRedraw)
	{
		Invalidate();
	}
}

///////////////////////////// CreateGraphFont /////////////////////////////////////
//	Пересоздает шрифт графика, а также автоматически подбирает его размер		///
///////////////////////////////////////////////////////////////////////////////////
bool CGraph::CreateGraphFont(CString FaceName)
{
	m_GraphFont.DeleteObject();		//на всякий случай

	UINT lFontSize;
	//В зависимости от размеров графика подберу размер шрифта
	if(m_pGraphicOptions->var_GraphHeight>400) lFontSize=18;
	else if(m_pGraphicOptions->var_GraphHeight>300) lFontSize=16;
	else lFontSize=14;

	 if(m_GraphFont.CreateFont	
			(
				lFontSize,				   // nHeight
				0,                         // nWidth
				0,                         // nEscapement
				0,                         // nOrientation
				FW_NORMAL,                 // nWeight
				FALSE,                     // bItalic
				FALSE,                     // bUnderline
				0,                         // cStrikeOut
				DEFAULT_CHARSET,           // nCharSet
				OUT_DEFAULT_PRECIS,        // nOutPrecision
				CLIP_DEFAULT_PRECIS,       // nClipPrecision
				DEFAULT_QUALITY,           // nQuality
				DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
				FaceName)==0)              // lpszFacename
	 {
		 CString str; str.Format(_TEXT("Невозможно создание шрифта графика %s размера %i"),FaceName,lFontSize);
		 m_pError->SetERROR(str, ERP_ERROR | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
		 return false;
	 }
 	
	 if(SecondDC.m_hDC!=NULL)
	 {
	/*	 HDC hDC;
 		CDC *dc;
		hDC=::GetDC(0);//get a whole screen dc
 		dc=new CDC;
 		dc->Attach(hDC);*/
	//	 CDC* pdc=SecondDC.GetDC();

		//меняю шрифт
		CFont* poldfont=SecondDC.SelectObject(&m_GraphFont);
		//и запоминю его параметры
		TEXTMETRIC textmetrics;
 		SecondDC.GetTextMetrics(&textmetrics);
 		m_iCharHeight=textmetrics.tmHeight;
 		m_iCharWidth=textmetrics.tmAveCharWidth;

	/*	dc->Detach();
		::ReleaseDC(0,hDC);
 		delete dc;*/

 		m_iFontSize=lFontSize;
 		m_szFontFace=FaceName;
	 }
	return true;
 }

//////////////////////////   SetGraphSizePos /////////////////////////////////////////////
/*																						///
	Sets the size of the entire graph size and position with the holding parent window.
	A value of -1 for any parameter means don't change that parameter.
	A value of 0 for the Width or Height(or any value between 0 the minimum graph width
	or height) means use the default graph width or height
*/
///////////////////////////////////////////////////////////////////////////////////
void CGraph::SetGraphSizePos(int xPos, int yPos, int Width, int Height)
{
	
	CRect rect;
	rect.left=m_pGraphicOptions->var_X_pos;
	rect.top=m_pGraphicOptions->var_Y_pos;
	rect.right=rect.left+m_pGraphicOptions->var_GraphWidth;
	rect.bottom=rect.top+m_pGraphicOptions->var_GraphHeight;

	/*
	If xPos or yPos <=0 then  position will not be changed
	*/
	m_pGraphicOptions->var_X_pos=xPos < 0?m_pGraphicOptions->var_X_pos:xPos;
	m_pGraphicOptions->var_Y_pos=yPos < 0?m_pGraphicOptions->var_Y_pos:yPos;

	/*
	A negative number or zero means no change of current width or height
	if a +ive size  given thats smaller than the default min size for
	height or width then use the default min size for those parameters;
	*/
	Width= (Width>0 && Width <G_MINGRAPHWIDTH)?G_MINGRAPHWIDTH:Width;
	m_pGraphicOptions->var_GraphWidth = Width <= 0? m_pGraphicOptions->var_GraphWidth :Width;

	Height = (Height>0 && Height <G_MINGRAPHHEIGHT)?G_MINGRAPHHEIGHT:Height;
	m_pGraphicOptions->var_GraphHeight = Height <= 0? m_pGraphicOptions->var_GraphHeight : Height;

	//remember to rescale the graph
	SetXAxisScale(m_dXAxisMin,m_dXAxisMax);
	SetYAxisScale(m_pGraphicOptions->var_yMin,m_pGraphicOptions->var_yMax);
	
	//clear old graph from the screen
	if(m_pParent)
	{
		m_pParent->InvalidateRect(&rect,TRUE);
		m_pParent->SendMessage(WM_PAINT,0,0);
	}
}	


////////////////////////// CalcTopMargin ///////////////////////////////////
//	Пересчитывает расстояние от верха до начала прямоугольника графика
////////////////////////////////////////////////////////////////////////////
bool CGraph::CalcTopMargin()
{
	//Осуществлю автоматическую регулировку в зависимости от размеров 
	//и имени графика
	if(m_pGraphicOptions->m_GraphName=="") m_TopMargin=m_iCharHeight;
	else if(m_pGraphicOptions->var_GraphHeight>400) m_TopMargin=4*m_iCharHeight;
	else if(m_pGraphicOptions->var_GraphHeight>300) m_TopMargin=3*m_iCharHeight;
	else m_TopMargin=2*m_iCharHeight;
	return true;
}


/////////////////////  CalcBottommargin  //////////////////////////////////
//	От низа до нижней границы графика
//////////////////////////////////////////////////////////////////////////
bool CGraph::CalcBottomMargin()
{
	FP32 lResult=0;
	//Эта область складывается из подписи оси Х графика и имен функций
	//1. Подпись оси Х
	lResult+=2.7f*(FP32)m_iCharHeight;
	//2. Место под подписи функций
	if(VecFunctions.size()>4)
	{//имена функций идут в два столбца
		lResult+=(m_iCharHeight+2)*floorf((FP32)VecFunctions.size()/2.f);
	}
	else
	{//имена функций идут в один столбец
		lResult+=(m_iCharHeight+2)*(FP32)VecFunctions.size();
	}
	m_BottomMargin=(UINT)lResult;
	return true;
}

/////////////////////// CalcLeftmargin ////////////////////////////////////
//	От лева до левой границы
////////////////////////////////////////////////////////////////////////////
bool CGraph::CalcLeftMargin()
{
	//Это значение зависит от количества цифирек, которые мне нужно прорисовывать
	//по вертикальной оси. Целую часть я буду рисовать полностью, а вот вещественную
	//буду считать в зависимости от точности
	if(m_pGraphicOptions->m_oVerticalTicks==0)
	{//не нужно писать чисел вообще
		m_LeftMargin=m_iCharWidth;
		return true;
	}

	INT8U lCharCount=0;
	FP32 lScale=(m_pGraphicOptions->var_yMax-m_pGraphicOptions->var_yMin)/m_pGraphicOptions->m_oVerticalTicks;
	
	//поправка на дробную часть
	if(lScale<0.000001f) lCharCount=9;
	else if(lScale<0.00001f) lCharCount=8;
	else if(lScale<0.0001f) lCharCount=7;
	else if(lScale<0.001f) lCharCount=6;
	else if(lScale<0.01f) lCharCount=5;
	else if(lScale<0.1f) lCharCount=4;
	else if(lScale<1) lCharCount=3;
	else if(lScale<10) lCharCount=2;
	else lCharCount=1;

	//поправка на целую часть
	FP32 temp=m_pGraphicOptions->var_yMax;
	while(1)
	{
		lCharCount++;
		temp/=10;
		if(temp<1)
			break;	//исчерпали количество знаков до запятой
	}

	//поправка на минус
	if(m_pGraphicOptions->var_yMin<0)
		lCharCount++;

	//доп. запас
	lCharCount+=2;

	m_LeftMargin=lCharCount*m_iCharWidth+lCharCount;	//+поправка на расстояние между символами
	return true;
}

///////////////////////// CalcRightMargin ////////////////////////////////////
//		Отступ в пикселах от правой границы области рисунка да сетки графика.
//////////////////////////////////////////////////////////////////////////////
bool CGraph::CalcRightMargin()
{
	m_RightMargin=3*m_iCharWidth;
	return true;
}

/////////////////////// SetXAxisScale /////////////////////////////////////////
/*
	This sets the min and max values of the x-axis (of the plotting area)
	It also sets what proportion of the x-range is denoted by one pixel
	a.ka. PixelsPerX
	It also caluclates where the x-origin (the x==0) point lies along
	the x-axis - if x does not pass through zero the x-origin gidline is shown
	at the top or bottom of the graph.
	(the x-origin point on screen is stored in the m_iOriginX parameter as 
	an offset from the LHS of the (total) graph
	26/2/2005 - The autofit parameter is always TRUE as scrolling is not yet
	implemented.
*/
//////////////////////////////////////////////////////////////////////////////
void CGraph::SetXAxisScale(FP32 min, FP32 max)
{
	//если право больше лева, то перевернем
	if (max < min)
	{
		FP32 temp=min;
		min=max;
		max=temp;
	}
	//if min and max are the same (especially if they are both zero
	//it can be a problem - so give them a bit of room
	if(min==max)
	{
		max+= 0.1f;
	}
	
	
	//set the member variables
	m_dXAxisMax=max;
	m_dXAxisMin=min;

	//Количество пикселей на 1 значение Х
	m_dPixelsPerX=(m_pGraphicOptions->var_GraphWidth-m_LeftMargin-m_RightMargin)/(max-min);

	//определю координаты в пикселах начала графика в горизонтальной плоскости
	if((min < 0) && (max >0))
	{
		m_iOriginX=(UINT)(abs(min) * m_dPixelsPerX) + m_LeftMargin;
	}
	else if((min <0) && (max <=0))
	{
		m_iOriginX=(m_pGraphicOptions->var_GraphWidth)-m_RightMargin;
	}
	else if(min >=0 && (max>=0))
	{
		m_iOriginX=m_LeftMargin;
	}
}

/////////////////////// SetYAxisScale ///////////////////////////////////////
/*
	This sets the min and max values of the y-axis (of the plotting area)
	It also sets what proportion of the grapgh axis pixels represents 1Y
	(PixelsPerY =plotheightinpixels/yrange)
	It also caluclates where the y-origin (the y==0) point lies along
	the y-axis - if y does not pass through zero the y-origin gidline is shown
	at the left or right of the graph (this can be overridden by using the SetYLineAtLeft()
	function.
	(the y-origin point on screen is stored in the m_iOriginY parameter as 
	an offset from the bottom of the (total) graph
	26/2/2005 - The autofit parameter is always TRUE as scrolling is not yet
	implemented.
*/	
/////////////////////////////////////////////////////////////////////////////
void CGraph::SetYAxisScale(FP32 min, FP32 max)
{
	FP32 temp,scale;
	//swap min and max if they are the wrong way around
	if (max < min)
	{
		temp=min;
		min=max;
		max=temp;
	}

	//if min and max are the same (especially if they are both zero
	//it can be a problem - so give them a bit of room
	if(min==max)
	{
		max+= 0.1f;
	}

	//set the member variables
	m_pGraphicOptions->var_yMin=min;
	m_pGraphicOptions->var_yMax=max;

	//calculate scaling
	CRect dataarea=CalcDataArea();

	//количество пикселей на 1 значение Х
	temp=max-min;
	scale=(m_pGraphicOptions->var_GraphHeight-(m_BottomMargin+m_TopMargin))/temp;
	m_dPixelsPerY=scale;

	//where should the Y origin be?

	if (min<0 && max >0)//if Y passes through zero
	{
		//from the bottom of the graph
		m_iOriginY=(UINT)(abs(min)*m_dPixelsPerY)+m_BottomMargin;
	}
	else if(min<0 && max<=0)//if Y values are all negative
	{
		m_iOriginY=(dataarea.bottom-dataarea.top)+m_BottomMargin;
	}
	else if (min >=0 && max >=0)//if Y values are all positive
	{
		m_iOriginY=m_BottomMargin;
	}		
}


//////////////////// DrawGrid ////////////////////////////////////////////////
/*
The grid comprises two parts - the rectangle drawn around the plotting area
and the vertical&horizontal gridlines.
The outline rectangle is always drawn, the drawing of the gidlines
is contrlled by the m_bShowGrid member parameter using the ShowGrid function.
The grid color is  m_crGridColor.
*/
//////////////////////////////////////////////////////////////////////////////
bool CGraph::DrawGrid()
{
	if(!m_pParent)
	{
		return false;
	}
	
	/*
	Always draw the dataarea outline rectangle
	*/
	
	CRect dataarea=CalcDataArea();;//where the graph data is actually drawn

	//need a pen of colour m_crGridColor
	CPen pen, *poldpen;
	pen.CreatePen(PS_DOT,1,m_crGridColor);

	
	//to make a rectangle outline we have to use a polyline
	//need an array of points
	//a recatangle comprises 5 point
	CPoint points[5];
	//topleft
	points[0].x=dataarea.left;
	points[0].y=dataarea.top;
	//topright
	points[1].x=dataarea.right;
	points[1].y=dataarea.top;
	//bottomright
	points[2].x=dataarea.right;
	points[2].y=dataarea.bottom;
	//leftbottom
	points[3].x=dataarea.left;
	points[3].y=dataarea.bottom;
	//back to topleft
	points[4].x=dataarea.left;
	points[4].y=dataarea.top;

	poldpen=SecondDC.SelectObject(&pen);
	SecondDC.Polyline(points,5);//draw the outline rectangle

	/* Now check whether the grid itself should
	be shown
	*/
	if(!m_pGraphicOptions->m_bShowGrid)
	{
		SecondDC.SelectObject(poldpen);
		return true;
	}

	if(m_pGraphicOptions->m_oMoveGrid)
	{//вертикальная сетка динамически движется за целыми значениями
		//время, на которое сетку нужно сдвинуть вправо
		FP32 time_offset;
		//время между отсчетами сетки
		FP32 step_time;
		GetMoveAxisOffsetTime(time_offset,step_time);
		//количество пикселов на которое сетку нужно сдвинуть вправо
		FP32 pixel_offset=time_offset*m_dPixelsPerX;
		//количество пикселов между шагами сетки
		FP32 pixel_step=step_time*m_dPixelsPerX;
		while(1)
		{
			SecondDC.MoveTo(dataarea.left+(int)pixel_offset , dataarea.top);
			SecondDC.LineTo(dataarea.left+(int)pixel_offset,dataarea.bottom);
			pixel_offset+=pixel_step;		//нарастим до следующей линии
			if (pixel_offset>dataarea.Width())
			{//всё, вышли за правую границу графика
				break;
			}
		}
	}
	else
	{//статические вертикальные черточки
		for(INT8U n=1; n<m_pGraphicOptions->m_oHorisontalTicks;n++)
		{
			SecondDC.MoveTo(dataarea.left+(int)(m_PixelsPerX_Tick*(FP32)n),dataarea.top);
			SecondDC.LineTo(dataarea.left+(int)(m_PixelsPerX_Tick*(FP32)n),dataarea.bottom);
		}
	}
	//do the Y grid lines
	//note Y gridlines run left - right
	for(INT8U n=1; n<m_pGraphicOptions->m_oVerticalTicks;n++)
	{
		SecondDC.MoveTo(dataarea.left, dataarea.top+(int)(m_PiselsPerY_Tick*(FP32)n));
		SecondDC.LineTo(dataarea.right,dataarea.top+(int)(m_PiselsPerY_Tick*(FP32)n));
	}

	SecondDC.SelectObject(poldpen);

	return true;
}

////////////////////// DrawTicks ///////////////////////////////////////////
/*
	DrawTicks does two things - it draws the x & Y axis lines and it also
	draws the little 'tick' lines.
	The axis lines are always shown but the 'ticks' are controlled by the
	m_bShowTicks member (using the ShowTicks function);
	The length of the ticks are set by the #define in the header file
	the axis lines and ticks use the same color (m_crXTickColor for
	the x-axis and m_crYTickColor for the y-axis)
*/
//*****************************************************************
bool CGraph::DrawTicks()
{
	ASSERT(m_pParent);
	
	if (!m_pGraphicOptions->m_bShowTicks)
	{//не нужно прорисовывать черточки
		return true;
	}
	CPen TickPen, *poldpen;
		
	//ЧЕРТОЧКИ ПО ОСИ X
	//start with the x-axis
	TickPen.CreatePen(PS_SOLID,1,m_crXTickColor);
	poldpen=SecondDC.SelectObject(&TickPen);
	//the ticklines vertically straddle the x-axis
	//two problems though - if the x-line is at or very close to the
	//top or bottom of the dataarea

	//длина черточки вверх от оси
	UINT xtoptick=G_TICKLENGTH/2;
	//длина черточки вниз от оси
	UINT xbottick=G_TICKLENGTH/2;
	int n;	//для циклов
	if(m_pGraphicOptions->m_oMoveGrid)
	{//у нас движущаяся сетка
		//время, на которое сетку нужно сдвинуть вправо
		float time_offset;
		//время между отсчетами сетки
		float step_time;
		GetMoveAxisOffsetTime(time_offset,step_time);
		//количество пикселов на которое сетку нужно сдвинуть вправо
		float pixel_offset=time_offset*m_dPixelsPerX;
		//количество пикселов между шагами сетки
		float pixel_step=step_time*m_dPixelsPerX;
		while(1)
		{
			//переместимся по оси Х на очередную черточку
			SecondDC.MoveTo(m_LeftMargin+(int)pixel_offset,m_pGraphicOptions->var_GraphHeight-m_iOriginY);
			//черточка вверх
			SecondDC.LineTo(m_LeftMargin+(int)pixel_offset,m_pGraphicOptions->var_GraphHeight-m_iOriginY-xtoptick);
			//черточка вниз (не работает, но так красивее)
			SecondDC.LineTo(m_LeftMargin+(int)pixel_offset,m_pGraphicOptions->var_GraphHeight-m_iOriginY+xbottick);

			pixel_offset+=pixel_step;		//нарастим до следующей линии
			if (pixel_offset>m_pGraphicOptions->var_GraphWidth-m_RightMargin-m_LeftMargin)
			{//всё, вышли за правую границу графика
				break;
			}
		}
	}
	else
	{//обычная статическая сетка
		for(n=1;n<m_pGraphicOptions->m_oHorisontalTicks;n++)
		{
			//loop and do the ticks
			//topticks
			SecondDC.MoveTo(m_LeftMargin+(int)(m_PixelsPerX_Tick*(FP32)n),m_pGraphicOptions->var_GraphHeight-m_iOriginY);
			SecondDC.LineTo(m_LeftMargin+(int)(m_PixelsPerX_Tick*(FP32)n),m_pGraphicOptions->var_GraphHeight-m_iOriginY-xtoptick);
			//bottom ticks
			SecondDC.LineTo(m_LeftMargin+(int)(m_PixelsPerX_Tick*(FP32)n),m_pGraphicOptions->var_GraphHeight-m_iOriginY+xbottick);

		}
	}
	//подчищусь
	SecondDC.SelectObject(poldpen);
	TickPen.DeleteObject();

	//ЧЕРТОЧКИ ПО ОСИ Y
	TickPen.CreatePen(PS_SOLID,1,m_crYTickColor);
	poldpen=SecondDC.SelectObject(&TickPen);
	//the tick horizontally straddle the the Y axis
	//some problems though - if the y-axis is at or very close to the
	//left or right of the data area or if the Y-line memeber is set to left
	//handside
	UINT ylefttick=G_TICKLENGTH/2;
	UINT yrighttick=G_TICKLENGTH/2;
	//check for the special case where the y-axis has been forced to the left
	if(m_pGraphicOptions->m_bYLineAtLeft)
	{
		ylefttick=0;
	}

	int x,y;
	if(m_pGraphicOptions->m_bYLineAtLeft)
	{
		x=m_LeftMargin;
		y=m_pGraphicOptions->var_GraphHeight-m_BottomMargin;
	}
	else
	{
		x=m_pGraphicOptions->var_X_pos+m_iOriginX;
		y=m_pGraphicOptions->var_GraphHeight-m_BottomMargin;
	}


	for(n=1;n<m_pGraphicOptions->m_oVerticalTicks;n++)
	{
		SecondDC.MoveTo(x,y-(int)((FP32)n*m_PiselsPerY_Tick));
		//do left side tick
		SecondDC.LineTo(x-ylefttick,y-(int)((FP32)n*m_PiselsPerY_Tick));
		//do rightside tick
		SecondDC.LineTo(x+yrighttick,y-(int)((FP32)n*m_PiselsPerY_Tick));
	}
	TickPen.DeleteObject();
	//cleanup
	SecondDC.SelectObject(poldpen);

	return true;
}

/************************    DrawBackGround()   ***********************************
*		Рисуем фон
**********************************************************************************/
bool CGraph::DrawBackGround()
{
	CPen pen, *lpOldPen;
	CBrush brush,*lpOldBrush;

	brush.CreateSolidBrush(m_crGraphBkColor);
	pen.CreatePen(PS_SOLID,1,m_crGraphBkColor);
	
	//размеры области графика
	CRect rect(0,0,m_pGraphicOptions->var_GraphWidth,m_pGraphicOptions->var_GraphHeight);
		
	//прорисовка фона
	lpOldPen=SecondDC.SelectObject(&pen);
	lpOldBrush=SecondDC.SelectObject(&brush);
	SecondDC.Rectangle(&rect);

	SecondDC.SelectObject(lpOldPen);		//|
	SecondDC.SelectObject(lpOldBrush);	//|восстановим старые настройки
	
	pen.DeleteObject();					//удалим созданный корондаш
	brush.DeleteObject();				//удалим созданную кисть

	return true;
}

/*******************    Draw_X_Axis()   *****************************
*		рисуем ось Х, !! Пока не отработано смещение (0;0) по оси X
*********************************************************************/
bool CGraph::Draw_X_Axis()
{
	SecondDC.MoveTo(m_LeftMargin,m_pGraphicOptions->var_GraphHeight-m_iOriginY);

	CPen pen, *poldpen;
	pen.CreatePen(PS_SOLID,1,m_crXTickColor);
	poldpen=SecondDC.SelectObject(&pen);
	SecondDC.LineTo(m_pGraphicOptions->var_GraphWidth-m_RightMargin,m_pGraphicOptions->var_GraphHeight-m_iOriginY);
	SecondDC.SelectObject(poldpen);
	pen.DeleteObject();

	return true;
}

/*******************    Draw_Y_Axis()   *****************************
*		рисуем ось Y
*********************************************************************/
bool CGraph::Draw_Y_Axis()
{
	CPen pen, *poldpen;
	pen.CreatePen(PS_SOLID,1,m_crXTickColor);
	poldpen=SecondDC.SelectObject(&pen);
	if (!m_pGraphicOptions->m_bYLineAtLeft)
	{//ось Y посередине
		//draw the Y Line so that it intercepts
		//the x-line like crosshairs
		SecondDC.MoveTo(m_iOriginX,m_pGraphicOptions->var_Y_pos+m_TopMargin);
		SecondDC.LineTo(m_iOriginX,m_pGraphicOptions->var_GraphHeight-m_BottomMargin);
	}
	else
	{//ось Y слева
		//draw the Y Line at the LHS
		SecondDC.MoveTo(m_LeftMargin,m_TopMargin);
		SecondDC.LineTo(m_LeftMargin,m_pGraphicOptions->var_GraphHeight-m_BottomMargin);
	}
	SecondDC.SelectObject(poldpen);
	pen.DeleteObject();

	return true;
}

///////////// CalcDataArea ////////////////////////////////////////////////////
/*
	This function calculates the actual plotting area of the graph
	this is the graph area minus the top,bottom,left & right margins
	Returns:
	CRect with the plotting area (in client area co-ords)
*/
///////////////////////////////////////////////////////////////////////////////
CRect CGraph::CalcDataArea()
{
	CRect lDataArea;
	lDataArea.left=m_LeftMargin;
	lDataArea.right=m_pGraphicOptions->var_GraphWidth-m_RightMargin;
	lDataArea.top=m_TopMargin;
	lDataArea.bottom=m_pGraphicOptions->var_GraphHeight-m_BottomMargin;
	return lDataArea;
}

///////////////////// DrawGraphTitle ////////////////////////////////////
/*
	This draws the Graph title string in the color m_crGraphTitleColor
	This title is centred abobe the plotting area.
*/
////////////////////////////////////////////////////////////////////////
bool CGraph::DrawGraphTitle()
{
	//The graph title is drawn one character line down
	//centered left right between the left and right margins
	if (m_pParent==NULL)
	{
		return false;
	}

	CRect rect;
	rect.left=m_LeftMargin;
	rect.top=(m_TopMargin-m_iCharHeight)/2;
	rect.bottom=rect.top+m_iCharHeight;
	rect.right=m_pGraphicOptions->var_GraphWidth-m_RightMargin;

	//draw the title using the specified colorscheme
	//using the graph font
	
	CFont *poldfont;

	//note we must clear of any old stuff crap fom this area
	CBrush brush;
	brush.CreateSolidBrush(m_crGraphBkColor);
	//or bottom of a given rect
	SecondDC.FillRect(&rect,&brush);

	SecondDC.SetBkMode(TRANSPARENT);
	SecondDC.SetTextColor(m_crGraphTitleColor);
	poldfont=SecondDC.SelectObject(&m_GraphFont);
	SecondDC.DrawText(m_pGraphicOptions->m_GraphName,&rect,DT_CENTER|DT_END_ELLIPSIS);
	
	//cleanup
	SecondDC.SelectObject(poldfont);

	return true;
}

///////////////////// DrawXLegend //////////////////////////////////////////////
/*
	The Xaxis legend is drawn below the plotting area
	below the x-axis scale numbers
*/
///////////////////////////////////////////////////////////////////////////////
void CGraph::DrawXLegend()
{
	//The x legend is drawn  below the data area
	if (m_pParent==NULL)	return;

	CRect rect,dataarea;
	dataarea=this->CalcDataArea();
	rect.left=m_LeftMargin;
	rect.right=m_pGraphicOptions->var_GraphWidth-m_RightMargin;
	rect.top=dataarea.bottom+3+m_iCharHeight;
	rect.bottom=rect.top+m_iCharHeight;
	//just 2 b safe move the rect down a bit so we don't
	//interfere with the yaxis numbers
	rect.top+=1;
	rect.bottom+=1;

	//draw the title using the specified colorscheme
	//using the graph font
	
	CFont *poldfont;

	//note we must clear of any old stuff crap fom this area
	CBrush brush;
	brush.CreateSolidBrush(m_crGraphBkColor);
	rect.InflateRect(0,0,1,1);//bcause fillrect does not go right to the right
	//or bottom of a given rect
	SecondDC.FillRect(&rect,&brush);
	rect.DeflateRect(0,0,1,1);

	SecondDC.SetBkMode(TRANSPARENT);
	SecondDC.SetTextColor(m_crXLegendTextColor);
	poldfont=SecondDC.SelectObject(&m_GraphFont);
	if(!VecFunctions.empty())
	{//есть хотя бы одна функция, по которой можно рисовать подпись оси Х
		SecondDC.DrawText(VecFunctions[0].szXLegend,&rect,DT_CENTER|DT_END_ELLIPSIS);
	}
	else
	{//нет функций, пожалуй ничего не буду рисовать
	}
	
	//cleanup
	SecondDC.SelectObject(poldfont);
}

//////////////////////// DrawYlegend //////////////////////////////////////////
/*
	The Y legend is drawn at the LHS of the plot area between the yaxis max 
	and yaxis min scale
*/
///////////////////////////////////////////////////////////////////////////////
void CGraph::DrawYLegend()
{
	
	/*
	РАЗМЕРНОСЬ Я ПРОРИСОВЫВАЮ В НАЗВАНИИ ГРАФИКОВ, ПОЭТОМУ ЩАС МНЕ ЗДЕСЬ ДЕЛАТЬ НЕЧЕГО
	if (GRAPH_WINDOW==NULL)
	{
		return;
	}
	UINT tmargin=CalcTopMargin();
	UINT bmargin=CalcBottomMargin();
	CRect rect;
	
	rect.left=m_pGraphicOptions->var_X_pos+m_iCharWidth;
	rect.right=rect.left+11*m_iCharWidth;
	rect.top=m_pGraphicOptions->var_Y_pos+tmargin+2*m_iCharHeight;
	//rect.bottom=m_pGraphicOptions->var_Y_pos+m_pGraphicOptions->var_GraphHeight-bmargin-2*m_iCharHeight; //по старому
	rect.bottom=rect.top; //по-новому

	//предварительные настройки для рисования
	CFont *poldfont;
	CBrush brush;
	brush.CreateSolidBrush(m_crGraphBkColor);
	SecondDC.SetBkMode(TRANSPARENT);
	poldfont=SecondDC.SelectObject(&m_GraphFont);
	//само рисование, если график виден - рисуем, если нет - его место занимает следующий
	if(Function_1.Visible)
	{
		rect.top=rect.bottom;
		rect.bottom=rect.top+m_iCharHeight;
		SecondDC.FillRect(&rect,&brush);
		SecondDC.SetTextColor(Function_1.Color);
		SecondDC.DrawText(Function_1.szYLegend,&rect,DT_RIGHT|DT_WORDBREAK|DT_END_ELLIPSIS);
	}

	if(Function_2.Visible)
	{
		rect.top=rect.bottom;
		rect.bottom=rect.top+m_iCharHeight;
		SecondDC.FillRect(&rect,&brush);
		SecondDC.SetTextColor(Function_2.Color);
		SecondDC.DrawText(Function_2.szYLegend,&rect,DT_RIGHT|DT_WORDBREAK|DT_END_ELLIPSIS);
	}

	if(Function_3.Visible)
	{
		rect.top=rect.bottom;
		rect.bottom=rect.top+m_iCharHeight;
		SecondDC.FillRect(&rect,&brush);
		SecondDC.SetTextColor(Function_3.Color);
		SecondDC.DrawText(Function_3.szYLegend,&rect,DT_RIGHT|DT_WORDBREAK|DT_END_ELLIPSIS);
	}

	if(Function_4.Visible)
	{
		rect.top=rect.bottom;
		rect.bottom=rect.top+m_iCharHeight;
		SecondDC.FillRect(&rect,&brush);
		SecondDC.SetTextColor(Function_4.Color);
		SecondDC.DrawText(Function_4.szYLegend,&rect,DT_RIGHT|DT_WORDBREAK|DT_END_ELLIPSIS);
	}

	if(Function_5.Visible)
	{
		rect.top=rect.bottom;
		rect.bottom=rect.top+m_iCharHeight;
		SecondDC.FillRect(&rect,&brush);
		SecondDC.SetTextColor(Function_5.Color);
		SecondDC.DrawText(Function_5.szYLegend,&rect,DT_RIGHT|DT_WORDBREAK|DT_END_ELLIPSIS);
	}

	if(Function_6.Visible)
	{
		rect.top=rect.bottom;
		rect.bottom=rect.top+m_iCharHeight;
		SecondDC.FillRect(&rect,&brush);
		SecondDC.SetTextColor(Function_6.Color);
		SecondDC.DrawText(Function_6.szYLegend,&rect,DT_RIGHT|DT_WORDBREAK|DT_END_ELLIPSIS);
	}

	if(Function_7.Visible)
	{
		rect.top=rect.bottom;
		rect.bottom=rect.top+m_iCharHeight;
		SecondDC.FillRect(&rect,&brush);
		SecondDC.SetTextColor(Function_7.Color);
		SecondDC.DrawText(Function_7.szYLegend,&rect,DT_RIGHT|DT_WORDBREAK|DT_END_ELLIPSIS);
	}

	if(Function_8.Visible)
	{
		rect.top=rect.bottom;
		rect.bottom=rect.top+m_iCharHeight;
		SecondDC.FillRect(&rect,&brush);
		SecondDC.SetTextColor(Function_8.Color);
		SecondDC.DrawText(Function_8.szYLegend,&rect,DT_RIGHT|DT_WORDBREAK|DT_END_ELLIPSIS);
	}
	
	//cleanup
	SecondDC.SelectObject(poldfont);*/
}

///////////////////// DrawXAxisNumbers //////////////////////////////////////
/*
	The X axis scale numbers are drawm directly below the plot area.
	Three numbers are drawn, min, middle and max scaling
*/
////////////////////////////////////////////////////////////////////////////
void CGraph::DrawXAxisNumbers()
{
	if (!m_pParent)	return;
	//если пользователь ничего не хочет видеть - я ничего и не нарисую
	if (m_pGraphicOptions->m_oHorisontalTicks<=0) return;

	CRect rect,dataarea;
	CFont *poldfont;
	
	poldfont=SecondDC.SelectObject(&m_GraphFont);
	SecondDC.SetTextColor(m_crXLegendTextColor);
	SecondDC.SetBkMode(TRANSPARENT);

	dataarea=CalcDataArea();
//надпись слева
	rect.left=dataarea.left-4*m_iCharWidth;
	rect.top=dataarea.bottom+3;
	rect.right=rect.left+8*m_iCharWidth;//allow for 7 characters
	rect.bottom=rect.top+m_iCharHeight;
	rect.top+=1;
	rect.bottom+=1;
	//очистим любой старый текст
	CBrush brush;
	brush.CreateSolidBrush(m_crGraphBkColor);

	//версия вносящая ошибку... float one_step=ceil((m_dXAxisMax-m_dXAxisMin)*100/m_pGraphicOptions->m_oHorisontalTicks)/100.;//шаг прироста

	//НАДПИСЬ ПОД ОСТАЛЬНЫМИ ЧЕРТОЧКАМИ (всего m_pGraphicOptions->m_oHorisontalTicks черточек)
	if(m_pGraphicOptions->m_oMoveGrid)
	{//динамическая сетка (плывет по экрану вместе с графиком
				//время, на которое сетку нужно сдвинуть вправо
		float time_offset;
		//время между отсчетами сетки
		float step_time;
		GetMoveAxisOffsetTime(time_offset,step_time);
		//количество пикселов на которое сетку нужно сдвинуть вправо
		float pixel_offset=time_offset*m_dPixelsPerX;
		//количество пикселов между шагами сетки
		float pixel_step=step_time*m_dPixelsPerX;

		time_offset+=m_dXAxisMin;	//получили время, которое уже нужно прорисовывать 
									//на первой черточке
		while(1)
		{
			CString lNumberText;
			//Чтобы числа были в удобочитаемом формате, прикинем, сколько знаков показывать
			if(m_pGraphicOptions->var_VisibleTime<0.1)
			{//надо проставить побольше чисел после запятой
				lNumberText.Format(_TEXT("%.4f"),time_offset);
			}
			else if(m_pGraphicOptions->var_VisibleTime<1)
			{//видимость меньше 1 секунды
				lNumberText.Format(_TEXT("%.3f"),time_offset);
			}
			else if(m_pGraphicOptions->var_VisibleTime<10)
			{//нужно показывать достаточно много времения
				lNumberText.Format(_TEXT("%.2f"),time_offset);
			}
			else
			{//сильно крупный график
				lNumberText.Format(_TEXT("%.1f"),time_offset);
			}

			rect.left=dataarea.left+(LONG)pixel_offset;	//точно под черточкой
			rect.left=rect.left-4*m_iCharWidth;		//немного левее от черточки
			rect.right=rect.left+8*m_iCharWidth;	//и растянем до немного правее от черточки

			SecondDC.FillRect(&rect,&brush);					//|
			SecondDC.DrawText(lNumberText,&rect,DT_NOCLIP|DT_CENTER);	//|собственно прорисовка подписи

			pixel_offset+=pixel_step;		//нарастим до следующей линии
			time_offset+=step_time;			//узнаем, какое следующее время показывать

			if ((int)pixel_offset>dataarea.Width())
			{//всё, вышли за правую границу графика
				break;
			}
		}
	}
	else
	{//обычная статическая сетка
		//посчитаем увеличение занчения Х за одно деление
		FP32 lStepTime=(m_dXAxisMax-m_dXAxisMin)/((FP32)m_pGraphicOptions->m_oHorisontalTicks);//шаг прироста
		FP32 tmp2=m_dXAxisMin;							//левое значение
		FP32 lPixelsPerStep=lStepTime*m_dPixelsPerX;	//количество пикселей на шаг
		//Чтобы не отображать кучу чисел после запятой
		TCHAR lFormBuf[5];
		if(lStepTime<0.01)
			_tcscpy_s(lFormBuf,_TEXT("%.4f\0"));
		if(lStepTime<0.1)
			_tcscpy_s(lFormBuf,_TEXT("%.3f\0"));
		else if(lStepTime<1)
			_tcscpy_s(lFormBuf,_TEXT("%.2f\0"));
		else 
			_tcscpy_s(lFormBuf,_TEXT("%.1f\0"));

		for(INT16U i=0;i<=m_pGraphicOptions->m_oHorisontalTicks;i++) //если не i++, то можно рисовать не под каждой черточкой
		{
			CString str;
			str.Format(lFormBuf,tmp2);
			str.TrimRight(_T("0"));
			str.TrimRight(_T("."));

			FP32 lNewPixelOffset=(i-0.5f)*lPixelsPerStep;			//съеду на середину
			rect.left=dataarea.left+(LONG)lNewPixelOffset;
			rect.right=rect.left+(LONG)lPixelsPerStep;

			SecondDC.FillRect(&rect,&brush);						//в принципе не надо, видно, когда числа налазят друг на друга
			SecondDC.DrawText(str,&rect,DT_NOCLIP|DT_CENTER);
			
			tmp2+=lStepTime;//увеличим подпись
		}
	}

	//cleanup
	SecondDC.SelectObject(poldfont);
}

///////////////////// DrawYAxisNumbers //////////////////////////////////////
/*
	The Y axis scale is drawn on the LHS of the plot area.
	Only two numbers are drawn - min and max to allow for the Y-axis
	legend.
*/
/////////////////////////////////////////////////////////////////////////////
bool CGraph::DrawYAxisNumbers()
{
	ASSERT(m_pParent);
	//Если пользователь ничего не хочет видеть - я ничего и не нарисую
	if(m_pGraphicOptions->m_oVerticalTicks<=0) return true;
	
	CRect rect,dataarea;
	CFont *poldfont;
	
	poldfont=SecondDC.SelectObject(&m_GraphFont);
	SecondDC.SetTextColor(m_crYLegendTextColor);
	SecondDC.SetBkMode(TRANSPARENT);

	dataarea=CalcDataArea();

	//выравнивание слева и справа
	rect.left=0;
	rect.right=m_LeftMargin-3;
	
//	rect.top=0;									//|
//	rect.bottom=dataarea.bottom-7+m_iCharHeight;//|чтобы не затирались числа по оси Х
	
/*	//clear any old text
	CBrush brush;
	brush.CreateSolidBrush(m_crGraphBkColor);
	SecondDC.FillRect(&rect,&brush);*/

	//посчитаем увеличение занчения Х за одно деление
	FP32 lStepValue=(m_pGraphicOptions->var_yMax-m_pGraphicOptions->var_yMin)/((FP32)m_pGraphicOptions->m_oVerticalTicks);//шаг прироста
	FP32 lCurrentValue=m_pGraphicOptions->var_yMin;					//нижнее значение
	FP32 lPixelsPerStep=lStepValue*m_dPixelsPerY;					//количество пикселей на шаг
	//Чтобы не отображать кучу чисел после запятой
	TCHAR lFormBuf[5];
	if(lStepValue<0.01)
		_tcscpy_s(lFormBuf,_TEXT("%.4f\0"));
	if(lStepValue<0.1)
		_tcscpy_s(lFormBuf,_TEXT("%.3f\0"));
	else if(lStepValue<1)
		_tcscpy_s(lFormBuf,_TEXT("%.2f\0"));
	else 
		_tcscpy_s(lFormBuf,_TEXT("%.1f\0"));

//	if(m_pGraphicOptions->m_oVerticalTicks!=0)
//	{//в противном случае я просто не буду отрисовывать
	//удалять нельзя, т.к. одно число я не рисую,и здесь происходит разрыв
		INT8S i=0;
		for(INT16U i=0; i<=m_pGraphicOptions->m_oVerticalTicks; i++)
		{//всего m_pGraphicOptions->m_oHorisontalTicks чисел
			CString str;
			str.Format(lFormBuf,lCurrentValue);
		//	str.TrimRight(_T("0"));
		//	str.TrimRight(_T("."));

			//пойдем снизу вверх
			FP32 lNewPixelOffset=lPixelsPerStep*(FP32)i-(FP32)m_iCharHeight/2.f;
			rect.bottom=dataarea.bottom-(LONG)lNewPixelOffset;
			rect.top=rect.bottom-m_iCharHeight;

			SecondDC.DrawText(str,&rect,DT_NOCLIP|DT_RIGHT);

			lCurrentValue+=lStepValue;	//какое число будет напротив следующей черточки
		}
//	}
	
	//cleanup
	SecondDC.SelectObject(poldfont);
	return true;
}

////////////////// DrawFunctionName /////////////////////////////////////
/*
	Функция рисует названия функций внизу графика в два столбца по 4 имени в каждом.
	В добавок нужно прорисовать единицы измерения и масштабные коэффициенты
	Отображается название только видимых функций
*/
////////////////////////////////////////////////////////////////////////////
void CGraph::DrawFunctionName()
{
	ASSERT(m_pParent);
	
	CRect dataarea=CalcDataArea();
	CRect rect;
	rect.top=dataarea.bottom+(LONG)(1.7f*(FP32)m_iCharHeight);
	rect.bottom=rect.top+m_iCharHeight+1;
	rect.left=m_iCharWidth;						//отступ слева	

	if(VecFunctions.size()>4)
	{//когда функций слишком много, буду отображать их в два столбца
		rect.right=dataarea.right/2-m_iCharWidth;	//ширина
	}
	else
	{//а иначе полное название
		rect.right=dataarea.right-m_iCharWidth;		//ширина
	}

	CBrush brush;
	brush.CreateSolidBrush(m_crGraphBkColor);
	CFont *poldfont;
	
	poldfont=SecondDC.SelectObject(&m_GraphFont);
	SecondDC.SetBkMode(TRANSPARENT);

	//Рисую имена всех графиков, вне зависимости от того,
	//виден он или нет
	
	//очищу контейнер ректов под имена функций, т.к. сейчас я сформирую его заного
	mas_FuncNameRects.clear();

	//пока мы можем отображать максимально 8 названий графиков
	//int max=>8?8:GRAPH_Dlg->VecFunctions.size();
	int counter=0;	//счетчик уже отображенных названий
	size_t lFuncNum=0;		//счетчик функций, по которым прошлись
	while(1)		
	{
		if(lFuncNum>=VecFunctions.size())
		{//больше не осталось функций
			break;
		}

		if(VecFunctions.size()>4 && counter==(VecFunctions.size()+1)/2)
		{//перенесемся в правый столбец
			rect.top=dataarea.bottom+(LONG)(1.7f*(FP32)m_iCharHeight);
			rect.bottom=rect.top+m_iCharHeight+1;
			rect.left=rect.right+m_iCharWidth;			//новый отступ слева
			rect.right=rect.left+dataarea.right/2-m_iCharWidth;	//ширина
		}

		counter++;					//сейчас отобразим еще одно название
		rect.top=rect.bottom;
		rect.bottom=rect.top+m_iCharHeight;
		SecondDC.FillRect(&rect,&brush);
		SecondDC.SetTextColor(VecFunctions[lFuncNum].Color);
		
		CString str=VecFunctions[lFuncNum].szFunctionName;
		//единица измерения функции
		CString str2;
		if(VecFunctions[lFuncNum].szYLegend.GetLength()!=0)
			str2=_T("[")+VecFunctions[lFuncNum].szYLegend+_T("]");
		if(VecFunctions[lFuncNum].ProporCoef!=1)
		{//покажем пропорциональный коэффициент, если он не тривиален
			str2.AppendChar('*');
			CString str3;
			str3.Format(_TEXT("%.5g"),VecFunctions[lFuncNum].ProporCoef);
			str2.Append(str3);
		}
		
		if(str2!="[]")
		{//если хвост оказался не тривиальным
			str.Append(str2);	//подсоединим хвост из пропроционального коэффициента и единицы измерения
		}
		SecondDC.DrawText(str,&rect,DT_LEFT|/*DT_WORDBREAK|*/DT_END_ELLIPSIS);

		//запомню рект, где произошла отрисовка функции, чтобы отловить по
		//нему двойной щелчёк мышью
		mas_FuncNameRects.push_back(CFuncNameRect(lFuncNum,rect));

		lFuncNum++;	//нарастим счетчик проверенных функций
	}

	SecondDC.SelectObject(poldfont);
}

////////////////// SetdefaultColorScheme  //////////////////////////////////
/*
	sets the colours of various bits back to the default colours
*/
///////////////////////////////////////////////////////////////////////////
void CGraph::SetDefaultColorScheme()
{
	SetColorScheme(G_DEFAULTSCHEME);
}

//////////////////// SetYLineAtLeft ////////////////////////////////////////
/*
	The Y axis line can be forced to the LHS of the 
	plot area using this function
*/
/////////////////////////////////////////////////////////////////////////////

void CGraph::SetYLineAtLeft(bool AtLeft)
{
	if(m_pGraphicOptions->m_bYLineAtLeft!=AtLeft)
	{
		m_pGraphicOptions->m_bYLineAtLeft=AtLeft;
		Invalidate();
	}
}

/****************************************************************************
*		Набор для преобразования координат в пикселы на графике, если
*	точка вылазит за границы, то возвращаем 0хFFFF
****************************************************************************/
UINT	CGraph::ConvertToGraph_X(FP32 x)
{
	if(x < m_dXAxisMin || x > m_dXAxisMax)
	{
		return 0xFFFF;
	}
	//calc the abs difference between Xmin and x;
	FP32 xdif = x-m_dXAxisMin;
	//find the dataarea
	CRect rect=CalcDataArea();
	UINT xpos=rect.left+(UINT)(xdif*m_dPixelsPerX); //from left
	return xpos;
}
UINT	CGraph::ConvertToGraph_Y(FP32 y)
{
	if( y <m_pGraphicOptions->var_yMin || y > m_pGraphicOptions->var_yMax)
	{
		return 0xFFFF;
	}
	//calc the abs difference between Ymin and y;
	FP32 ydif = y-m_pGraphicOptions->var_yMin;
	//find the dataarea
	CRect rect=CalcDataArea();
	UINT ypos=rect.bottom-(UINT)(ydif*m_dPixelsPerY); //from bottom
	return ypos;
}

/***************     ConvertToGraphCoords    *********************************
*		Переводит вещественную точку в смещение в пикселах относительно
*	нижнего левого угла области прорисовки графика.
*		Если точка вылазит за область прорисовки графика, то возвращается (-1;-1)
*****************************************************************************/
CPoint CGraph::ConvertToGraphCoords(FP32 x, FP32 y)
{
	UINT lx=ConvertToGraph_X(x);
	if(lx==0xFFFF) return CPoint(-1,-1);
	UINT ly=ConvertToGraph_Y(y);
	if(ly==0xFFFF) return CPoint(-1,-1);

	return CPoint(lx,ly);
}

/*****************************    ConvertToGraphCoordsEx   **************************
*		Когда функция отрисовывается непрерывной линией, то мне нужно рисовать 
*	прямую между точками, даже когда одна из них выходит за границы графика.
*	Для этого и создана эта функция.
*		Если якобы невидимая точка на самом деле видимая, то возращаются ее координаты.
*		v_x, v_y - координаты видимой точки
*		h_x, h_y - координаты невидимой точки
*		Функция возвращает координаты точки, лежащей на границе области прорисовки графика
*	продолжение которой пересекло бы точку (h_x,h_y).
************************************************************************************/
CPoint	CGraph::ConvertToGraphCoordsEx(FP32 v_x, FP32 v_y, FP32 h_x, FP32 h_y)
{
	// Все формулы, по которым я вычисляю очень непонятные и приводятся в файле 
	// mathcad в папке проекта.
	ASSERT(IsPointVisible(v_x,v_y));

	if(IsPointVisible(h_x,h_y))
	{//мой алгоритм и без этого даст правильный результат,
	//но чтобы не вычислять мудреные формулы я сразу сделаю попроще.
		return ConvertToGraphCoords(h_x, h_y);
	}

	CRect lDataArea=CalcDataArea();	//область прорисовки данных графика
	int lWidth=lDataArea.Width();
	int	lHeight=lDataArea.Height();

	//Для начала я смещу все точки в новое начало координат
	FP32 ovx=v_x-m_dXAxisMin;
	FP32 ohx=h_x-m_dXAxisMin;
	FP32 ovy=v_y-m_pGraphicOptions->var_yMin;
	FP32 ohy=h_y-m_pGraphicOptions->var_yMin;

	//и для более быстрых вычислений преобразую в пиксели (целые числа)
	INT64S ivx=(INT64S)(ovx*m_dPixelsPerX);
	INT64S ihx=(INT64S)(ohx*m_dPixelsPerX);
	INT64S ivy=(INT64S)(ovy*m_dPixelsPerY);
	INT64S ihy=(INT64S)(ohy*m_dPixelsPerY);

	//теперь я представлю, что график ограничен только по горизонтали и 
	//найду коэффициент подобия
	FP32 k;
	if((ivx-ihx)==0)
	{//такое может случиться при работе с целыми числами
		k=0;
	}
	if(ihx<0)
	{//точка уехала влево
		k=(FP32)ihx/(FP32)(ivx-ihx);
	}
	else if(ihx>lWidth)
	{//точка уехала вправо
		k=(FP32)(ihx-lWidth)/(FP32)(ivx-ihx);
	}
	else
	{//точка в допустимых пределах
		k=0;
	}
	
	//новые координаты невидимой точки с учетом ограничения по горизонтали
	INT64S new_x_hor=ihx+(INT64S)((FP32)(ihx-ivx)*k);
	INT64S new_y_hor=ihy+(INT64S)((FP32)(ihy-ivy)*k);

	//теперь я учту и ограничение по вертикали
	if((ivy-new_y_hor)==0)
	{//такое может случиться при работе с целыми числами
		k=0;
	}
	if(new_y_hor<0)
	{//точка уехала вниз
		k=(FP32)new_y_hor/(FP32)(ivy-new_y_hor);
	}
	else if(new_y_hor>lHeight)
	{//точка уехала вверх
		k=(FP32)(new_y_hor-lHeight)/(FP32)(ivy-new_y_hor);
	}
	else
	{//точка в допустимых пределах
		k=0;
	}

	//окончательные кординаты проекции невидимой точки
	int x=new_x_hor+(int)((FP32)(new_x_hor-ivx)*k);
	int y=new_y_hor+(int)((FP32)(new_y_hor-ivy)*k);

	//нужно преобразовать координаты от левого нижнего угла графика в 
	//координаты на CDC графика
	x+=lDataArea.left;
	y=lDataArea.bottom-y;

	return CPoint(x,y);
}

/************************************************************************************
*		Набор для преобразования точки на экране в вещественные координаты на графике.
*	Если заданная точка попадает в область графика на окне, то возвращается истина.
*	В противном случае, если Cutting==true, то обрезается до ближайшей границы и возв
*	ращается истина, в противном случе возвращается ложь.
************************************************************************************/
bool CGraph::ConvertToReal_X(int x, FP32* pRealX, bool Cutting)
{
	if(x<(int)m_LeftMargin)
	{//вылезли за график слева
		if(Cutting)
		{
			*pRealX=m_dXAxisMin;
			return true;
		}
		else
			return false;
	}
	else if(x>m_pGraphicOptions->var_GraphWidth-(int)m_RightMargin)
	{//вылезли за график справа
		if(Cutting)
		{
			*pRealX=m_dXAxisMax;
			return true;
		}
		else
			return false;
	}
	else
	{//попадаю в график
		*pRealX=m_dXAxisMin+(x-m_LeftMargin)/m_dPixelsPerX;
		return true;
	}
	return false;	//здесь я бывать не должен		
}
bool CGraph::ConvertToReal_Y(int y, FP32* pRealY, bool Cutting)
{
	if(y>m_pGraphicOptions->var_GraphHeight-(int)m_BottomMargin)
	{//вылезли за график снизу
		if(Cutting)
		{
			*pRealY=m_pGraphicOptions->var_yMin;
			return true;
		}
		else
			return false;
	}
	else if(y<(int)m_TopMargin)
	{//вылезли за график сверху
		if(Cutting)
		{
			*pRealY=m_pGraphicOptions->var_yMax;
			return true;
		}
		else
			return false;
	}
	else
	{//попадаю в график
		*pRealY=m_pGraphicOptions->var_yMin+(m_pGraphicOptions->var_GraphHeight-m_BottomMargin-y)/m_dPixelsPerY;
		return true;
	}
	return false;	//здесь я бывать не должен
}

/**************************    IsPointVisible    *************************************
*		Проверяет, видна ли данная точка на экране графика.
*************************************************************************************/
bool CGraph::IsPointVisible(FP32 x, FP32 y)			
{
	if(x < m_dXAxisMin || x > m_dXAxisMax) return false;
	if(y < m_pGraphicOptions->var_yMin || y > m_pGraphicOptions->var_yMax) return false;
	return true;
}
/**************************    CalcAxys()   ***************************************
*	Делает все подготовительные операции для прорисовки функции, 
*	размечивает рабочую область графика (масштабирует по времени и по пикселам
*	САМИ ГРАФИКИ ПРОРИСОВЫВАЮТСЯ  НЕ ЗДЕСЬ
**************************************************************************************/
bool CGraph::CalcAxys()
{
	/*
	Далее мы узнем, с какой и по какую точку времени показывать пользователю график
	*/
	if(GetRegime()==GRG_PLAY)
	{//график в реальном времени отображает процессы
		//нужно подогнать область визуализации под актуальное время
		//коэффициент, чтобы не идти впритык к правой границе
		m_pGraphicOptions->var_X_view=GetOffsetTime()-0.9f*m_pGraphicOptions->var_VisibleTime;
	}

	if(m_pGraphicOptions->var_X_view < 0)
	{//если на отрицательной точке, то буду показывать с нуля (ПОКА)
		//разрешение по оси X
		SetXAxisScale(0,m_pGraphicOptions->var_VisibleTime);
	}
	else
	{//нормальное отображение
		//не будем делать задел с правой стороны
		SetXAxisScale(m_pGraphicOptions->var_X_view,m_pGraphicOptions->var_X_view+m_pGraphicOptions->var_VisibleTime);
	}
	
	//разрешение по оси Y
	if(m_pGraphicOptions->var_yMax<=m_pGraphicOptions->var_yMin)
	{//проверка
		m_pError->SetERROR(_TEXT("Нижняя планка графика оказалась выше верхней, отображение остановлено!!! Введены свои значения"),ERP_ERROR | ER_WRITE_TO_LOG);
		SetYAxisScale(0,100);
		return false;

	}
	SetYAxisScale(m_pGraphicOptions->var_yMin,m_pGraphicOptions->var_yMax);

	//количество пикселей между линиями сетки по осям
	m_PiselsPerY_Tick=(FP32)(m_pGraphicOptions->var_GraphHeight-m_TopMargin-m_BottomMargin)/(FP32)m_pGraphicOptions->m_oVerticalTicks;
	m_PixelsPerX_Tick=(FP32)(m_pGraphicOptions->var_GraphWidth-m_RightMargin-m_LeftMargin)/(FP32)m_pGraphicOptions->m_oHorisontalTicks;
	
	return true;
}

/************************    DrawFunction    ***********************************
*		Эта функция полностью отрисовывает переданную ей по указателю функцию на экране.
*	При этом осуществляется подсвечивание выделенных пользователем областей и др.
*	опции.
*******************************************************************************/
void CGraph::DrawFunction(CFunctionClass* Function)
{
	ASSERT(m_pParent);
	ASSERT(Function);
	
	CPoint ThisPoint;			//текущая точка
	CPoint PrevPoint(-1,-1);	//предыдущая точка
	CPoint NextPoint(0,0);		//следующая точка

	FP32 xperpixel= 1/m_dPixelsPerX;
	FP32 yperpixel=1/m_dPixelsPerY;

	//ЗДЕСЬ ОПРЕДЕЛЯЮТСЯ ЭКРАННЫЕ КООРДИНАТЫ ОБЛАСТИ ОТРИСОВКИ ФУНКЦИЙ
	ThisPoint=ConvertToGraphCoords(m_dXAxisMin,m_pGraphicOptions->var_yMin);
	LONG xstart=ThisPoint.x;		//left
	LONG ystart=ThisPoint.y;		//bottom
	bool lIsRealOnScreen=false;		//точка может уехать за границы сверху и снизу
									//и показываться на экране ее коррекция (но тоже точка)
									//этот флаг говорит, что точка отображенная на экране является
									//истинной!
	
	size_t points_lPointNum=Function->pPlotXYItems.size();	//чтобы каждый раз не вызывать эту функцию
	for (size_t lPointNum =0; lPointNum < points_lPointNum; lPointNum++)
	{
		//координаты контрольной точки
		FP32 x=Function->pPlotXYItems[lPointNum].x;
		FP32 y=Function->pPlotXYItems[lPointNum].y*Function->ProporCoef;	//не забыть промасштабировать
		if(Function->AlwaysDown) y+=m_pGraphicOptions->var_yMin;			//поправка на свойство Always Down
			
		// я НЕ буду отрисовывать точку толко в случае, если ни она, ни предшествующая ей
		// не видны на экране, в противном случае мне нужно отрисовать хотя бы обрубок линии
		if (!IsPointVisible(x,y))
		{//теущая точка не попадает в поле графика по оси Y
			if(Function->ChartType!=G_BARCHART_TIGHT && Function->ChartType!=G_BARCHART_WIDE && Function->ChartType!=G_BARCHART_TIGHT_2)
			{//точку прорисовывать не нужно
				continue; //к следующей точке
			}
			if(x>=m_dXAxisMin && x<=m_dXAxisMax && y>=m_pGraphicOptions->var_yMin)
			{//для вертикальных столбиков я сделаю исключение и буду "подрезать" столбики по 
			//верхней кромке графика
			}
			else
			{
				continue;
			}
		}

		//ТЕКУЩАЯ ТОЧКА ВИДНА
		//запомню ее для всяких последующих полезностей
		INT16S lFuncNum=FromFuncPtrToFuncNum(Function);
		if(lFuncNum!=-1)
		{//функция отработала успешно
			mas_VisiblePoints.push_back(CFuncPointSel(lFuncNum,lPointNum));
			lIsRealOnScreen=true;	//выставлю об этом флаг
		}
		else 
		{
			m_pError->SetERROR(_TEXT("Ошибка при преобразовании указателя в номер функции"), ERP_ERROR | ER_WRITE_TO_LOG);
		}
		
		//переменная определяет находится ли точка в фокусе
		bool IsFocused=false;
		
		if(IsFunctionSelected(lFuncNum))
		{//Если выделена вся функция - подсвечиваются и все ее точки
			IsFocused=true;
		}
		else if(IsPointSelected(lFuncNum,lPointNum))
		{//выделена не функция, но конкретная точка
			IsFocused=true;
		}

		//прорисуем эту точку
		PlotPoints(Function,lPointNum,IsFocused);

		//текущая точка становится предыдущей
		PrevPoint=ThisPoint;
	}//конец цикла по всем точкам функции
}

///////////////// PlotPoints  /////////////////////////////////////////////
/*																		///
	Each Function e.g. DoSineX,DoPlotXY, etc, calls this function as	///
	they calculate each point so that each point can be drawn on the	///
	Takes:																///
	UINT x, UNIT y - the graph co-ord of the point just calculeted		///
	(current point).													///
	UINT prevx, UINT prevy - the co-ords of the previous point			///
	This routine checks what type of plot (line, dot, or bar) is		///
	required and calls the appropriate routine							///
*/																		///
///////////////////////////////////////////////////////////////////////////
void CGraph::PlotPoints(CFunctionClass* Function,size_t PointNum, bool IsFocused)
{
	//here we check the chart type and plot the points accordingly
	//we need to constarin the Y values to keep them within the
	//plot area;

	switch(Function->ChartType)
	{
	case G_BARCHART_TIGHT:
		{//узенькие вертикальные столбики
			DrawBarTight(Function,PointNum,IsFocused);
			break;
		}
	case G_BARCHART_TIGHT_2:
		{//узенькие вертикальные столбики
			DrawBarTight_2(Function,PointNum,IsFocused);
			break;
		}
	case G_BARCHART_WIDE:
		{//широкие вертикальные столбики
			DrawBarWide(Function,PointNum,IsFocused);
			break;
		}
	case G_LINECHART:
		{//непрерывная линия
			DrawConnectLine(Function,PointNum,IsFocused);
			break;
		}
	case G_DOTCHART:
		{//кружочки в точках данных
			DrawDot(Function,PointNum,IsFocused);
			break;
		}
	case G_LINEDOTCHART:
		{//непрерывная линия + кружочки в точках данных
			DrawDot(Function,PointNum,IsFocused);
			DrawConnectLine(Function,PointNum,IsFocused);
			break;
		}
	default:
		{
			m_pError->SetERROR(_TEXT("Выбран неопознанный тип графика!"), ERP_FATALERROR | ER_WRITE_TO_LOG);
		}
	}//SWITCH

	return;
}


/*****************    DrawDot    *********************************************
*		Когда на графике нужно показать точки данных, по которым строится функция.
*	Function - указатель на функцию, которой принадлежит данная точка
*	PointNum - номер точки в функции (она точно видна на экране)
*	IsFocused - выделил ли пользователь эту точку (true - yes)
*****************************************************************************/
void CGraph::DrawDot(CFunctionClass* Function,size_t PointNum, bool IsFocused)
{
	/*
	Для наглядности, будем рисовать не точку, а кружочек в этой точке
	*/
	if (!m_pParent)
	{
		return;
	}
	
	CPen pen, *poldpen;
	if(IsFocused)
	{//если точка выделена - сделаю ее пожирнее
		pen.CreatePen(PS_SOLID,Function->LineWidth*2,Function->Color);
	}
	else
	{
		pen.CreatePen(PS_SOLID,Function->LineWidth,Function->Color);
	}

	poldpen=SecondDC.SelectObject(&pen);

	CPoint ThisPoint=ConvertToGraphCoords(Function->pPlotXYItems[PointNum].x,Function->pPlotXYItems[PointNum].y*Function->ProporCoef);
	
	//радиус кружка
	unsigned char size=IsFocused?4:2;
	CRect r;
	r.top=ThisPoint.y-size;
	r.bottom=ThisPoint.y+size;
	r.left=ThisPoint.x-size;
	r.right=ThisPoint.x+size;
	SecondDC.Ellipse(r);

	//вернем все на свои места
	SecondDC.SelectObject(poldpen);
}


/***********************   DrawConnectLine    ************************************
*	Рисует линию для данной точки, если график отображается линией
*********************************************************************************/
void CGraph::DrawConnectLine(CFunctionClass* Function,size_t PointNum, bool IsFocused)
{
	//Т.к. точка PointNum видима (иначе бы эта функция не вызывалась)
	//то линию к ней от предыдущей точки я рисую ВСЕГДА
	//Однако, если точка PointNum+1 не видима, то для нее эта
	//функция вызываться не будет и мне нужно дополнительно прорисовать 
	//обрубок линии и для нее

	/*CString str; str.Format("%i ",PointNum);
	TRACE(str);*/

	if(!m_pParent)
	{
		return;
	}
	
	CPen pen, *poldpen;
	if(IsFocused)
	{//если функция выделена, то сделаю ее пожирнее
		pen.CreatePen(PS_SOLID,Function->LineWidth*2,Function->Color);
	}
	else
	{
		pen.CreatePen(PS_SOLID,Function->LineWidth,Function->Color);
	}
	poldpen=SecondDC.SelectObject(&pen);
	
	FP32 tx=Function->pPlotXYItems[PointNum].x;
	FP32 ty=Function->pPlotXYItems[PointNum].y*Function->ProporCoef;
	//Обработаю свойство AlwaysDown
	if(Function->AlwaysDown) ty+=m_pGraphicOptions->var_yMin;

	//Прорисую линию к предыдущей точке
	CPoint ThisPoint=ConvertToGraphCoords(tx,ty);
	
	if(PointNum!=0)
	{//есть предыдущая точка
		FP32 px=Function->pPlotXYItems[PointNum-1].x;
		FP32 py=Function->pPlotXYItems[PointNum-1].y*Function->ProporCoef;
		CPoint PrevPoint=ConvertToGraphCoordsEx(tx,ty,px,py);

		//прорисую к ней линию
		SecondDC.MoveTo(PrevPoint);
		SecondDC.LineTo(ThisPoint);
	}
	if(PointNum!=Function->pPlotXYItems.size()-1)
	{//это не последняя точка графика
		FP32 nx=Function->pPlotXYItems[PointNum+1].x;
		FP32 ny=Function->pPlotXYItems[PointNum+1].y*Function->ProporCoef;
		CPoint NextPoint=ConvertToGraphCoordsEx(tx,ty,nx,ny);
		
		//прорисую к ней линию
		SecondDC.MoveTo(NextPoint);
		SecondDC.LineTo(ThisPoint);
	}

	SecondDC.SelectObject(poldpen);
}

////////////////// DrawBarTight  ///////////////////////////////////////////////////
/*																			///
	This draws the bar of a bar chart										///
	The lefthand side of the bar is at point xy.							///
	It also calcultes whether it is a 'positive' bar or a 'negative 'bar'	///
	Some manipulation of the barwidths maybe required depending on what		///
	function is being drawn													///
*/																			///
///////////////////////////////////////////////////////////////////////////////
void CGraph::DrawBarTight(CFunctionClass* Function,size_t PointNum, bool IsFocused)
{
	ASSERT(Function);
	ASSERT(m_pParent);

	//В х и у содержатся координаты В ПИКСЕЛАХ точки, которую нужно отрисовать
	FP32 temp=Function->pPlotXYItems[PointNum].y*Function->ProporCoef;				//координата точки по Y
	if(Function->AlwaysDown) temp+=m_pGraphicOptions->var_yMin;						//поправка к свойству AlwaysDown
	if(temp>m_pGraphicOptions->var_yMax) temp=m_pGraphicOptions->var_yMax;			//буду обрезать макушки
	CPoint ThisPoint=ConvertToGraphCoords(Function->pPlotXYItems[PointNum].x,temp);	//переведу в пикселы
	if(ThisPoint.x==-1)
	{//точка вылезает за границы графика (ошибка, т.к. сюда заходить не должен был)
		return;
	}

	LONG x=ThisPoint.x;
	LONG y=ThisPoint.y;

	UINT barwidth;			//ширина столбика
	if( m_dPixelsPerX >1)
	{
		/*хорошо задумано, но для специфичных целей
		barwidth=m_dPixelsPerX;*/
		barwidth=4;
	}
	else
	{
		barwidth=2;
	}//
	
	CRect rect,dataarea;
	
	dataarea=CalcDataArea();	//вычислим прямоугольник, где отрисовывается график

	//это только для того, чтобы не вылезти справа и слева за область графика
	rect.left=(x-(LONG)barwidth/2)>dataarea.left?x-(LONG)barwidth/2 : dataarea.left;
	rect.right= ((x+(LONG)barwidth/2)<dataarea.right)? x+(LONG)barwidth/2 : dataarea.right;
	//the tricky bit
	//the top/bottom of the bar depends on whether the bar
	//is a negative value or positive value
	
	//note we have need of the Y origin here
	//the Yorigin is an off-set from the bottom of the entire graph bottom
	//and therefore includes the bottom margin -
	// - not from the bottom of the plot area as is our  pixel co-ords
	UINT bmargin =CalcBottomMargin();
	UINT Yorg=m_iOriginY-bmargin;
	
	
	
	//Здесь идет несколько ветвей. Во первых столбики могут идти от нуля как вниз, так и вверх
	//в зависимости от того, положительна или отрицательна функция. Во вторых, если включено
	//свойство AlwaysDown (отображать всё время внизу), то как ни крути, столбики должны идти
	//не от нуля, а от нижней границы графика
	if(Function->AlwaysDown)
	{//столбики в любом случае рисовать от нижней границы графика
		rect.bottom=dataarea.bottom;
		if(y==dataarea.bottom)
		{//только для того, чтобы даже нулевое значение было видно маленькой черточкой внизу экрана
			rect.top=y+1;
		}
		else
		{//действительное значение
			rect.top=y;
		}
	}
	else
	{//обычная отрисовка, столбики начинаются от нуля
		if(y < dataarea.bottom-(LONG)Yorg)
		{//столбик идет снизу вверх (Y был больше 0)
			rect.top=y;
			rect.bottom=dataarea.bottom-Yorg;
		
		}
		else
		{//столбик идет сверху вниз (Y был меньше 0)
			rect.top=dataarea.bottom-Yorg;
			rect.bottom=y;
		}
	}

	CPen pen, *poldpen;
	CBrush brush;	
	
	
	
	brush.CreateSolidBrush(Function->Color);
	pen.CreatePen(PS_SOLID,1,Function->Color);
	poldpen=SecondDC.SelectObject(&pen);
	
	SecondDC.Rectangle(&rect);

	SecondDC.SelectObject(poldpen);
}

/**********************************   DrawBarWide   *********************************************
*		Эта функция рисует столбик, но не узенький, как DrawBarTight, а широкий, так, что границы
*	двух соседних столбиков соприкасаются. Столбики переменной ширины.
*************************************************************************************************/
void CGraph::DrawBarWide(CFunctionClass* Function,size_t PointNum, bool IsFocused)
{
	ASSERT(Function);
	ASSERT(m_pParent);

	//В пикселах:
	// x,y - координаты точки, к которой мы пририсовываем столбик
	// prevX, prevY - координаты предыдущей точки
	// nextX, nextY - координаты последующей точки
	FP32 temp_y=Function->pPlotXYItems[PointNum].y*Function->ProporCoef;
	if(Function->AlwaysDown) temp_y+=m_pGraphicOptions->var_yMin;						//поправка к свойству AlwaysDown
	if(temp_y>m_pGraphicOptions->var_yMax) temp_y=m_pGraphicOptions->var_yMax;			//|
	if(temp_y<m_pGraphicOptions->var_yMin) temp_y=m_pGraphicOptions->var_yMin;			//| буду обрезать макушки

	CPoint ThisPoint=ConvertToGraphCoords(Function->pPlotXYItems[PointNum].x,temp_y);
		
	UINT prevX=0xFFFF;			//координата предыдущей точки
	UINT to_left;				//отклонение влево
	if(PointNum!=0)
	{
		prevX=ConvertToGraph_X(Function->pPlotXYItems[PointNum-1].x);
		to_left=(ThisPoint.x-prevX)/2;
	}
	
	UINT nextX=0xFFFF;			//координата слудующей точки
	UINT to_right;				//отклонение вправо
	if(PointNum!=Function->pPlotXYItems.size()-1)
	{
		nextX=ConvertToGraph_X(Function->pPlotXYItems[PointNum+1].x);
		to_right=(nextX-ThisPoint.x)/2;
	}
	if(nextX==0xFFFF)
	{//это последняя точка, либо следующая точка не видна
		if(prevX!=0xFFFF)
		{//есть точка слева
			to_right=to_left;					//сделаю столбик симметричным
		}
		else
		{//слева ничего нет
			to_right=m_RightMargin-ThisPoint.x;	//до конца графика
		}
	}
	if(prevX==0xFFFF)
	{//это первая точка, либо предыдущая точка не видна
		if(nextX!=0xFFFF)
		{//есть точка справа
			to_left=to_right;					//буду симметричен
		}
		else
		{//справа ничего нет
			to_left=ThisPoint.x-m_LeftMargin;	//отклоню столбик до начала графика
		}
	}

	CRect rect;						//прямоугольник - столбик
	CRect dataarea=CalcDataArea();	//вычислим прямоугольник, где отрисовывается график
	rect.left=ThisPoint.x-to_left;
	if(rect.left<dataarea.left)
	{//вылезли за левую границу
		rect.left=dataarea.left;
	}
	rect.right=ThisPoint.x+to_right;
	if(rect.right>dataarea.right)
	{//вылезли за правую границу
		rect.right=dataarea.right;
	}
	
	//расстояние от нижней границы графика до начала координат
	UINT Yorg=m_iOriginY-m_BottomMargin;
	
	//Здесь идет несколько ветвей. Во первых столбики могут идти от нуля как вниз, так и вверх
	//в зависимости от того, положительна или отрицательна функция. Во вторых, если включено
	//свойство AlwaysDown (отображать всё время внизу), то как ни крути, столбики должны идти
	//не от нуля, а от нижней границы графика
	if(Function->AlwaysDown)
	{//столбики в любом случае рисовать от нижней границы графика
		rect.bottom=dataarea.bottom;
		if(ThisPoint.y==dataarea.bottom)
		{//только для того, чтобы даже нулевое значение было видно маленькой черточкой внизу экрана
			rect.top=ThisPoint.y+1;
		}
		else
		{//действительное значение
			rect.top=ThisPoint.y;
		}
	}
	else
	{//обычная отрисовка, столбики начинаются от нуля
		if(ThisPoint.y < dataarea.bottom-Yorg)
		{//столбик идет снизу вверх (Y был больше 0)
			rect.top=ThisPoint.y;
			rect.bottom=dataarea.bottom-Yorg;
		
		}
		else
		{//столбик идет сверху вниз (Y был меньше 0)
			rect.top=dataarea.bottom-Yorg;
			rect.bottom=ThisPoint.y;
		}
	}
	
	if(rect.bottom==rect.top)
	{//нулевой столбик, нарисуем хотябы линию
		rect.top--;
	}

	//цветовые настройки
	CPen pen, *poldpen;
	CBrush brush, *poldbrush;	
	brush.CreateSolidBrush(Function->ColorEx);
	pen.CreatePen(PS_SOLID,Function->LineWidth,Function->Color);
	poldpen=SecondDC.SelectObject(&pen);
	poldbrush=SecondDC.SelectObject(&brush);
	
	//рисую прямоугольник
	SecondDC.Rectangle(&rect);

	//очищаюсь
	SecondDC.SelectObject(poldpen);
	SecondDC.SelectObject(poldbrush);
}

/****************************   DrawBarTight_2   **************************************************************
*		Представьте такую ситуацию, у нас много графиков, и все
*		они отображаются столбиками. Если их много, то они будут перекрывать друг друга!
*		чтобы этого не произошло, мы будем для каждого графика рисовать столбик поуже, у нас
*		т.о. получится несколько столбиков по соседству, но они не будут перекрываться
*		Этот тип будет работать ТОЛЬКО, если все графики имеют одни и те же точки по оси Х
*		и все графики только этого типа
************************************************************************************************************/
void CGraph::DrawBarTight_2(CFunctionClass* Function,size_t PointNum, bool IsFocused)
{
	ASSERT(Function);
	ASSERT(m_pParent);

	//В пикселах:
	// x,y - координаты точки, к которой мы пририсовываем столбик
	// prevX, prevY - координаты предыдущей точки
	// nextX, nextY - координаты последующей точки

	CPoint ThisPoint=ConvertToGraphCoords(Function->pPlotXYItems[PointNum].x,Function->pPlotXYItems[PointNum].y*Function->ProporCoef);
	CPoint PrevPoint(ThisPoint);
	if(PointNum!=0)
	{
		PrevPoint=ConvertToGraphCoords(Function->pPlotXYItems[PointNum-1].x,Function->pPlotXYItems[PointNum-1].y*Function->ProporCoef);
	}

	CPoint NextPoint(ThisPoint);
	if(PointNum<Function->pPlotXYItems.size()-1)
	{
		NextPoint=ConvertToGraphCoords(Function->pPlotXYItems[PointNum+1].x,Function->pPlotXYItems[PointNum+1].y*Function->ProporCoef);
	}

	int x=ThisPoint.x;
	int y=ThisPoint.y;
	int prevX=PrevPoint.x;
	int prevY=PrevPoint.y;
	int nextX=NextPoint.x;
	int nextY=NextPoint.y;
	
	CRect rect;						//прямоугольник - столбик
	CRect dataarea=CalcDataArea();	//вычислим прямоугольник, где отрисовывается график

	int to_left=to_left=(ThisPoint.x-PrevPoint.x)/2;	//отклонение влево 
	int to_right=to_right=(NextPoint.x-ThisPoint.x)/2;	//отклонение вправо

	if(PointNum==0)
	{//у этой точки нет соседа слева
		to_left=to_right;
	}
	
	if(PointNum==Function->pPlotXYItems.size()-1)
	{//аналогичная ситуация справа
		to_right=to_left;
	}

	if(to_right==to_left==0)
	{//блин, скорее всего это график из одной точки
		to_right=to_left=1;
	}

	rect.left=x-to_left;
	if(rect.left<dataarea.left)
	{//вылезли за левую границу
		rect.left=dataarea.left;
	}
	rect.right=x+to_right;
	if(rect.right>dataarea.right)
	{//вылезли за правую границу
		rect.right=dataarea.right;
	}

	//сделаем так, чтобы кучки столбиков не соприкасались
	if(rect.Width()>40)
	{//не будем мелочиться
		rect.left+=8;
		rect.right-=8;
	}
	else if(rect.Width()>10)
	{//ни рыба, ни мясо
		rect.left+=3;
		rect.right-=3;
	}//а в остальных случаях делать ничего не будем (и так места мало)

	//ТЕПЕРЬ ПОДКОРРЕКТИРУЕМ ШИРУНУ И ПОЛОЖЕНИЕ СТОЛБЦА, ЧТОБЫ ОН ЗАНИМАЛ В КОГОРТЕ ТОЧНО СВОЕ МЕСТО
	int width_for_one=rect.Width()/m_CountVisibleGraphs;	//ширина на каждый из подстолбиков
	int indent_to_left=width_for_one*(m_thisFuncNumber-1);	//отступ влева от общего пространства
	rect.left+=indent_to_left;
	rect.right=rect.left+width_for_one;
	
	UINT bmargin =CalcBottomMargin();
	UINT Yorg=m_iOriginY-bmargin;
	
	//Здесь идет несколько ветвей. Во первых столбики могут идти от нуля как вниз, так и вверх
	//в зависимости от того, положительна или отрицательна функция. Во вторых, если включено
	//свойство AlwaysDown (отображать всё время внизу), то как ни крути, столбики должны идти
	//не от нуля, а от нижней границы графика
	if(Function->AlwaysDown)
	{//столбики в любом случае рисовать от нижней границы графика
		rect.bottom=dataarea.bottom;
		if(y==dataarea.bottom)
		{//только для того, чтобы даже нулевое значение было видно маленькой черточкой внизу экрана
			rect.top=y+1;
		}
		else
		{//действительное значение
			rect.top=y;
		}
	}
	else
	{//обычная отрисовка, столбики начинаются от нуля
		if(y < dataarea.bottom-Yorg)
		{//столбик идет снизу вверх (Y был больше 0)
			rect.top=y;
			rect.bottom=dataarea.bottom-Yorg;
		
		}
		else
		{//столбик идет сверху вниз (Y был меньше 0)
			rect.top=dataarea.bottom-Yorg;
			rect.bottom=y;
		}
	}
	
	if(rect.bottom==rect.top)
	{//нулевой столбик, нарисуем хотябы линию
		rect.top--;
	}

	CPen pen, *poldpen;
	CBrush brush, *poldbrush;	
	
	
	
	brush.CreateSolidBrush(Function->Color);		//закрасим наш столбик
	pen.CreatePen(PS_SOLID,1,Function->ColorEx);	//окаймовку сделаем дополнительным цветом

	poldpen=SecondDC.SelectObject(&pen);
	poldbrush=SecondDC.SelectObject(&brush);
	
	SecondDC.Rectangle(&rect);

	SecondDC.SelectObject(poldpen);
	SecondDC.SelectObject(poldbrush);
}

/********************   ConstrainY(double y)   **********************************
*		Функция работает как нелинейное звено с насыщением
********************************************************************************/
FP32 CGraph::ConstrainY(FP32 y)
{
	if ( (y < m_pGraphicOptions->var_yMax) && (y > m_pGraphicOptions->var_yMin))
	{
		return y;
	}
	else if (y <= m_pGraphicOptions->var_yMin)
	{
		return m_pGraphicOptions->var_yMin;
	}
	else if (y >=m_pGraphicOptions->var_yMax)
	{
		return m_pGraphicOptions->var_yMax;
	}
	return y; //здесь я беть не должен
}

/******************     GetMultiPlotPenColor    **********************************
*		Когда мне надоедает выдумывать цвета графика, лучше воспользоваться
*	хорошо подобранной палитрой.
*********************************************************************************/
COLORREF CGraph::GetMultiPlotPenColor(UINT PenNumber)
{
	switch(PenNumber)
	{
	case 0:
		return RGB(255,0,0);	// красный
	case 1:
		return RGB(0,0,0);		// чёрный
	case 2:
		return RGB(16,123,82);	// ~зеленый
	case 3:
		return RGB(0,0,255);	// синий
	case 4:
		return RGB(15,146,189);	// ~голубой
	case 5:
		return RGB(205,143,0);	// ~оранжевый
	case 6:
		return RGB(0,255,255);
	case 7:
		return RGB(255,0,255);
	default:
		return RGB(rand()%255,rand()%255,rand()%255);
	}
	return RGB(255,0,0);		//здесь я быть не должен
}

/************************    SetRegime    ***************************************
*		Устанавливает режим работы графика.
********************************************************************************/
bool	CGraph::SetRegime(INT8U Regime)
{
	if(Regime==GRG_NOTINITIALIZED)	return false;	//в этот режим нельзя перейти вручную
	if(m_Regime==Regime)			return true;	//режим менять не нужно
	switch(m_Regime)
	{
	case GRG_NOTINITIALIZED:
		break;
	case GRG_STOP:
		break;
	case GRG_PAUSE:
		break;
	case GRG_PLAY:
		break;
	case GRG_RESTART:			//в этом режиме я не могу находиться
		return false;			//т.к. он проскакивает мгновенно
	default:
		return false;			//неопознанный режим
	}
	switch(Regime)
	{
	case GRG_STOP:
		m_Regime=Regime;
		break;
	case GRG_PAUSE:
		{
			if(m_Regime==GRG_STOP)
			{//из стопа в паузу переходить нельзя
				return false;
			}
			//ничего не нужно делать, т.к. var_X_view уже в нужном положении
			m_Regime=Regime;
			break;
		}
	case GRG_PLAY:
		{
			if(m_Regime==GRG_STOP || m_Regime==GRG_NOTINITIALIZED)
			{//после стопа график можно только перезапустить
				//!!! НЕ УБИРАТЬ, ИДЕТ ПЕРЕХОД НА GRG_RESTART!!!
			}
			else
			{//ничего не нужно делать т.к. var_X_view перебросится автоматически при 
			//отрисовке
				m_Regime=Regime;
				break;
			}
		}
	case GRG_RESTART:
		{
			//очистим уже имеющуюся информацию
			for(int i=0; i<VecFunctions.size();i++)
			{
				VecFunctions[i].pPlotXYItems.clear();
			}
			//запомним начальное время построения графика
			m_StartTime=GetTickCount();	
			m_Regime=GRG_PLAY;			//т.к. я фактически стал проигрывать снова
			break;
		}
	default:
		return false;			//неопознанный режим
	}
	return true;				//всё прошло нормально
}

/************************    GetRegime    ***************************************
*		Возвращает режим работы графика.
********************************************************************************/
INT8U	CGraph::GetRegime()
{
	return m_Regime;
}

/*********************   GetMoveAxisOffsetTime()   *******************************
*		Если мы работаем с сеткой, которая плывет по экарану вместе с графиком, то 
*	нам нужно в каждый момент прорисовки находить ее смещение от левого края. Эта 
*	функция находит такое смещение во времени.
*********************************************************************************/
void CGraph::GetMoveAxisOffsetTime(float &offset,float& step)
{
	//offset-начальное смещение от левой оси
	//step	-время между отсчетами вертикальных линий
	//время на один тик сетки
	float time_per_tick=m_pGraphicOptions->var_VisibleTime/m_pGraphicOptions->m_oHorisontalTicks;
	if(time_per_tick==0)
	{//а иначе я уйду в бесконечный цикл
		m_pError->SetERROR(_TEXT("Ошибка при вычислении разметки плавающей сетки графика"), ERP_FATALERROR | ER_WRITE_TO_LOG);
		offset=0;
		step=1;
		return;
	}

	float temp=0;
	do
	{	//
		if(temp>=m_dXAxisMin)
		{//заехали, пора выходить
			break;
		}
		temp+=time_per_tick;	//нарастим значение
	}
	while(1);
	step=time_per_tick;
	offset=temp-m_dXAxisMin;
}

/****************    SetGraphSize(int height, int width)  ************************
*		Задает размеры графика
**********************************************************************************/
void CGraph::SetGraphSize(int height, int width)
{
	//Чтобы не происходило коллизицй, будем проверять на минимально доступные
	//размеры.
	//Если аргумент равен нулю, то размер не меняется (ДЛЯ ВОЗМОЖНОСТИ
	// ИЗМЕНЕНИЯ ТОЛЬКО ОДНОГО ИЗ РАЗМЕРОВ)

	//m_pGraphicOptions может быть не проинициализирован, когда родительское окно
	//только создалось и устанавливает еще свои размеры
	if(m_pGraphicOptions!=NULL)
	{
		if(width!=0)
		{
			m_pGraphicOptions->var_GraphWidth = width < G_MINGRAPHWIDTH?G_MINGRAPHWIDTH:width;
		}
		if(height!=0)
		{
			m_pGraphicOptions->var_GraphHeight=height < G_MINGRAPHHEIGHT ?G_MINGRAPHHEIGHT:height;
		}
		if(m_hWnd!=NULL)
		{//это может быть, если окно еще не успело создаться
			SetWindowPos(NULL,0,0,m_pGraphicOptions->var_GraphWidth,m_pGraphicOptions->var_GraphHeight, SWP_NOMOVE | SWP_NOREPOSITION | SWP_NOREDRAW);
			//SWP_NOREDRAW чтобы избежать мерцания
		}
		//в зависимости от размера графика поменяю размер шрифта
		CreateGraphFont(_TEXT("Courier"));
	}
}

/***************   SetGraphPos(int x,int y)   ***************************************************
*		Эта функция выставляет положение левого верхнего угла картинки графика на
*	родительском окне
***********************************************************************************************/
void CGraph::SetGraphPos(int x,int y)
{
	if(y>=0)
	{m_pGraphicOptions->var_Y_pos=y;}
	else
	{m_pGraphicOptions->var_Y_pos=0;}
	if(x>=0)
	{m_pGraphicOptions->var_X_pos=x;}
	else
	{m_pGraphicOptions->var_X_pos=0;}
}

/*************************   GetOffsetTime()   ************************************** 
*		Выдает смещение во времени между началом построения графика и моментом вызова функции
*	в СЕКУНДАХ
**************************************************************************************/
float CGraph::GetOffsetTime()
{
	return float(GetTickCount()-m_StartTime)/1000.f;
}

/************************    SetX_Offset    *****************************************
*		устанавливает смещение левой границы визуализиции графика (если не GRG_PLAY)
************************************************************************************/
bool CGraph::SetX_Offset(FP32 x_offset)
{
	if(GetRegime()==GRG_PLAY) return false;	//в этом режиме не могу прокручивать график
	//пока не ришил допускать ли < 0
	m_pGraphicOptions->var_X_view=x_offset;
	return true;
}

/*************************    SaveGraph(CString FileName)   *************************
*      Сохраняет график по укзанному в аргументе имени файла.
*	Если все проходит нормально, возвращается истина.
*	! Сохраняются только видимые функции.
*	!!! Формат файла графика описан в файле FileFormats.txt
*************************************************************************************/
bool CGraph::SaveGraph(const TCHAR* FileName)
{
	//куда сохраниться график
	ofstream m_SaveFile;

	//Откроем файл, куда сохранить график
	m_SaveFile.open(FileName,ios::trunc | ios::out | ios::binary);
	if(!m_SaveFile)
	{
		CString str=FileName;
		m_pError->SetERROR(_TEXT("Не создался файл \r\n ")+str,ERP_WARNING | ER_WRITE_TO_LOG);		
		return false;
	}
			
	//пропишем версию формата файла, расшифровка константы дана в CFunctionClass.h
	m_SaveFile.write(VERSION_GRAPH_FILE,VERSION_GRAPH_FILE_LEN);

	//Сохраню название графика
	INT16U lGraphTitleLength=m_pGraphicOptions->m_GraphName.GetLength();
	if(lGraphTitleLength>255)
	{
		m_pError->SetERROR(_TEXT("Длина названия графика не может быть больше 255 символов"), ERP_WARNING | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
		m_SaveFile.close(); //закроем файл, чтобы дать работать с ним другим ресурсам
		return false;							
	}
	
	m_SaveFile.write((const char*)&lGraphTitleLength, sizeof(INT8U));
	m_SaveFile <<'$';  //разделитель полей
	//Т.к. я собираюсь поддерживать unicode, а менять формать файла пока не буду
	//(пока имена в ANSI), то мне нужно поставить конвертацию
	USES_CONVERSION;
	LPSTR lGraphName=T2A((LPTSTR)m_pGraphicOptions->m_GraphName.GetBuffer());
	m_SaveFile.write(lGraphName,lGraphTitleLength);
	m_SaveFile <<'$';  //разделитель полей

	//Сохраним данные графиков
	for(size_t i=0; i<VecFunctions.size();i++)
	{
		if(VecFunctions[i].Visible)
		{//!Сохранять буду только видимые фукнции
			if(!VecFunctions[i].WriteFunction(m_SaveFile))
			{
				CString str; str.Format(_T("При записи функции №%u произошла ошибка, запись прервана"), i);
				m_pError->SetERROR(str,ERP_ERROR | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
				m_SaveFile.close(); //закроем файл, чтобы дать работать с ним другим ресурсам
				return false;
			}
		}
	}
	
	m_SaveFile.close(); //закроем файл, чтобы дать работать с ним другим ресурсам
	return true;
}

/***********************    LoadGraph   *******************************************
*		Открывает график из определенного файла. Старый график удаляется.
*	Если все нормально - возвращается истина. Здесь две версии, т.к. у меня возникали
*	проблемы, когда проект GRAPHIC был ANSI а внешний проект был UNICODE, то возникал
*	неразбериха.
***********************************************************************************/
bool CGraph::LoadGraph(const CHAR* FileName)
{
	//это ANSI версия, которая трансформирует строку и вызывает
	//UNICODE версию
	USES_CONVERSION;
	return LoadGraph(A2W(FileName));
}
bool CGraph::LoadGraph(const WCHAR* FileName)
{
	VecFunctions.clear();			//удалю все старое
	LoadGraphToExists(FileName);	//добавлю новое
	return true;
}

/************************    LoadGraphToExists    **********************************************
*		Функция добавляет к уже отображаемым функциям новые, считанные из файла.
*	FileName - имя файла из которого считывать
*	Если все нормально - возвращается истина, в противном случае ложь.
************************************************************************************************/
bool CGraph::LoadGraphToExists(const CHAR* FileName)
{
	//это ANSI версия, которая трансформирует строку и вызывает
	//UNICODE версию
	USES_CONVERSION;
	return LoadGraphToExists(A2W(FileName));
}
bool CGraph::LoadGraphToExists(const WCHAR* FileName)
{
	ifstream lLoadFile;
	CStringW lFileName=FileName;

	//Откроем файл, откуда загрузить график
	lLoadFile.open(lFileName, ios::binary);
	if(!lLoadFile.is_open())
	{
		m_pError->SetERROR(L"Не открылся файл \r\n "+lFileName+L" для загрузки графика.", ERP_WARNING | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);		
		return false;
	}
	else
	{//файл открыт, пока все нормально
		SetRegime(GRG_STOP);						//остановим график и подчистим функции

		char lVersion[VERSION_GRAPH_FILE_LEN+1];
		
		/*
		Планируется доработка формата файла, поэтому, если верси не та, то 
		просто не будем ее обрабатывать
		*/
		lLoadFile.read(lVersion,VERSION_GRAPH_FILE_LEN);
		lVersion[VERSION_GRAPH_FILE_LEN]=0;		//чтобы считанная строка корректно оканчивалась для сравнения
		//в зависимости от версии у меня разные алгоритмы чтения
		if(strcmp(lVersion,"ver 1.0\0")==0)
		{//ВЕРСИЯ 1.0
			size_t i=VecFunctions.size();
			while(lLoadFile.peek()!=EOF)
			{
				VecFunctions.push_back(CFunctionClass());
				if(!VecFunctions[i].ReadFunction_1_0(lLoadFile))
				{
					CString str; str.Format(_TEXT("При чтении функции №%i из файла возникли ошибки, процесс чтения остановлен"),i+1);
					m_pError->SetERROR(str, ERP_ERROR | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
					lLoadFile.close();
					return false;
				}
				i++;	//к следующей функции
			}
		}
		else if(strcmp(lVersion,"ver 1.1\0")==0)
		{//ВЕРСИЯ 1.1
			//считаю название графика
			INT8U lGraphTitleLength;
			char lSeparator=0;
			lLoadFile.read((char *)&lGraphTitleLength,sizeof(INT8U));
			lLoadFile.read(&lSeparator,sizeof(INT8U));						//прочтем разделитель
			if(lSeparator!='$')
			{
				m_pError->SetERROR(_TEXT("В файле версии 1.1 неверно поле названия графика"), ERP_WARNING | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
				lLoadFile.close();
				return false;
			}
			lSeparator=0;

			INT8U lGraphTitle[255];
			lLoadFile.read((char *)lGraphTitle,lGraphTitleLength);
			lGraphTitle[lGraphTitleLength]=0x00;					//закрою строку
			lLoadFile.read(&lSeparator,sizeof(INT8U));						//прочтем разделитель
			if(lSeparator!='$')
			{
				m_pError->SetERROR(_TEXT("В файле версии 1.1 неверно поле названия графика"), ERP_WARNING | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
				lLoadFile.close();
				return false;
			}

			//Последовательно прочитаю все функции
			size_t i=VecFunctions.size();
			while(lLoadFile.peek()!=EOF)
			{
				VecFunctions.push_back(CFunctionClass());
				if(!VecFunctions[i].ReadFunction_1_1(lLoadFile))
				{
					CString str; str.Format(_TEXT("При чтении функции №%i из файла возникли ошибки, процесс чтения остановлен"),i+1);
					m_pError->SetERROR(str, ERP_ERROR | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
					lLoadFile.close();
					return false;
				}
				i++;	//к следующей функции
			}
			m_pGraphicOptions->m_GraphName=lGraphTitle;	//Сохраню название графика
		}
		else
		{
			CString str; str.Format(_TEXT("Неверный формат файла графика \"%s\", попробуйте выбрать корректный файл"), FileName);
			m_pError->SetERROR(str,ERP_ERROR | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
			lLoadFile.close(); //закроем файл, чтобы дать работать с ним другим ресурсам
			return false;
		}

		lLoadFile.close();
		FP32 lMaxTime=GetMaxGraphTime();
		FP32 lMinTime=GetMinGraphTime();
		SetRegime(GRG_STOP);		//остановлю
		//отображу всё, что загрузил
		m_pGraphicOptions->var_VisibleTime=lMaxTime-lMinTime;
		SetX_Offset(lMinTime);
		m_pGraphicOptions->var_yMax=ceil(GetMaxGraphValue())+2;
		FP32 lMin=GetMinGraphValue();
		if(lMin>0)
		{
			m_pGraphicOptions->var_yMin=0;		
		}
		else
		{
			m_pGraphicOptions->var_yMin=floor(lMin)-2;
		}
	}
	return true;
}

/***********************   SaveGraphMathcad(char* FileName, INT16S FuncNum=-1)   ******************
*		Сохранение данных графика в формате для mathcad.
*	FileName	- имя файла, куда сохранять
*	FuncNum		- номер функции, которую сохранять, т.к. сохранять мы
*					можем только одну функцию.
*					-1 -сохранять первую видимую
*	Если все нормально, то возвращается истина. В противном случае - ложь
**************************************************************************************************/
bool CGraph::SaveGraphMathcad(const TCHAR* FileName, INT16S FuncNum)
{
	//куда сохраниться график
	ofstream m_SaveFile;
	CString lFileName=FileName;

	//Откроем файл, куда сохранить график
	if(m_SaveFile.is_open()) m_SaveFile.close();//на всякий случай
	m_SaveFile.open(lFileName,ios::trunc | ios::out /*| ios::binary*/);
	if(!m_SaveFile)
	{
		m_pError->SetERROR(_TEXT("Не создался файл \r\n для сохранения графика в формате Mathcad")+lFileName,ERP_WARNING | ER_WRITE_TO_LOG);		
		return false;
	}
	else
	{//файл открыт, пока все нормально
		CFunctionClass *tmp_func=NULL;		//указатель на функцию, которую будем писать
		if(FuncNum==-1)
		{//нужно сохранить первую видимую функцию
			for(unsigned int i=0; i<VecFunctions.size();i++)
			{//пройдемся по всем функциям
				if (VecFunctions[i].Visible)
				{
					tmp_func=&VecFunctions[i];
					break;
				}
			}
			if(tmp_func==NULL)
			{//цикл дошел до конца, а ни одна функция не подошла
				m_pError->SetERROR(_TEXT("При сохранении данных графика для mathcad, оказалось, что все графики скрыты"),ERP_WATCHFUL | ER_WRITE_TO_LOG);
				return false;
			}
		}
		else
		{//номер функции, которую сохранять четко задан
			if(FuncNum<0 || (size_t)FuncNum>VecFunctions.size())
			{
				m_pError->SetERROR(_TEXT("При сохранении графика в формате Mathcad передан несуществующий номер функции"), ERP_WARNING | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
				return false;
			}
			tmp_func=&VecFunctions[FuncNum];
		}

		//Формат файла следующий: строка чисел в ASCII, разделенных запятыми. Первая строка -
		//верхняя строка итоговой матрицы. Вторая - соответственно и т.д.
		INT64U i;
		for(i=0; i<tmp_func->pPlotXYItems.size(); i++)
		{//допустим, наверху будут отсчеты по времени
			m_SaveFile<<tmp_func->pPlotXYItems[i].x;
			m_SaveFile<<',';
		}
		m_SaveFile<<endl;	//перейдем на следующую строку
		for(i=0; i<tmp_func->pPlotXYItems.size(); i++)
		{//а внизу сами данные
			m_SaveFile<<tmp_func->pPlotXYItems[i].y;
			m_SaveFile<<',';
		}
		//вообще-то в конце каждой строки остались лишние запятые, но mathcad съедает и это

		m_SaveFile.close(); //закроем файл, чтобы дать работать с ним другим ресурсам
	}

	return true;	//все прошло нормально
}

/*********************    SaveGraphBMP    *************************
*		Сохранение данных графика в формате BMP.
******************************************************************/
bool CGraph::SaveGraphBMP(const TCHAR* FileName)
{
	//файл, куда будем сохранять изображение
	ofstream SaveBMP;

	SaveBMP.open(FileName,ios::trunc | ios::out | ios::binary);
	if(!SaveBMP)
	{
		m_pError->SetERROR(_T("Не создался файл \r\n ")+CString(FileName)+_T("для сохранения графика") ,ERP_ERROR | ER_WRITE_TO_LOG);
		return false;
	}

	//это для того, чтобы с окна исчезли все следы от предыдущих окон
	this->UpdateWindow();

	BITMAP bmp;
	BITMAPINFO *pbmi,bmi;
	BITMAPFILEHEADER bfh;
	WORD wClrBits;
	//HANDLE hFile;
	LPBYTE lpData;
	HDC hTempDC;
	
	CBitmap* MainBitmap= new CBitmap; 
	CDC* MainCDC = new CDC; 

	//Инициализучим 
	CClientDC dc(this); 
	MainBitmap->CreateCompatibleBitmap(&dc,m_pGraphicOptions->var_GraphWidth,m_pGraphicOptions->var_GraphHeight); 
	MainCDC->CreateCompatibleDC(&dc); 
	MainCDC->SelectObject(MainBitmap); 

	//А далее все просто, копируем битмап, как при выводе на экран, только обратно. :)))) 

	MainCDC->BitBlt(0,0,m_pGraphicOptions->var_GraphWidth,m_pGraphicOptions->var_GraphHeight,&dc,0,0,SRCCOPY); 
	/*Теперь в MainBitmap находится клиентская часть окна*/

	//Получение параметров рисунка
	if (!GetObject(HBITMAP(*MainBitmap),sizeof(bmp),&bmp))
	{
		m_pError->SetERROR(_T("Ошибка при сохранении рисунка"),ERP_ERROR | ER_WRITE_TO_LOG);
		SaveBMP.close(); //закроем файл
		return false;
	}
	
	//Количество битов под пиксель
	wClrBits = (WORD)(bmp.bmPlanes*bmp.bmBitsPixel);
	if (wClrBits == 1)
	{
	}
	else if (wClrBits <=4)
	{
		wClrBits = 4;
	}
	else if (wClrBits <=8)
	{
		wClrBits = 8;
	}
	else if (wClrBits <=16)
	{
		wClrBits = 16;
	}
	else if (wClrBits <=24)
	{
		wClrBits = 24;
	}
	else if (wClrBits <=32)
	{
		wClrBits = 32;
	}

	//Выделение памяти для BITMAPINFO
	if (wClrBits !=24)
	{
		pbmi = (PBITMAPINFO)LocalAlloc(LPTR,sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*(1<<wClrBits));
	}
	else
	{
		pbmi = (PBITMAPINFO)LocalAlloc(LPTR,sizeof(BITMAPINFOHEADER));
	}

	//Заполнение BITMAPINFO
	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biHeight = bmp.bmHeight;
	pbmi->bmiHeader.biWidth = bmp.bmWidth;
	pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
	pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
	if (wClrBits <24)
	{
		pbmi->bmiHeader.biClrUsed = (1<<wClrBits);
	}
	
	pbmi->bmiHeader.biCompression = BI_RGB;

	//..количество памяти, необходимое для таблицы цветов
	pbmi->bmiHeader.biSizeImage = ((bmp.bmWidth*wClrBits+31)&~31)/8 * bmp.bmHeight;
	pbmi->bmiHeader.biClrImportant = 0;

	
	//Получение памяти под таблицу цветов и массив индексов
	lpData = (LPBYTE) GlobalAlloc(GMEM_FIXED,pbmi->bmiHeader.biSizeImage);
	if (!lpData)
	{
		SaveBMP.close(); //закроем файл
		return false;
	}
	
	//Копирование таблицы цветов в выделенную область памяти
	/*hDC = MainCDC->m_hDC;*/
	hTempDC = CreateCompatibleDC(MainCDC->m_hDC);
	ReleaseDC(MainCDC);
	SelectObject(MainCDC->m_hDC,HBITMAP (*MainBitmap));
	bmi = *pbmi;
	if (!GetDIBits(hTempDC,HBITMAP (*MainBitmap),0,(WORD)bmp.bmHeight,lpData,&bmi,DIB_RGB_COLORS))
	{
		ReleaseDC(MainCDC);
		DeleteDC(hTempDC);
		GlobalFree((HGLOBAL) lpData);
		SaveBMP.close(); //закроем файл
		return false;
	}
	ReleaseDC(MainCDC);
	DeleteDC(hTempDC);

	//Заполняем структуру информации о файле
	bfh.bfType = 0x4d42;
	bfh.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) + pbmi->bmiHeader.biSize + pbmi->bmiHeader.biClrUsed*sizeof(RGBQUAD) + pbmi->bmiHeader.biSizeImage);
	bfh.bfReserved1 = 0;
	bfh.bfReserved2 = 0;
	//Смещение данных рисунка от начала файлаэ
	bfh.bfOffBits = (DWORD)(sizeof(BITMAPFILEHEADER) + pbmi->bmiHeader.biSize +	pbmi->bmiHeader.biClrUsed*sizeof(RGBQUAD));

	//Запись данных в файл
	//..BITMAPFILEHEADER
	if (!SaveBMP.write((char *)&bfh,sizeof(bfh)))
	{
		GlobalFree(lpData);
		//CloseHandle(hFile);
		SaveBMP.close(); //закроем файл
		return false;
	}
	//..BITMAPINFOHEADER и RGBQUAD
	if (!SaveBMP.write((char *)&pbmi->bmiHeader,sizeof(BITMAPINFOHEADER) + pbmi->bmiHeader.biClrUsed*sizeof(RGBQUAD)))
	{
		GlobalFree(lpData);
		//CloseHandle(hFile);
		SaveBMP.close(); //закроем файл
		return false;
	}
	//..массив цветов и индексов
	if (!SaveBMP.write((char *)lpData,pbmi->bmiHeader.biSizeImage))
	{
		GlobalFree(lpData);
		//CloseHandle(hFile);
		SaveBMP.close(); //закроем файл
		return false;
	}

	//Запись закончена
	SaveBMP.close(); //закроем файл

	//подчистим за собой
	MainBitmap->DeleteObject();
	MainCDC->Detach();
	delete MainBitmap;
	delete MainCDC;

	m_pError->SetERROR(_T("Файл графика \r\n ")+CString(FileName)+_T("успешно записан") , ERP_GOOD | ER_WRITE_TO_LOG);

	return true;
}

/****************************    GetVisibleFunctionsCount()   ******************************
*		Выдает количество видимых функций
********************************************************************************************/
INT16U CGraph::GetVisibleFunctionsCount()
{
	INT16U count=0;
	for(INT16U i=0; i<VecFunctions.size(); i++)
	{
		if(VecFunctions[i].Visible)
		{
			count++;
		}
	}
	return count;
}

/***************************************************************************
*							РАБОТА С МЫШЬЮ
***************************************************************************/
void CGraph::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(m_ZoomState==ZOOM_FOCUS_ENDED)
	{//пользователь уже предварительно выделил область
		if(!m_bCursorInFocus)
		{//нажали кнопку где-то снаружи
			m_ZoomState=ZOOM_NOT_ACTIVE;		//я закончил выделение
			m_LThisPoint.SetPoint(-1,-1);		//чтобы потом не отрисовывать ложные прямоугольники
		}
		else
		{//ага! пользователь хочет-таки увеличить
			FP32 lNewLeft,lNewRight,lNewTop, lNewBottom;
			ConvertToReal_X(m_ZoomRect.left,&lNewLeft,true);
			ConvertToReal_X(m_ZoomRect.right,&lNewRight,true);
			ConvertToReal_Y(m_ZoomRect.top,&lNewTop,true);
			ConvertToReal_Y(m_ZoomRect.bottom,&lNewBottom,true);
			if(lNewLeft!=lNewRight && lNewTop!=lNewBottom)
			{//выделение корректно
				m_pGraphicOptions->var_yMin=lNewBottom;
				m_pGraphicOptions->var_yMax=lNewTop;
				SetX_Offset(lNewLeft);
				m_pGraphicOptions->var_VisibleTime=lNewRight-lNewLeft;
			}
			m_ZoomState=ZOOM_NOT_ACTIVE;	//закончил масштабирование
			//поменяю курсор
			HCURSOR hCursor;
			hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW); 
			SetClassLong(this->m_hWnd,GCL_HCURSOR,(LONG)hCursor);
			return;							//чтобы автоматически не начать заного
		}
	}
	if(m_ZoomState==ZOOM_NOT_ACTIVE)
	{//пользователь только начинает выделять область для увеличения
		m_ZoomState=ZOOM_FOCUSING;
		m_LDownPoint=point;			//запомню точку, где я начал выделять
		m_LThisPoint=point;			//я же здесь!
		m_ZoomRect.SetRectEmpty();	//я еще не прорисовывал рамок выделения

		//Запрещу вывод курсора за пределы окна
		GetClipCursor(&m_LOldScreenRect);
		RECT lClip;								//|
		GetWindowRect(&lClip);					//|
		ClipCursor(&lClip);						//|новый рект для курсора
	}
}

void CGraph::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_ZoomState==ZOOM_FOCUSING)
	{//пользователь выделяет область
		if(abs(m_LDownPoint.x-point.x)>2 || abs(m_LDownPoint.y-point.y)>2)
		{//мы отодвинулись достатьчно от начальной точки, чтобы засчитать выделение
			m_ZoomState=ZOOM_FOCUS_ENDED;
		}
		else
		{//можно считать что это не выделение, а просто щелчёк
			m_ZoomState=ZOOM_NOT_ACTIVE;
			m_LThisPoint.SetPoint(-1,-1);		//чтобы потом не отрисовывать ложные прямоугольники
		}
		ClipCursor(&m_LOldScreenRect);			//восстановлю область перемещения курсора
	}
}

void CGraph::OnRButtonDown(UINT nFlags, CPoint point)
{
	m_DownPoint=point;			//запомню точку, где я начал выделять
	m_bStartFocus=true;			//запомню, что я начал выделять
	m_LastRect.SetRectEmpty();	//я еще не прорисовывал рамок выделения

/*ЕСЛИ ВЫДЕЛЕНИЕ РАБОТАЕТ НОРМАЛЬНО, ТО УДАЛИТЬ
	//Сложилась скользкая ситуация, когда я выделяю что-то и покидаю курсором окно, то
	//после отжатия левой кнопки мыши выделение не пропадает, поэтому мне пришлось 
	//сбрасывать выделение каждый раз, когда курсор покидает окно
	//но это делается хитрожопо!
	TRACKMOUSEEVENT tmp;
	tmp.cbSize=sizeof(TRACKMOUSEEVENT);
	tmp.dwFlags=TME_LEAVE;
	tmp.hwndTrack=this->m_hWnd;
	TrackMouseEvent(&tmp);
*/
	GetClipCursor(&m_OldScreenRect);
	RECT lClip;								//|
	GetWindowRect(&lClip);					//|
	ClipCursor(&lClip);						//|новый рект для курсора
}

void CGraph::OnRButtonUp(UINT nFlags, CPoint point)
{
	if(m_bStartFocus)
	{//только, если я действительно начинал выделение (а не завел с нажатой кнопкой на график)
		ClipCursor(&m_OldScreenRect);		//восстановлю область перемещения курсора
		m_bStartFocus=false;				//я закончил выделение
		m_ThisPoint.SetPoint(-1,-1);		//чтобы потом не отрисовывать ложные прямоугольники
		if(point==m_DownPoint)
		{//это был одиночный клик
			mas_SelectionPoints.clear();	//очищу массив выделенных точек
			mas_SelectionFunctoins.clear();	//очищу старые выделенные функции;
			Invalidate();					//перерисую, чтобы снять выделения
		}
	}
}

/*****************    OnMouseMove(UINT nFlags, CPoint point)   *************
*		Эта функция мониторинга перетаскивания опции
***************************************************************************/
void CGraph::OnMouseMove(UINT nFlags, CPoint point)
{
	bool lNeedToInvalidate=false;	//чтобы не отрисовывать по 10 раз

	if(m_ZoomState!=ZOOM_NOT_ACTIVE)
	{//идет процесс масштабирования
		if(nFlags & MK_LBUTTON)
		{//левая кнопка мыши нажата
			m_LThisPoint=point;		//до куда идет выделение
			lNeedToInvalidate=true;	//перерисую
		}
		else
		{//пользователь уже выделил область и теперь просто водит мышью по экрану
			if(m_ZoomRect.PtInRect(point))
			{//куросор в области выделения
				if(!m_bCursorInFocus)
				{//пользователь пересек краницу в область выделения
					m_bCursorInFocus=true;
					//поменяю курсор
					HCURSOR hCursor;
					hCursor = AfxGetApp()->LoadStandardCursor(IDC_HAND); 
					SetClassLong(this->m_hWnd,GCL_HCURSOR,(LONG)hCursor);
				}
			}
			else if(m_bCursorInFocus)
			{//пользователь покидает границу области выделения
				m_bCursorInFocus=false;
				//поменяю курсор
				HCURSOR hCursor;
				hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW); 
				SetClassLong(this->m_hWnd,GCL_HCURSOR,(LONG)hCursor);
			}
		}
	}

	if(m_bStartFocus)
	{//шло выделение правой кнопкой мыши
		m_ThisPoint=point;		//до куда идет выделение

		mas_SelectionPoints.clear();	//очищу старые выделенные точки
		mas_SelectionFunctoins.clear();	//очищу старые выделенные функции;
		vector<size_t> tmp;	//чтобы при выборе функции они не повторялись

		size_t lVisiblePointsCount=mas_VisiblePoints.size();
		for(size_t i=0; i<lVisiblePointsCount; i++)
		{
			size_t lFuncNum=mas_VisiblePoints[i].m_FuncNum;
			FP32 lx=VecFunctions[lFuncNum].pPlotXYItems[mas_VisiblePoints[i].m_PointNum].x;
			FP32 ly=VecFunctions[lFuncNum].pPlotXYItems[mas_VisiblePoints[i].m_PointNum].y*VecFunctions[lFuncNum].ProporCoef;

			CPoint lPoint(ConvertToGraphCoords(lx,ly));
			
			if(lPoint.x!=-1)
			{//точка может не попасть на экран в случае если она ушла вверх или
			//вниз
				CRect rect(GetFocusRect(m_DownPoint,m_ThisPoint));
				if(rect.PtInRect(lPoint))
				{//данная точка данной функции выделена
					//Теперь мне нужно разобраться - выделять
					//отдельную точку или всю функцию. Если функция отображается
					if(nFlags & MK_SHIFT)
					{//при зажатом  shift выделяю всю функцию
						if(find(tmp.begin(),tmp.end(),lFuncNum)==tmp.end())
						{//такой функции больше нет в списке
							mas_SelectionFunctoins.push_back(CFuncFuncSel(lFuncNum));
							tmp.push_back(lFuncNum);
						}
					}
					else
					{//добавлю точку в массив выделенных точек
						mas_SelectionPoints.push_back(mas_VisiblePoints[i]);
					}
				}
			}
			
		}

		lNeedToInvalidate=true;	//перерисую
	}

	if(lNeedToInvalidate)
		Invalidate();			//нужно прорисовать изменения
}

void CGraph::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	vector<CFuncNameRect>::reverse_iterator Iter=mas_FuncNameRects.rbegin();
	for(Iter;Iter<mas_FuncNameRects.rend();Iter++)
	{
		if(Iter->m_FuncNameRect.PtInRect(point))
		{//да, кликнули на имени функции
			//отправлю об этом сообщение родителю
			m_pParent->SendMessage(MSG_GR_FUNC_NAME_SEL,Iter->m_FuncNum,0);
		}
	}
}


BOOL CGraph::PreTranslateMessage(MSG* pMsg)
{
	switch(pMsg->message)
	{
	/*ЕСЛИ ВЫДЕЛЕНИЕ РАБОТАЕТ НОРМАЛЬНО, ТО УДАЛИТЬ
	case WM_MOUSELEAVE:
		{
			//Это не простое сообщение, чтобы оно появилось нужно уметь работать с 
			//функцией TrackMouseEvent. Курсор покинул окно с графиком и чтобы не 
			//было дальнейших неприятностей я очищу выделение
			m_bStartFocus=false;
			m_ThisPoint.SetPoint(-1,-1);	//чтобы потом не отрисовывать ложные прямоугольники
			break;
		}
	*/
	case WM_KEYDOWN:	
		{//была нажата кнопка
			if(GetKeyState(VK_CONTROL)>>1)
			{//двигать функцию буду, только при нажатом контроле
				switch(pMsg->wParam)
				{
					case VK_LEFT:
					{//переместить выделенные точки (функции) влево
						MoveSelectedPointsLeft();
						MoveSelectedFunctionsLeft();
						Invalidate();
						break;
					}
					case VK_RIGHT:
					{//переместить выделенные точки (функции) вправо
						MoveSelectedPointsRight();
						MoveSelectedFunctionsRight();
						Invalidate();
						break;
					}
					case VK_UP:
					{//переместить выделенные точки (функции) вверх
						MoveSelectedPointsUp();
						MoveSelectedFunctionsUp();
						Invalidate();
						break;
					}
					case VK_DOWN:
					{//переместить выделенные точки (функции) вниз
						MoveSelectedPointsDown();
						MoveSelectedFunctionsDown();
						Invalidate();
						break;
					}

				}//конец switch по определению нажатой клавиши
			}
			break;
		}
	}//конец главного switch

	return CWnd::PreTranslateMessage(pMsg);
}

/****************   OnKeyDown   *********************************
*	Пользователь нажал на кнопку
****************************************************************/
void CGraph::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch(nChar)
	{
	case VK_DELETE:
		//удалю то, что выделил
		if(!(GetKeyState(VK_SHIFT)>>1))
		{//удаление не выделенного
			DeleteSelectedPoints();
			DeleteSelectedFunctions();
		}
		else
		{//удаление выделенного
			DeleteSelectedPoints(true);
			DeleteSelectedFunctions(true);
		}
		mas_SelectionPoints.clear();	//
		mas_SelectionFunctoins.clear();	//больше нет ничего выделенного
		break;
	}
}

/*****************    GetFocusRect   ******************************
*	Всегда выдает нормальные рект по двум токам пространства - не обязательно, чтобы 
*	первая являлась левым верхним, а вторая правым нижним углом
*		point_1, point_2 - эти точки
*******************************************************************/
CRect CGraph::GetFocusRect(CPoint point_1,CPoint point_2)
{
	if(point_1.x==-1 || point_2.x==-1)
	{//выделение еще не продвинулось ни на один пиксел, либо
	//вообще еще не началось
		return CRect();	//пустой четырёхугольник
	}

	int ly_top,ly_bottom,lx_left,lx_right;
	if(point_1.y<point_2.y)
	{
		ly_top=point_1.y;
		ly_bottom=point_2.y;
	}
	else
	{
		ly_top=point_2.y;
		ly_bottom=point_1.y;
	}
	if(point_1.x<point_2.x)
	{
		lx_left=point_1.x;
		lx_right=point_2.x;
	}
	else
	{
		lx_left=point_2.x;
		lx_right=point_1.x;
	}
	return CRect(lx_left,ly_top,lx_right,ly_bottom);
}

/*****************    IsPointSelected    **********************
*		Определяет, является ли данная точка данной фунцкии 
*	выделенной.
**************************************************************/
bool CGraph::IsPointSelected(size_t FuncNum, size_t Num)
{
	size_t lPointsSelected=mas_SelectionPoints.size();
	for(size_t i=0; i<lPointsSelected; i++)
	{
		if(mas_SelectionPoints[i].m_FuncNum!=FuncNum)
		{
			continue;
		}
		if(mas_SelectionPoints[i].m_PointNum!=Num)
		{
			continue;
		}
		//да, такая точка есть в нашем сундучке с выделенными точками
		return true;	
	}
	//среди просмотренных точек данной не оказалось
	return false;
}

/********************    IsFunctionSelected    *************************
*		определяет, является ли данная функция выделенной
***********************************************************************/
bool CGraph::IsFunctionSelected(size_t FuncNum)
{
	size_t lFunctionSelected=mas_SelectionFunctoins.size();
	for(size_t i=0; i<lFunctionSelected; i++)
	{
		if(mas_SelectionFunctoins[i].m_FuncNum!=FuncNum)
		{
			continue;
		}
		//да, эта функция есть в нашем сундучке с выделенными точками
		return true;	
	}
	//среди просмотренных точек данной не оказалось
	return false;
}

/************************    DeleteSelectedPoints      ****************
*	Удаляет (или оставляет) выделенные точки
*	Inverse == false - удаляет выделенной
*	Inverse == true - удаляет не выделенное, а выделенное оставляет
**********************************************************************/
struct IsFuncEqual: binary_function<CFuncPointSel,size_t, bool>
{
	bool operator ()(CFuncPointSel a, size_t FuncNum) const
	{
		return a.m_FuncNum==FuncNum;
	}
};
struct IsPointEqual: binary_function<CFuncPointSel,size_t, bool>
{
	bool operator ()(CFuncPointSel a, size_t PointNum) const
	{
		return a.m_PointNum==PointNum;
	}
};
void CGraph::DeleteSelectedPoints(bool Inverse)
{
	
	

	//sort(mas_SelectionPoints.begin(),mas_SelectionPoints.end());
	
	size_t lFuncCount=VecFunctions.size();
	for(size_t lFuncNum=0; lFuncNum<lFuncCount; lFuncNum++)
	{//цикл по всем функиям графика
		//vector<CFuncPointSel>::iterator end=remove_if(mas_SelectionPoints.begin(),mas_SelectionPoints.end(),/*IsFuncEqual()*/Predicate);
//		vector<CFuncPointSel>::iterator end=remove_if(mas_SelectionPoints.begin(),mas_SelectionPoints.end(),bind2nd(Predicate<size_t>,lFuncNum));
		vector<CFuncPointSel>::iterator end=remove_if(mas_SelectionPoints.begin(),mas_SelectionPoints.end(),not1(bind2nd(IsFuncEqual(),lFuncNum)));
		if(end!=mas_SelectionPoints.begin())
		{//у этой функции есть выделенные точки
			//vector<bool> lInfStore(VecFunctions[lFuncNum].pPlotXYItems.size(),Inverse);
			//size_t i=VecFunctions[lFuncNum].pPlotXYItems.size()-1;
			if(!Inverse)
			{
				vector<CFuncPointSel>::reverse_iterator Iter;
				for(Iter=mas_SelectionPoints.rbegin(); Iter<mas_SelectionPoints.rend();Iter++)
				{
					VECPTS::iterator p=VecFunctions[lFuncNum].pPlotXYItems.begin()+Iter->m_PointNum;
					VecFunctions[lFuncNum].pPlotXYItems.erase(p,p+1);
				}
			}
			else
			{
				size_t i=VecFunctions[lFuncNum].pPlotXYItems.size();
				do
				{
					i--;
					if(find_if(mas_SelectionPoints.begin(),end,bind2nd(IsPointEqual(),i))==mas_SelectionPoints.end())
					{
						VECPTS::iterator p=VecFunctions[lFuncNum].pPlotXYItems.begin()+i;
						VecFunctions[lFuncNum].pPlotXYItems.erase(p,p+1);
					}
				}while(i!=0);
			}
			
			/*for(size_t i=0; i<end-mas_SelectionPoints.begin(); i++)
			{
				lInfStore[mas_SelectionPoints[i].m_PointNum]=!Inverse;	//запомню точки, которые нужно удалить
			}
			size_t lCounter=0;
			VECPTS::iterator end2;
			end2 = remove_if(VecFunctions[lFuncNum].pPlotXYItems.begin(),
						VecFunctions[lFuncNum].pPlotXYItems.end(),
						bind2nd(IsPointRemove(),lInfStore[lCounter++]));
			VecFunctions[lFuncNum].pPlotXYItems.erase(end2,VecFunctions[lFuncNum].pPlotXYItems.end());
			*/
		}
	}
/*
	if(!Inverse)
	{//УДАЛИТЬ ВЫДЕЛЕННОЕ
		vector<CFuncPointSel>::iterator i;
		while(!mas_SelectionPoints.empty())
		{//как ни странно, удаление с конца здесь имеет очень важно значение, 
		//т.к. элементы не путаются
			i=mas_SelectionPoints.end()-1;
			size_t lPointNum=i->m_PointNum;
			size_t lFuncNum=i->m_FuncNum;
			VecFunctions[lFuncNum].pPlotXYItems.erase(VecFunctions[lFuncNum].pPlotXYItems.begin()+lPointNum);
			mas_SelectionPoints.pop_back();
		}
	}
	else
	{//УДАЛИТЬ НЕ ВЫДЕЛЕННОЕ
		//Это долгая операция, т.к. мне нужно просмотреть все точки
		//выделенных функций и оставить те, которые выделены
		size_t lFuncCount=VecFunctions.size();
		for(size_t i=0; i<lFuncCount; i++)
		{//по всем функциям графика

		}
	}*/
}

/************************    DeleteSelectedFunctions      ****************
*	Удаляет (или оставляет) выделенные функции
*	Inverse == false - удаляет выделенной
*	Inverse == true - удаляет не выделенное, а выделенное оставляет
**********************************************************************/
void CGraph::DeleteSelectedFunctions(bool Inverse)
{
	vector<CFuncFuncSel>::iterator i;
	while(!mas_SelectionFunctoins.empty())
	{//как ни странно, удаление с конца здесь имеет очень важно значение, 
	//т.к. элементы не путаются
		i=mas_SelectionFunctoins.end()-1;
		VecFunctions.erase(VecFunctions.begin()+i->m_FuncNum);
		mas_SelectionFunctoins.pop_back();
	}
}

/*************************    MoveSelectedPointsUp    *********************
*	Поднимает все выделенные точки вверх. При этом то, насколько они поднимутся
*	определяется 20й частью выдимой по вертикали области экрана.
**************************************************************************/
void CGraph::MoveSelectedPointsUp()
{
	FP32 lDif=(m_pGraphicOptions->var_yMax-m_pGraphicOptions->var_yMin)/40.f;
	vector<CFuncPointSel>::iterator Iter;
	for(Iter=mas_SelectionPoints.begin(); Iter!=mas_SelectionPoints.end(); Iter++)
	{
		VecFunctions[Iter->m_FuncNum].pPlotXYItems[Iter->m_PointNum].y+=lDif;
	}
}
void CGraph::MoveSelectedPointsDown()
{
	FP32 lDif=(m_pGraphicOptions->var_yMax-m_pGraphicOptions->var_yMin)/40.f;
	vector<CFuncPointSel>::iterator Iter;
	for(Iter=mas_SelectionPoints.begin(); Iter!=mas_SelectionPoints.end(); Iter++)
	{
		VecFunctions[Iter->m_FuncNum].pPlotXYItems[Iter->m_PointNum].y-=lDif;
	}
}
void CGraph::MoveSelectedPointsLeft()
{
	FP32 lDif=m_pGraphicOptions->var_VisibleTime/40.f;
	vector<CFuncPointSel>::iterator Iter;
	for(Iter=mas_SelectionPoints.begin(); Iter!=mas_SelectionPoints.end(); Iter++)
	{
		VecFunctions[Iter->m_FuncNum].pPlotXYItems[Iter->m_PointNum].x-=lDif;
	}
}
void CGraph::MoveSelectedPointsRight()
{
	FP32 lDif=m_pGraphicOptions->var_VisibleTime/40.f;
	vector<CFuncPointSel>::iterator Iter;
	for(Iter=mas_SelectionPoints.begin(); Iter!=mas_SelectionPoints.end(); Iter++)
	{
		VecFunctions[Iter->m_FuncNum].pPlotXYItems[Iter->m_PointNum].x+=lDif;
	}
}

/************************************************************************
*	ТОЖЕ САМОЕ, НО С ЦЕЛЫМИ ФУНКЦИЯМИ
*************************************************************************/
void CGraph::MoveSelectedFunctionsUp()
{
	FP32 lDif=(m_pGraphicOptions->var_yMax-m_pGraphicOptions->var_yMin)/40.f;
	vector<CFuncFuncSel>::iterator Iter;
	for(Iter=mas_SelectionFunctoins.begin(); Iter!=mas_SelectionFunctoins.end(); Iter++)
	{
		VECPTS::iterator Iter2;
		for(Iter2=VecFunctions[Iter->m_FuncNum].pPlotXYItems.begin();Iter2!=VecFunctions[Iter->m_FuncNum].pPlotXYItems.end(); Iter2++)
		{
			Iter2->y+=lDif;
		}
	}
}
void CGraph::MoveSelectedFunctionsDown()
{
	FP32 lDif=(m_pGraphicOptions->var_yMax-m_pGraphicOptions->var_yMin)/40.f;
	vector<CFuncFuncSel>::iterator Iter;
	for(Iter=mas_SelectionFunctoins.begin(); Iter!=mas_SelectionFunctoins.end(); Iter++)
	{
		VECPTS::iterator Iter2;
		for(Iter2=VecFunctions[Iter->m_FuncNum].pPlotXYItems.begin();Iter2!=VecFunctions[Iter->m_FuncNum].pPlotXYItems.end(); Iter2++)
		{
			Iter2->y-=lDif;
		}
	}
}
void CGraph::MoveSelectedFunctionsLeft()
{
	FP32 lDif=m_pGraphicOptions->var_VisibleTime/100.f;
	vector<CFuncFuncSel>::iterator Iter;
	for(Iter=mas_SelectionFunctoins.begin(); Iter!=mas_SelectionFunctoins.end(); Iter++)
	{
		VECPTS::iterator Iter2;
		for(Iter2=VecFunctions[Iter->m_FuncNum].pPlotXYItems.begin();Iter2!=VecFunctions[Iter->m_FuncNum].pPlotXYItems.end(); Iter2++)
		{
			Iter2->x-=lDif;
		}
	}
}
void CGraph::MoveSelectedFunctionsRight()
{
	FP32 lDif=m_pGraphicOptions->var_VisibleTime/100.f;
	vector<CFuncFuncSel>::iterator Iter;
	for(Iter=mas_SelectionFunctoins.begin(); Iter!=mas_SelectionFunctoins.end(); Iter++)
	{
		VECPTS::iterator Iter2;
		for(Iter2=VecFunctions[Iter->m_FuncNum].pPlotXYItems.begin();Iter2!=VecFunctions[Iter->m_FuncNum].pPlotXYItems.end(); Iter2++)
		{
			Iter2->x+=lDif;
		}
	}
}



/************************    FromFuncPtrToFuncNum    *********************
*	на входе указатель на функцию - на выходе ее номер в VecFunctions 
*	или INVALID_FUNC_NUM, если такой функции нету.
*************************************************************************/
INT16U CGraph::FromFuncPtrToFuncNum(CFunctionClass* ptr)
{
	size_t lFuncCount=VecFunctions.size();
	for(size_t i=0; i< lFuncCount; i++)
	{
		CFunctionClass *pFunc=&VecFunctions[i];
		if(pFunc==ptr)
		{//указатель совпал
			return (INT16S) i;
		}
	}
	return INVALID_FUNC_NUM;	//такой функции не оказалось
}
/*************************    FromFuncNameToFuncNum    *******************
*	на входе имя функции, на выходе ее номер в VecFunctions
*	или INVALID_FUNC_NUM, если такой функции нету.
*************************************************************************/
INT16U CGraph::FromFuncNameToFuncNum(const TCHAR* FuncName)
{
	size_t lFuncCount=VecFunctions.size();
	for(size_t i=0; i< lFuncCount; i++)
	{
		if(VecFunctions[i].szFunctionName==FuncName)
		{//имя совпало
			return (INT16S) i;
		}
	}
	return INVALID_FUNC_NUM;	//такой функции не оказалось
}

/*************************************************************************
*	Функции, собирающие информацию о функциях.
*	FromTime и ToTime - временные границы в функции, где ищутся требуемое значение
*************************************************************************/
FP32 CGraph::GetMaxGraphValue(FP32 FromTime, FP32 ToTime)
{
	if(VecFunctions.empty())
	{//чтобы хоть что-то отображалось
		return 100;
	}
	FP32 lResult=-FLT_MAX;
	vector<CFunctionClass>::iterator Iter;
	for(Iter=VecFunctions.begin(); Iter!=VecFunctions.end(); Iter++)
	{
		FP32 tmp=Iter->GetMaxValue(FromTime,ToTime);
		if(lResult<tmp) lResult=tmp;
	}
	return lResult;
}
FP32 CGraph::GetMinGraphValue(FP32 FromTime, FP32 ToTime)
{
	if(VecFunctions.empty())
	{//чтобы хоть что-то отображалось
		return 0;
	}
	FP32 lResult=FLT_MAX;
	vector<CFunctionClass>::iterator Iter;
	for(Iter=VecFunctions.begin(); Iter!=VecFunctions.end(); Iter++)
	{//цикл по всем функциям графика
		FP32 tmp=Iter->GetMinValue(FromTime,ToTime);
		if(lResult>tmp) lResult=tmp;
	}
	return lResult;
}
FP32 CGraph::GetMaxGraphScreenValue()
{//эта функция аналог GetMaxGraphValue - но только временными границами является 
//видимая область
	FP32 lMax=GetMaxGraphValue(m_pGraphicOptions->var_X_view, m_pGraphicOptions->var_X_view+m_pGraphicOptions->var_VisibleTime);
	if(lMax==-FLT_MAX)
	{//на графике ни одной видимой точки
		lMax=100;
	}
	return lMax;
}
FP32 CGraph::GetMinGraphScreenValue()
{//эта функция аналог GetMaxGraphValue - но только временными границами является 
//видимая область
	FP32 lMin=GetMinGraphValue(m_pGraphicOptions->var_X_view, m_pGraphicOptions->var_X_view+m_pGraphicOptions->var_VisibleTime);
	if (lMin==FLT_MAX)
	{//ни одной точки не видно
		lMin=0;
	}
	return lMin;
}
FP32 CGraph::GetMaxGraphTime()
{
	if(VecFunctions.empty())
	{
		return 100;
	}
	FP32 lResult=-10000000;
	vector<CFunctionClass>::iterator Iter;
	for(Iter=VecFunctions.begin(); Iter!=VecFunctions.end(); Iter++)
	{
		FP32 tmp=Iter->GetMaxTime();
		if(lResult<tmp) lResult=tmp;
	}
	return lResult;
}
FP32 CGraph::GetMinGraphTime()
{
	if(VecFunctions.empty())
	{
		return 0;
	}
	FP32 lResult=10000000;
	vector<CFunctionClass>::iterator Iter;
	for(Iter=VecFunctions.begin(); Iter!=VecFunctions.end(); Iter++)
	{
		FP32 tmp=Iter->GetMinTime();
		if(lResult>tmp) lResult=tmp;
	}
	return lResult;
}

/*******************************     PrintGraph     ***********************************
*		Функция распечатывает график, предварительно показывая пользователю диалог
*	распечатки.
***************************************************************************************/
bool CGraph::PrintGraph(void)
{
	CPrintDialog dlg(TRUE,PD_RETURNDC);

	/*Неудачная попытка изначально настроиться на альбомный лист
	DEVMODE *dm=new (DEVMODE);
	dm->dmSize=sizeof(DEVMODE);
	dm->dmFields = DM_ORIENTATION | DM_PAPERSIZE; 
	dm->dmOrientation = DMORIENT_LANDSCAPE; 
	dm->dmPaperSize = DMPAPER_LETTER; 
	dlg.m_pd.hDevMode=dm;*/

	if(dlg.DoModal() != IDOK)					//здесь нужно выборать альбомный лист
	{ 
		return true;
	}
	HDC hdcPrinter = dlg.m_pd.hDC;
	
	if (hdcPrinter == NULL)
	{
		MessageBox(_T("Buy a printer!"));
	}
	else
	{
		// create a CDC and attach it to the default printer
		CDC dcPrinter;
		dcPrinter.Attach(hdcPrinter);
		
		// call StartDoc() to begin printing
		DOCINFO docinfo;
		memset(&docinfo,0,sizeof(docinfo));
		docinfo.cbSize = sizeof(docinfo);
		docinfo.lpszDocName = _T("Распечатка графиков Sergeant Incorporation(c)");

		// if it fails, complain and exit gracefully
		if (dcPrinter.StartDoc(&docinfo) < 0)
		{
			MessageBox(_T("Printer wouldn't initalize"));
		}
		else
		{
			// start a page
			if (dcPrinter.StartPage() < 0)
			{
				MessageBox(_T("Could not start page"));
				dcPrinter.AbortDoc();
			}
			else
			{
				//задаю размеры листа и то, что на этот лист растягивать
				int lPrintPixWidth=GetDeviceCaps(hdcPrinter,HORZRES);
				int lPrintPixHeight=GetDeviceCaps(hdcPrinter,VERTRES);
				int lGraphPixWidth=m_pGraphicOptions->var_GraphWidth;
				int lGraphPixHeight=m_pGraphicOptions->var_GraphHeight;

				//обновим экран, чтобы мусор не попал на рисунок
				this->UpdateWindow();
				dcPrinter.StretchBlt(0,0,lPrintPixWidth,lPrintPixHeight,GetDC(),0,0,lGraphPixWidth,lGraphPixHeight,SRCCOPY);

				dcPrinter.EndPage();
				dcPrinter.EndDoc();
			}
		}
	}
	return true;
}