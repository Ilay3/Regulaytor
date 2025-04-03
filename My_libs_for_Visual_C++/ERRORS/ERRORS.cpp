#include "stdafx.h"
#include "ERRORS.h"

/*********************  �����������  ****************************************************
***************************************************************************************/
ERROR_Class::ERROR_Class()
{
	ErrorServ=NULL;			//������� ��������� �� ������� ���������� �������
	m_bNew=false;
	m_iPriority=0;
	m_sLastError="No error";
	m_pParent=NULL;
}

bool ERROR_Class::InitERROR(void (*error_serv)(ERROR_Class *error),CWnd* pParent)
{
	this->ErrorServ=error_serv;		//�������� ��������� �� �������������� �������
	m_pParent=pParent;				//�������� ��������� �� ��������, ����� � ������ ����
									//���������� ��� ���������
	return true;
}

/****************** SetERROR(CString error, unsigned char priority) ************************
*		���������� ������ � ������ ���������.
*	error - ����� ���������
*	priority - ��������� ��������� (������ �� ������ � ����)
*	Title - ��������� �� ������ ��������� ���������. ��������� � LogFile �� �������
********************************************************************************************/
void ERROR_Class::SetERROR(CString error, unsigned char priority, TCHAR* Title)	
{
	m_iPriority=priority;	//��������� ������
	m_sLastError=error;		//����� ������
	if(Title!=NULL)
		m_sLastTitle=Title;	//��������� ���������
	else 
		m_sLastTitle=_T("");//��� ���������

	m_bNew=true;			//����, ���������, ��� ��� ����� ������
	if(ErrorServ!=NULL)
	{//��� ����� ������� ������� ������
		ErrorServ(this);		//������� ������� ������, ������� � ������� ������������ ������
	}

	if(m_iPriority & ER_SHOW_MESSAGE)
	{//����� ���� ����������� ���������
		UINT lFlags=0;
		priority&=0x0F;				//������� ������� ������	
		TCHAR lTitle[100];			//��������� ���� ���������

		if(Title!=NULL)
			_tcscpy_s(lTitle,_countof(lTitle),Title);	//��������� ��� ������

		if(priority>=ERP_FATALERROR)
		{
			lFlags|=MB_TOPMOST | MB_ICONERROR;
			if(Title==NULL)
				_tcscpy_s(lTitle,_T("��������� ������\0"));
		}
		if(priority>=ERP_ERROR)
		{//���� ����������� ������ �������
			lFlags|=MB_ICONERROR;
			if(Title==NULL)
				_tcscpy_s(lTitle,_T("������\0"));
		}
		else if(priority>=ERP_WARNING)
		{
			lFlags|=MB_ICONWARNING;
			if(Title==NULL)
				_tcscpy_s(lTitle,_T("��������������\0"));
		}
		else if(priority>=ERP_WATCHFUL)
		{
			lFlags|=MB_ICONWARNING;
			if(Title==NULL)
				_tcscpy_s(lTitle,_T("�������� ��������\0"));
		}
		else if(priority>=ERP_NORMAL)
		{
			lFlags|=MB_ICONINFORMATION;
			if(Title==NULL)
				_tcscpy_s(lTitle,_T("���������\0"));
		}
		else if(priority>=ERP_GOOD)
		{
			lFlags|=MB_ICONINFORMATION;
			if(Title==NULL)
				_tcscpy_s(lTitle,_T("�������� ���������\0"));
		}
		else if(priority>=ERP_VERYGOOD)
		{
			lFlags|=MB_ICONINFORMATION;
			if(Title==NULL)
				_tcscpy_s(lTitle,_T("������� ������������\0"));
		}
		//������ ���������
		if(m_pParent!=NULL)
			MessageBox(m_pParent->m_hWnd,error,lTitle,lFlags);
		else
			MessageBox(NULL,error,lTitle,lFlags);
	}

	if(m_pParent!=NULL && IsWindow(m_pParent->m_hWnd))
	{//����� ��������� ��������� ��������������� ����
		m_pParent->SendMessage(MSG_ER_ERROR,0, reinterpret_cast<LPARAM>(this));
	}
}

/****************** GetLastError() **********************************************************
*		������ ����� ��������� ����������� ������, ��� ���� ��������� ����
*		���������������� ���� ������
********************************************************************************************/
CString ERROR_Class::GetLastError()			
{
	m_bNew=false;
	return m_sLastError;
}