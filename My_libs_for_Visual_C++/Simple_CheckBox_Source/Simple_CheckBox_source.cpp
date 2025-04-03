#include "stdafx.h"
#include "Simple_CheckBox_source.h"

/********************** ����������� ************************************************
*		�������� ���������� ����������
************************************************************************************/
CSimple_CheckBox_source::CSimple_CheckBox_source()
{
	m_source=NULL;		//�� ��������� �������������
	m_value=false;			//��������� �������� 
	m_sName="No name";
	m_bReadOnly=false;	//������ �� ���������, ����� ������, ��� ������
	ComeNewValue=NULL;	//������� ��������� �� ������� �������
}

/*****************************  Set_value	(bool dir, bool temp)  **********************
*		���������� ����� ��������
*		// dir = true - �������� �������� � ���� ���������
*		// dir = false - ���-�� ��� � � ���� ����� ��������
*		//���������� 
*		// true - ������� ���������� �������
*		// false - ��� ��������� ���� ������, ��� �������� � ERRORS
********************************************************************************************/
bool CSimple_CheckBox_source::Set_value	(bool value)
{
	// dir = true - �������� �������� � ���� ���������
	// dir = false - ���-�� ��� � � ���� ����� ��������
	
	if(m_source==NULL || m_source->m_hWnd==NULL)
	{
		m_Error.SetERROR("���������� ������������ �������� \""+m_sName+ "\"�� �������� �� ����������",10);
		return false;
	}

	//������� � � ���� � � ����
	m_value=value;
	m_source->SetCheck(value);
	
	if(ComeNewValue!=NULL)
	{// ����� ��� � ���������� ������� �������
		ComeNewValue();
	}
	return true;
}

/**************************	ReadValueFromWindow(void) *******************************
*	��������� �������� �� ����, ��� ����� �������� m_Value
*************************************************************************************/
bool CSimple_CheckBox_source::ReadValueFromWindow (void)	
{
	if(!m_bReadOnly)
	{//� ���� ������������� ����� ���� ���-�� ��������
		if(m_source==NULL || m_source->m_hWnd==NULL)
		{
			m_Error.SetERROR("���������� ������������ �������� \""+m_sName+ "\"�� �������� �� ����������",10);
			return false;
		}
		m_value=static_cast<bool>(m_source->GetCheck());
	}
	else
	{
		m_Error.SetERROR("�������� "+m_sName+" ������ ��� ������", 2);
		return false;
	}

	if(ComeNewValue!=NULL)
	{// ����� ��� � ���������� ������� �������
		ComeNewValue();
	}
	return true;
}

/*******************************	Get_value()	****************************************
**		���������� �������� ���������� ����������
****************************************************************************************/
bool CSimple_CheckBox_source::Get_value()
{
	return m_value;
}


/*********************************  SetAllValues  ******************************************************
*		��������� ����� ����� ��� �����
**********************************************************************************************************/

bool	CSimple_CheckBox_source::SetAllValues(CButton* source, CString name,bool value)
{
	m_source=source;
	m_sName=name;
	source->SetWindowText(name);
	//��� ������ ���������� � ����� (����� ���������� ���������)
	this->Set_value(value);
	
	return true;
}

/*********************************	SetVisualisation (unsigned char height);	**************************
*		� ������������ ������� ��� ������ �������
*			height - ������ ������
******************************************************************************************************/
bool	CSimple_CheckBox_source::SetVisualisation (bool read_onli, bool isInt, bool PresCount, unsigned char height)
{
	if(m_source==NULL || m_source->m_hWnd==NULL)
	{
		m_Error.SetERROR("���������� ��������� ������� ��� \""+m_sName+ "\"�� �������� �� ����������",10);
		return false;
	}
	if(read_onli)
	{
		Set_ReadOnly(true);
	}
	return true;
}

/****************	Set_ReadOnly	(bool read_only)	*************
*	���������� ��� ������� ���� "������ ������"
*********************************************************************/
bool	CSimple_CheckBox_source::Set_ReadOnly	(bool read_only)
{
	m_bReadOnly=read_only;
	return true;
}