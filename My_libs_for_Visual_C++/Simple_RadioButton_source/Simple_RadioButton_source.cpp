#include "stdafx.h"
#include "Simple_RadioButton_source.h"

CSimple_RadioButton_source::CSimple_RadioButton_source()
{
	m_sName="No named";
	m_value=0x00;		//не выбран ни один радиобаттон
	ComeNewValue=NULL;
}

CSimple_RadioButton_source::~CSimple_RadioButton_source()
{
}

/**************************	SetAllValues(CString name,unsigned char value)	************
*	Эта функция одновременно задает имя группы радиобаттонов и текущее значение
*	Однако источники здесь не присваиваются, т.к. их точное число не определено
****************************************************************************************/
bool CSimple_RadioButton_source::SetAllValues(CString name,unsigned char value)
{
	m_sName=name;
	SetValue(value);
	return true;
}

/****************************	ReadValueFromWindow()	********************************
*		Эта функция считывает значения всех радиобаттонов, смотрит какой из них выбран
*	и в зависимости от этого проставляет значение m_value
*****************************************************************************************/
bool CSimple_RadioButton_source::ReadValueFromWindow()
{
	for(unsigned char i=0; i<m_sources.size();i++)
	{//будем последовательно проходить по всем источникам
		if(m_sources[i]!=NULL && m_sources[i]->m_hWnd!=NULL)
		{//это существующий источник
			if(m_sources[i]->GetCheck())
			{//ого, эта кнопка выбрана
				m_value=i+1;	//выставим ее код
				break;
			}
		}
		else
		{
			m_Error.SetERROR("Указатель на радиобаттон неверный в группе "+m_sName, 8);
			return false;
		}
	}
	if(ComeNewValue!=NULL)
	{//нужно уведомит об этом внешнюю функцию
		ComeNewValue();
	}
	return true;
}

/****************************	SetValue(unsigned char value)	**************************
*		Эта функция заносит очередное значение в m_value и одновременно вытавляет
*	нужный радиобаттон в окне
******************************************************************************************/
bool CSimple_RadioButton_source::SetValue(unsigned char value)
{
	if(value>m_sources.size())
	{
		m_Error.SetERROR("Невозможно подобрать такой радиобаттнон в группе "+m_sName, 8);
		return false;
	}
	
	//нужно снять выделение с теущего радиобаттона
	if(m_value!=0)
	{//если вообще что-то было выделено
		if(m_sources[m_value-1]!=NULL && m_sources[m_value-1]->m_hWnd!=NULL)
		{
			m_sources[m_value-1]->SetCheck(BST_UNCHECKED);
		}
		else
		{
			m_Error.SetERROR("Указатель на радиобаттон неверный в группе "+m_sName, 8);
			return false;
		}
	}

	//и, если нужно, выставить следующий
	if(value!=0)
	{
		if(m_sources[value-1]!=NULL && m_sources[value-1]->m_hWnd!=NULL)
		{
			m_sources[value-1]->SetCheck(BST_CHECKED);
		}
		else
		{
			m_Error.SetERROR("Указатель на радиобаттон неверный в группе "+m_sName, 8);
			return false;
		}
	}
	
	//изменим внутреннюю переменную
	m_value=value;

	return true;
}
/****************************************************************************************************
*	возвращает текущее значение m_value
***********************************************************************************************/
unsigned char	CSimple_RadioButton_source::GetValue()
{
	return m_value;
}