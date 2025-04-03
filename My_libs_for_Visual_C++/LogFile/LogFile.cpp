#include "stdafx.h"
#include "LogFile.h"
#include <AtlConv.h>		//��� �������������� unicode<->ANSI



LOG_FILE_Class::LOG_FILE_Class()
{
	this->m_sFileName=_T("LogFile");
	m_pError=NULL;
}

/**************************************************************
*		�������������
***************************************************************/
bool LOG_FILE_Class::InitLogFile(const TCHAR* file_name, ERROR_Class *pError, CString name_of_program, bool UseUnicode)
{
	// file_name - ��� ����� ������� ���������
	// pError - ��������� �� ���������, ������������� ������
	// name_of_program - �������������� ��� ���������, ����� ��� ���������

	m_sFileName=file_name;
	m_pError=pError;
	m_UseUnicodeFormat=UseUnicode;
	//������� ����
	if(!Open(name_of_program))
	{
		return false;
	}
	return true;
}


/***********************	Show(void)	*****************************************
*	������ ��������� ���� ���� � ����������
*********************************************************************************/
bool LOG_FILE_Class::Show(void)
{
	CString str;
	str.Format(_TEXT("Notepad.exe %s"),m_sFileName);
	
	STARTUPINFO si={sizeof(si)};
	PROCESS_INFORMATION pi;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );
	if( !CreateProcess( NULL, (LPTSTR)str.GetBuffer(), NULL, NULL, FALSE,
		0, NULL, NULL, &si, &pi))
	{
		m_pError->SetERROR(_TEXT("���������� ��������� ��������� ��� ������ ������� ���������."), ERP_ERROR | ER_WRITE_TO_LOG | ER_SHOW_MESSAGE);
		return false;
	}
	//��� �� ����� ��������� ��������� �������� � ������
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );

	return true;
}

bool LOG_FILE_Class::Open(CString name_of_program)
{
	/************************************************************
	*	���� ������������ �������� ������� ���������� ���������, �� 
	*	�� ��� ����� �� ������ ������� ���� ����������� ������ ��
	*	����� �����, ������� ��������� ����� ������ ����. ���� ��������
	*	� ANSI, � �� ���� � ����� ����� ������� �����, �� ����
	*	�� ���������, ���� �� ����� ������� ���������!
	*************************************************************/
	if(m_sFileName[2]!=':')
	{//��������� �� ��������� ���� �������� ������ ���
		//������� ������ ����
		TCHAR dir[255];
		GetCurrentDirectory(244,dir);
		m_sFileName=dir+('\\'+m_sFileName);
	}
	if(LogFile.is_open()) 
		LogFile.close();//�� ������ ������
	
	LogFile.open(m_sFileName,ios::trunc | ios::out | ios::binary);
	if(!LogFile)
	{
		DWORD lErr=GetLastError();
		CString str; str.Format(_TEXT("�� �������� ���� \"%s\", �������� ���� ������� �� ������ ��� ����������. GetLastError()=%u"),m_sFileName,lErr);
		if(m_pError!=NULL)
		{
			m_pError->SetERROR(str,ERP_ERROR | ER_SHOW_MESSAGE);		
		}
		else
		{//���� ��� ������ ������, �� ���� ������� ���������
			AfxMessageBox(str);
		}
		return false;
	}
	else
	{
		CString str=_TEXT("������ ����������� ���������...");
		if (name_of_program!=_TEXT(""))
		{//��������� ��� ������� ��������������� ���������� ��� �����
			str.TrimRight('.');
			str.Append(_TEXT("  \""));
			str.Append(name_of_program);
			str.Append(_TEXT("\"."));
		}
		if(m_UseUnicodeFormat)
		{//����� ������ � unicode
			str.Append(_T("\r\n\r\n"));	//|
			unsigned short t=0xFEFF;	//|
			LogFile.write((char*)&t,2);	//| ��������, ���������, ��� ���� unicode
			LogFile.write((char*)str.GetBuffer(),str.GetLength()*sizeof(TCHAR));
		}
		else
		{//����� ������ � ansi
			USES_CONVERSION;
			LogFile.write(T2A(str.GetBuffer()),str.GetLength());
		}
	}
	return true;
}

/*************************	Write(*)	***********************************
*		��� ������� ������ ��������� ������ � ����
***************************************************************************/
bool LOG_FILE_Class::Write(ERROR_Class* perror)
{
	if(!LogFile.is_open()) return false;		//���� ������ �� ������
	CString str;
		
	//������� ������� �����
	SYSTEMTIME time;
	GetLocalTime(&time);
	str.Format(_TEXT("\r\n%ih:%imin : %is : %ims      %s"),time.wHour,time.wMinute,time.wSecond,time.wMilliseconds,perror->GetLastError());

	if(m_UseUnicodeFormat)
	{//����� ������ � unicode
		LogFile.write((char*)str.GetBuffer(),str.GetLength()*sizeof(TCHAR)); //������� ������
	}
	else
	{//� ������� ANSI
		USES_CONVERSION;
		LogFile.write(T2A(str.GetBuffer()),str.GetLength()); //������� ������
	}
/*	if(perror->m_iPriority & ~ER_WRITE_TO_LOG)
	{//��������� ���������
		str.Format("    ���������:   %u ",perror->m_iPriority & ~ER_WRITE_TO_LOG);
		LogFile.write(str,str.GetLength());  //� �� ��������
	}*/
	LogFile.flush();		//����� �� ������ Windows � ���������� ���� �� �����

	return true;
}

/*************************	Write	***************************************
*		��� ������� ������ ��������� ������ � ����
***************************************************************************/
bool LOG_FILE_Class::Write(CString error, unsigned char priority)
{
	if(!LogFile.is_open()) return false;

/*	CString str=error;
	LogFile.write("     ",5);
	USES_CONVERSION;
	LogFile.write(T2A(str),str.GetLength()); //������� ������
	if(priority & ~ER_WRITE_TO_LOG)
	{//��������� ���������
		str.Format(_TEXT("    ���������:   %u "),priority & ~ER_WRITE_TO_LOG);
		LogFile.write(str,str.GetLength());  //� �� ��������
	}
	LogFile<<endl;*/
	return true;
}