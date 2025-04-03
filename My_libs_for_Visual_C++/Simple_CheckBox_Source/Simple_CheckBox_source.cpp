#include "stdafx.h"
#include "Simple_CheckBox_source.h"

/********************** КОНСТРУКТОР ************************************************
*		Задаемся начальными значениями
************************************************************************************/
CSimple_CheckBox_source::CSimple_CheckBox_source()
{
	m_source=NULL;		//во избежании недоразумений
	m_value=false;			//начальное значение 
	m_sName="No name";
	m_bReadOnly=false;	//пускай по умолчанию, можно делать, что угодно
	ComeNewValue=NULL;	//обнулим указатель на внешнюю функцию
}

/*****************************  Set_value	(bool dir, bool temp)  **********************
*		Выставляет новое значение
*		// dir = true - поменяли значение в окне программы
*		// dir = false - где-то еще и в окне нужно обновить
*		//Возвращает 
*		// true - функция отработала успешно
*		// false - при отработке были ошибки, они записаны в ERRORS
********************************************************************************************/
bool CSimple_CheckBox_source::Set_value	(bool value)
{
	// dir = true - поменяли значение в окне программы
	// dir = false - где-то еще и в окне нужно обновить
	
	if(m_source==NULL || m_source->m_hWnd==NULL)
	{
		m_Error.SetERROR("Попытались использовать значение \""+m_sName+ "\"но источник не существует",10);
		return false;
	}

	//обновим и в окне и в теле
	m_value=value;
	m_source->SetCheck(value);
	
	if(ComeNewValue!=NULL)
	{// нужно еще и оповестить внешнюю функцию
		ComeNewValue();
	}
	return true;
}

/**************************	ReadValueFromWindow(void) *******************************
*	Считывает значение из окна, тем самым обновляя m_Value
*************************************************************************************/
bool CSimple_CheckBox_source::ReadValueFromWindow (void)	
{
	if(!m_bReadOnly)
	{//в окне действительно можно было что-то поменять
		if(m_source==NULL || m_source->m_hWnd==NULL)
		{
			m_Error.SetERROR("Попытались использовать значение \""+m_sName+ "\"но источник не существует",10);
			return false;
		}
		m_value=static_cast<bool>(m_source->GetCheck());
	}
	else
	{
		m_Error.SetERROR("Значение "+m_sName+" только для чтения", 2);
		return false;
	}

	if(ComeNewValue!=NULL)
	{// нужно еще и оповестить внешнюю функцию
		ComeNewValue();
	}
	return true;
}

/*******************************	Get_value()	****************************************
**		Возвращает значение внутренней переменной
****************************************************************************************/
bool CSimple_CheckBox_source::Get_value()
{
	return m_value;
}


/*********************************  SetAllValues  ******************************************************
*		выставить одним махом все опции
**********************************************************************************************************/

bool	CSimple_CheckBox_source::SetAllValues(CButton* source, CString name,bool value)
{
	m_source=source;
	m_sName=name;
	source->SetWindowText(name);
	//это должно находиться в конце (после присвоения источника)
	this->Set_value(value);
	
	return true;
}

/*********************************	SetVisualisation (unsigned char height);	**************************
*		У станавливает внешний вид нашего Эдитика
*			height - высота текста
******************************************************************************************************/
bool	CSimple_CheckBox_source::SetVisualisation (bool read_onli, bool isInt, bool PresCount, unsigned char height)
{
	if(m_source==NULL || m_source->m_hWnd==NULL)
	{
		m_Error.SetERROR("Попытались выставить внешний вид \""+m_sName+ "\"но источник не существует",10);
		return false;
	}
	if(read_onli)
	{
		Set_ReadOnly(true);
	}
	return true;
}

/****************	Set_ReadOnly	(bool read_only)	*************
*	выставляет или снимает флаг "только чтение"
*********************************************************************/
bool	CSimple_CheckBox_source::Set_ReadOnly	(bool read_only)
{
	m_bReadOnly=read_only;
	return true;
}