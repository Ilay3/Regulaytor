#include "stdafx.h"
#include "Simple_RadioButton_source.h"

CSimple_RadioButton_source::CSimple_RadioButton_source()
{
	m_sName="No named";
	m_value=0x00;		//�� ������ �� ���� �����������
	ComeNewValue=NULL;
}

CSimple_RadioButton_source::~CSimple_RadioButton_source()
{
}

/**************************	SetAllValues(CString name,unsigned char value)	************
*	��� ������� ������������ ������ ��� ������ ������������� � ������� ��������
*	������ ��������� ����� �� �������������, �.�. �� ������ ����� �� ����������
****************************************************************************************/
bool CSimple_RadioButton_source::SetAllValues(CString name,unsigned char value)
{
	m_sName=name;
	SetValue(value);
	return true;
}

/****************************	ReadValueFromWindow()	********************************
*		��� ������� ��������� �������� ���� �������������, ������� ����� �� ��� ������
*	� � ����������� �� ����� ����������� �������� m_value
*****************************************************************************************/
bool CSimple_RadioButton_source::ReadValueFromWindow()
{
	for(unsigned char i=0; i<m_sources.size();i++)
	{//����� ��������������� ��������� �� ���� ����������
		if(m_sources[i]!=NULL && m_sources[i]->m_hWnd!=NULL)
		{//��� ������������ ��������
			if(m_sources[i]->GetCheck())
			{//���, ��� ������ �������
				m_value=i+1;	//�������� �� ���
				break;
			}
		}
		else
		{
			m_Error.SetERROR("��������� �� ����������� �������� � ������ "+m_sName, 8);
			return false;
		}
	}
	if(ComeNewValue!=NULL)
	{//����� �������� �� ���� ������� �������
		ComeNewValue();
	}
	return true;
}

/****************************	SetValue(unsigned char value)	**************************
*		��� ������� ������� ��������� �������� � m_value � ������������ ���������
*	������ ����������� � ����
******************************************************************************************/
bool CSimple_RadioButton_source::SetValue(unsigned char value)
{
	if(value>m_sources.size())
	{
		m_Error.SetERROR("���������� ��������� ����� ������������ � ������ "+m_sName, 8);
		return false;
	}
	
	//����� ����� ��������� � ������� ������������
	if(m_value!=0)
	{//���� ������ ���-�� ���� ��������
		if(m_sources[m_value-1]!=NULL && m_sources[m_value-1]->m_hWnd!=NULL)
		{
			m_sources[m_value-1]->SetCheck(BST_UNCHECKED);
		}
		else
		{
			m_Error.SetERROR("��������� �� ����������� �������� � ������ "+m_sName, 8);
			return false;
		}
	}

	//�, ���� �����, ��������� ���������
	if(value!=0)
	{
		if(m_sources[value-1]!=NULL && m_sources[value-1]->m_hWnd!=NULL)
		{
			m_sources[value-1]->SetCheck(BST_CHECKED);
		}
		else
		{
			m_Error.SetERROR("��������� �� ����������� �������� � ������ "+m_sName, 8);
			return false;
		}
	}
	
	//������� ���������� ����������
	m_value=value;

	return true;
}
/****************************************************************************************************
*	���������� ������� �������� m_value
***********************************************************************************************/
unsigned char	CSimple_RadioButton_source::GetValue()
{
	return m_value;
}