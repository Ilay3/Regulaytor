/*******************************************************************************
********************************************************************************
**		��� ����� ����������� ����, �� ������������ ��������� ������������� 
**	������� (� �����������) � �������� ��������� MSG_NEW_COMMAND �������������
**	����, ����� ��� ��������� ���������� �������.
********************************************************************************
*******************************************************************************/

/******************************************************************************
*						���� ����������� ���������
*	17.12.2007 - ������� ���������, ��� ������ ������������ � ��������������
*	19.12.2007 - ��� ������������ ��������� ������� ������� ��������������
*	20.11.2007 - ������� ��������� ���������� ������, �������� ����� ����������
*					������� � �� �������� ��������.
*	23.09.2008 - ��������� UNICODE
******************************************************************************/
#pragma once
#include <types_visual_c++.h>	//���� � ����� ������
#include <ERRORS\ERRORS.h>		//����� ����������� �� ������� � ����������
#include <fstream>				//��� ������ � �������
#include <map>					//��� ������ �� ���������
#include <AtlConv.h>			//��� ������ � UNICODE

using namespace std;

#define MSG_NEW_COMMAND (WM_USER+30)	//��� ��������� ������������, ����� ��������� ��������� ������
//�������� �� ���������� �����������
//	1. �������� ON_MESSAGE(MSG_NEW_COMMAND,InterpretMessage)
//	2. �������� LRESULT ������::InterpretMessage(WPARAM pCommand, LPARAM pArgString)
//	3. pCommand ��������� �� ������ � ��������� ���� TCHAR
//	4. ��������


//���������� ����� ���������� ����
class CCommandWindow: public CComboBox
{
	BOOL PreTranslateMessage(MSG *pMsg);
	
	ERROR_Class *m_pError;			//��� ������������ ������ � ������ ������� ����

	
	//������ ��������� ������ ������������� �������
	CString PreviousCommand;
public:

	//������������� ����������� ����
	bool InitCommandWindow(ERROR_Class *pError, CString ProgramDir);

	//� �����, ��� ������� �� ��������� ����������, � ������� ���������� ���������
	CString m_ProgramDir;	

	//��� ���������, ������� �������� ��� ���������� � ��������� ������ � �������
	map<CString,FP32,less<CString> > FP32_map;

	//������� ���������� � ��������� ������
	bool Create_FP32_var(CString var_name, FP32 var_value);
	
	//���� ����� ���������� ��� ����������, �� �� ������ ������
	bool ReCreate_FP32_var(CString var_name, FP32 var_value)
	{
		FP32_map[var_name]=var_value;
		return true;
	}
	
	//����������� ��������� � ��������� ������ �����-�� ��������
	bool Set_FP32_var(CString var_name, FP32 var_value);

	static bool GetBOOLArgument(CString *pstr, bool *ok=NULL);
	float GetFLOATArgument(CString *pstr, bool *ok=NULL);
	INT32S GetINTArgument(CString *pstr, bool *ok=NULL);
	INT32U GetUINTArgument(CString *pstr, bool *ok=NULL);
	static CString GetSTRINGArgument(CString *pstr, bool *ok=NULL);
	bool ReadMacros(CString filename);
public:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelendok();
public:
	afx_msg void OnCbnEditchange();

	//���������� ���������� ��������� ����������� ��������
	//� ���� ���� ���������� ��������
	bool PreTranslateCommand(CString AllString);
};