#include "stdafx.h"
#include "Simple_Edit_source.h"

/********************** КОНСТРУКТОР ************************************************
*		Задаемся начальными значениями
************************************************************************************/
CSimple_Edit_source::CSimple_Edit_source()
{
	m_b_isInt=false;	//целое или вещественное
	m_PresCount=3;		//количество знаков после запятой
	m_source=NULL;		//во избежании недоразумений
	m_value=-1;			//начальное значение 
	m_sName="No name";
	m_smallest=(float)-10e12;	//наименьшее допустимое значение
	m_largest=(float)10e12;	//наибольшее допустимое значение
}

/*****************************  Set_value	(bool dir, double temp)  **********************
*		Выставляет новое значение
*		// dir = true - поменяли значение в окне программы
*		// dir = false - где-то еще и в окне нужно обновить
*		//Возвращает 
*		// true - функция отработала успешно
*		// false - при отработке были ошибки, они записаны в ERRORS
********************************************************************************************/
bool CSimple_Edit_source::Set_value		(bool dir, double temp)
{
	// dir = true - поменяли значение в окне программы
	// dir = false - где-то еще и в окне нужно обновить
	if(m_source==NULL || m_source->m_hWnd==NULL)
	{
		m_Error.SetERROR("Попытались заполнить значение \""+m_sName+ "\"но источник не существует",10);
		return false;
	}

	CString str;
	if(dir==true)
	{//было изменено значение в окне программы, и поэтому его обновлять не надо
		m_source->GetWindowText(str);//считаем с экрана
		temp=(float)atof(str.GetBuffer());//строка в вещественное
		if(temp>=m_smallest && temp<=m_largest)
		{//корректное значение
			m_value=temp;
		}
		else
		{//была ошибка, но не тотальная
			m_Error.SetERROR("Некорректное значение поля скорости Режим 1, число не обработано",5);
		}
	}
	else
	{//нужно еще и обновить в окне
		m_value=temp;
		char t[10];
		if(m_b_isInt)
		{
			str.Format("%d",(int)temp);
		}
		else
		{	//t=_itoa(
			//str.Format("%.2f",temp);
			_gcvt_s(t,10,temp,m_PresCount);//конвертируем вещественное в строку (m_PresCount знака после запятой)
		}
		//str.Format("%s",t);	str.TrimRight('.');
		m_source->SetWindowText(str);//покажем на экране
	}
	
	return !m_Error.m_bNew;
}

/*******************************	Get_value()	****************************************
**		Возвращает значение внутренней переменной
****************************************************************************************/
double CSimple_Edit_source::Get_value()
{
	return m_value;
}


/*********************************  SetAllValues  ******************************************************
*		выставить одним махом все опции
**********************************************************************************************************/

bool	CSimple_Edit_source::SetAllValues(CEdit* source, CString name,float value, float smallest, float largest)
{
	m_source=source;
	m_sName=name;
	m_smallest=smallest;
	m_largest=largest;
	//это должно находиться в конце
	this->Set_value(false,value);
	
	return true;
}

/*********************************	SetVisualisation (unsigned char height);	**************************
*		У станавливает внешний вид нашего Эдитика
*			height - высота текста
******************************************************************************************************/
//Для красоты
bool	CSimple_Edit_source::SetVisualisation (bool read_onli, bool isInt, bool PresCount, unsigned char height)
{
	if(m_source==NULL || m_source->m_hWnd==NULL)
	{
		m_Error.SetERROR("Попытались выставить внешний вид \""+m_sName+ "\"но источник не существует",10);
		return false;
	}

	m_source->SetReadOnly(true);

	m_b_isInt=isInt;
	m_PresCount=PresCount;
	//какой высоты нам нужен текст
	m_Font.DeleteObject();  //на всякий случай
	m_Font.CreateFont		/*Выставим шрифт побольше*/
		(
			-height,                   // nHeight
			0,                         // nWidth
			0,                         // nEscapement
			0,                         // nOrientation
			FW_NORMAL,                 // nWeight
			FALSE,                     // bItalic
			FALSE,                     // bUnderline
			0,                         // cStrikeOut
			DEFAULT_CHARSET,              // nCharSet
			OUT_DEFAULT_PRECIS,        // nOutPrecision
			CLIP_DEFAULT_PRECIS,       // nClipPrecision
			DEFAULT_QUALITY,           // nQuality
			DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
			"Arial");                 // lpszFacename

	m_source->SetFont(&m_Font,true);
	return true;
}