#include "stdafx.h"
#include "HistoryFile.h"




/******************************* Close (void) *******************************************
*		��������� ����. ��������� �������, ����� ��������� � ������������������ ��������� 
*	����������.
******************************************************************************************/
bool HISTORY_FILE_Class::Close(void)
{
	if(File.is_open())
	{
		this->File.close();
	}
	return true;
}

/******************************* Open(void) *********************************************
*		��������� ���� ������� �� � ������ ������ � ��� ������������ ������ 
*	� ��� ������������� ������
*****************************************************************************************/
bool HISTORY_FILE_Class::Open(void)
{
	if(File.is_open()) File.close();//�� ������ ������
	/*
	�������� ����� ����� ��������� ����,����� � ��� � ������� �������� ���������,
	� ������, ���� ���� � ����� ������ ��� ���������, �������� ���������� � �����
	*/
	
	CStringA str;
	char t[5];
	SYSTEMTIME time;
	GetLocalTime(&time);
	str.Format("%i",time.wDay);
	
	switch(time.wMonth)
	{
	case 1:		str=str+" January "; break;
	case 2:		str=str+" February "; break;
	case 3:		str=str+" March "; break;
	case 4:		str=str+" April "; break;
	case 5:		str=str+" May "; break;
	case 6:		str=str+" June "; break;
	case 7:		str=str+" July "; break;
	case 8:		str=str+" August "; break;
	case 9:		str=str+" September "; break;
	case 10:	str=str+" October "; break;
	case 11:	str=str+" November "; break;
	case 12:	str=str+" December "; break;
	default:	AfxMessageBox(_T("�������� ���������� ������� � ����� �������"));
				return false;//�� ���� ����������
	}
	
	_itoa_s(time.wYear,t,5,10);
	str=str+t;
	str=str+".txt";
	
	CStringA dir="History Data";
	CreateDirectory(dir,NULL);
	File.open(dir+"\\"+str,/*ios::trunc ������� ������������*/ ios::app /*���������� � �����*/ | ios::out);
	
	if(!File)
	{
		m_Error.SetERROR(_T("�� �������� ���� ������� CAN ���������")+str,ERP_ERROR | ER_WRITE_TO_LOG);		
		return false;
	}
	else
	{
		CStringA str2="���� ������� ���������� c��������...";
		File<<endl<<endl;
		File.write(str2,str2.GetLength());
		File<<endl<<endl;
		m_Error.SetERROR(_T("���� ������� ���������� c��������   ")+str+_T(" ������� ������"),ERP_NORMAL | ER_WRITE_TO_LOG);
	}
	return true;
}

/*********************** Write(unsigned int count, ...)*****************
*	���������� � ���� ������� ��. ��� ������� � ���������� ����������� ����������.
*	��������� ����� ��� float.
*	count - ���������� ����������
**************************************************************************/
bool HISTORY_FILE_Class::Write(float * mas,unsigned char count)
{
	/*
	��� ������ ����� ���������� ��� �� ����������� �������, ��� � ����� ���������� ��������� ������ (��� �������)
	�� ��������� � ������� ������ � ������ ���������� � ���� ����
	*/
	/*
	������ ��� ������ ���� ������� ��� �������� ����������� ���-�� ������������
	���������� ������� (�� �� �� ������������������ ��� ���).
	*/
	if(count==0)
	{//��� ����, ����� ��� ������ ������� ����� ���� �� ������������ � ���������� ���������
		count=this->count_of_items;
	}

	if(File.is_open())
	{
		CString str;
		char t[15];
		
		File.precision(3);		//���������� ������ ����� �������
		File.setf(File.fixed);	//���������� ��� �������� �������

		//������� ������� �����
		SYSTEMTIME time;
		GetLocalTime(&time);
		
		_itoa_s(time.wHour,t,5,10);
		str=t;
		str+="h:";//����� ���� ��������
		
		_itoa_s(time.wMinute,t,5,10);
		str+=t;
		str+="min:";//����� ���� ��������

		_itoa_s(time.wSecond,t,5,10);
		str+=t;
		str+="s:";//����� ���� ��������

		_itoa_s(time.wMilliseconds,t,5,10);
		str+=t;
		str+="ms";//����� ���� ��������
		
		File.width(27);			//����� � ����� ��� ���� ���������
		File<<str.GetBuffer();	//����� �����

		for(unsigned char i=0; i<count; i++)
		{
			File.setf(File.left);	//� � ����� �������������
			File.width(25);
			File<<Names[i].GetBuffer();
			File.width(19);
			File.setf(File.right);	//� � ����� �������������
			File<<*mas++; 
		}
		File<<endl;
	}

	return true;
}

HISTORY_FILE_Class::~HISTORY_FILE_Class()
{
	this->Close();	//�� ������ ������
}

HISTORY_FILE_Class::HISTORY_FILE_Class()
{
	unsigned char i=0;
	#ifdef VAR_1
	Names[i++]=VAR_1_NAME;
	#endif

	#ifdef VAR_2
	Names[i++]=VAR_2_NAME;
	#endif

	#ifdef VAR_3
	Names[i++]=VAR_3_NAME;
	#endif

	#ifdef VAR_4
	Names[i++]=VAR_4_NAME;
	#endif

	#ifdef VAR_5
	Names[i++]=VAR_5_NAME;
	#endif

	#ifdef VAR_6
	Names[i++]=VAR_6_NAME;
	#endif

	#ifdef VAR_7
	Names[i++]=VAR_7_NAME;
	#endif

	#ifdef VAR_8
	Names[i++]=VAR_8_NAME;
	#endif

	#ifdef VAR_9
	Names[i++]=VAR_9_NAME;
	#endif

	#ifdef VAR_10
	Names[i++]=VAR_10_NAME;
	#endif

	//������� ����� �������
	this->count_of_items=i;
}