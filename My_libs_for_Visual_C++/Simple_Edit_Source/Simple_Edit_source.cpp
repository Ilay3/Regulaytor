#include "stdafx.h"
#include "Simple_Edit_source.h"

/********************** ����������� ************************************************
*		�������� ���������� ����������
************************************************************************************/
CSimple_Edit_source::CSimple_Edit_source()
{
	m_b_isInt=false;	//����� ��� ������������
	m_PresCount=3;		//���������� ������ ����� �������
	m_source=NULL;		//�� ��������� �������������
	m_value=-1;			//��������� �������� 
	m_sName="No name";
	m_smallest=(float)-10e12;	//���������� ���������� ��������
	m_largest=(float)10e12;	//���������� ���������� ��������
}

/*****************************  Set_value	(bool dir, double temp)  **********************
*		���������� ����� ��������
*		// dir = true - �������� �������� � ���� ���������
*		// dir = false - ���-�� ��� � � ���� ����� ��������
*		//���������� 
*		// true - ������� ���������� �������
*		// false - ��� ��������� ���� ������, ��� �������� � ERRORS
********************************************************************************************/
bool CSimple_Edit_source::Set_value		(bool dir, double temp)
{
	// dir = true - �������� �������� � ���� ���������
	// dir = false - ���-�� ��� � � ���� ����� ��������
	if(m_source==NULL || m_source->m_hWnd==NULL)
	{
		m_Error.SetERROR("���������� ��������� �������� \""+m_sName+ "\"�� �������� �� ����������",10);
		return false;
	}

	CString str;
	if(dir==true)
	{//���� �������� �������� � ���� ���������, � ������� ��� ��������� �� ����
		m_source->GetWindowText(str);//������� � ������
		temp=(float)atof(str.GetBuffer());//������ � ������������
		if(temp>=m_smallest && temp<=m_largest)
		{//���������� ��������
			m_value=temp;
		}
		else
		{//���� ������, �� �� ���������
			m_Error.SetERROR("������������ �������� ���� �������� ����� 1, ����� �� ����������",5);
		}
	}
	else
	{//����� ��� � �������� � ����
		m_value=temp;
		char t[10];
		if(m_b_isInt)
		{
			str.Format("%d",(int)temp);
		}
		else
		{	//t=_itoa(
			//str.Format("%.2f",temp);
			_gcvt_s(t,10,temp,m_PresCount);//������������ ������������ � ������ (m_PresCount ����� ����� �������)
		}
		//str.Format("%s",t);	str.TrimRight('.');
		m_source->SetWindowText(str);//������� �� ������
	}
	
	return !m_Error.m_bNew;
}

/*******************************	Get_value()	****************************************
**		���������� �������� ���������� ����������
****************************************************************************************/
double CSimple_Edit_source::Get_value()
{
	return m_value;
}


/*********************************  SetAllValues  ******************************************************
*		��������� ����� ����� ��� �����
**********************************************************************************************************/

bool	CSimple_Edit_source::SetAllValues(CEdit* source, CString name,float value, float smallest, float largest)
{
	m_source=source;
	m_sName=name;
	m_smallest=smallest;
	m_largest=largest;
	//��� ������ ���������� � �����
	this->Set_value(false,value);
	
	return true;
}

/*********************************	SetVisualisation (unsigned char height);	**************************
*		� ������������ ������� ��� ������ �������
*			height - ������ ������
******************************************************************************************************/
//��� �������
bool	CSimple_Edit_source::SetVisualisation (bool read_onli, bool isInt, bool PresCount, unsigned char height)
{
	if(m_source==NULL || m_source->m_hWnd==NULL)
	{
		m_Error.SetERROR("���������� ��������� ������� ��� \""+m_sName+ "\"�� �������� �� ����������",10);
		return false;
	}

	m_source->SetReadOnly(true);

	m_b_isInt=isInt;
	m_PresCount=PresCount;
	//����� ������ ��� ����� �����
	m_Font.DeleteObject();  //�� ������ ������
	m_Font.CreateFont		/*�������� ����� ��������*/
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