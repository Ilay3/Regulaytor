#include "stdafx.h"
#include "FunctionClass.h"
#include <AtlConv.h>			//��� �������������� unicode<->ANSI

/******************************************************************************************
*	     �����������		�����������		�����������		�����������		�����������
//�� ���� ������, �� ���� ����������� ����������� � ��� �����, �� ��� ���������� 
//release ������� visual c++ ������ ����������
*******************************************************************************************/
CFunctionClass::CFunctionClass()
{
		ProporCoef=1;				//��� ������������ ������������������
		ChartType=G_LINECHART;		//������ - ����������� �����
		szFunctionName=_T("No named");	//���� ���� �����
		szYLegend=_T("no");			//��� ������� ���������
		szXLegend=_T("�����, �");	//������� �� ��� �
		Visible=false;				//�� ����� �� ���� ����������
		AlwaysDown=false;			//�������� �������� ����������� ����� ������
		Color=RGB(255,0,0);			//���� �������
		ColorEx=RGB(0,255,0);		//���� ��� ������ �������������� ����
		LineWidth=1;				//������� ����� ���� �������
		LineType=0;					//���� �� �����������
}

/********************************************************************************************
*		����������		����������		����������		����������
********************************************************************************************/
CFunctionClass::~CFunctionClass()
{
}

/******************************** AddGraphWhithTime (float var, float time) *************************
*		��� ������� ���������� ����������, ������ ����� ��������� ������������� � �������� ���������,
*	� �� ����������� �������������.
******************************************************************************************************/
bool CFunctionClass::AddGraphWhithTime (FP32 var, FP32 time)
{	//!!!!! time - � ��������!!!!!
	//if(m_StopALL)
	//{//���������� �����������
	//	return false;
	//}
	pPlotXYItems.push_back(CDPoint(time,var));//��������� ����� �����
	return true;
}

/**********************	GetMaxValue()   GetMinValue()	****************************************
*	��� ��� ������� ���������� ������������ �������� � ����������� �������� (��������������) 
*	�� ���� �����, ������� �������� ������ �������, ��� �������, ��� ��������������� �����������
*	������ ��������� �� �������
*		FromTime � ToTime - ��������� �������, � ������� ������ ��������
*	���� ������� ������, ��� �� �������� � �������� ����, �� �������� -FLT_MAX ��� FLT_MAX
*		UseProporCoef - true - ��� ����������� ����������� ����������� ������������������ �������
*						false - �� �����������
************************************************************************************************/
FP32 CFunctionClass::GetMaxValue(FP32 FromTime, FP32 ToTime, bool UseProporCoef)
{
	FP32 MAX=-FLT_MAX;	//�� ����, ��-����� ������ ��� ������, ���� ��������� ������...
	for(size_t i=0; i<pPlotXYItems.size();i++)
	{//��������������� �������� ��������
		FP32 lValue=pPlotXYItems[i].y;
		if(UseProporCoef) lValue*=ProporCoef;		//���� ����, ���� ����������� ������������������
		if(lValue>MAX && pPlotXYItems[i].x>=FromTime && pPlotXYItems[i].x<=ToTime)
		{
			MAX=lValue;
		}
	}
	//������ ��, ��� ���������
	return MAX;
}
FP32 CFunctionClass::GetMinValue(FP32 FromTime, FP32 ToTime, bool UseProporCoef)
{
	FP32 MIN=FLT_MAX;	//�� ����, ��-����� ������ ��� ������, ���� ��������� ������...
	for(size_t i=0; i<pPlotXYItems.size();i++)
	{//��������������� �������� ��������
		FP32 lValue=pPlotXYItems[i].y;
		if(UseProporCoef) lValue*=ProporCoef;		//���� ����, ���� ����������� ������������������
		if(lValue<MIN && pPlotXYItems[i].x>=FromTime && pPlotXYItems[i].x<=ToTime)
		{
			MIN=lValue;
		}
	}
	//������ ��, ��� ���������
	return MIN;
}

/**********************	GetMaxTime()   GetMinTime()	****************************************
*	��� ��� ������� ���������� ������������ �������� � ����������� �������� (��������������) ������� 
*	��� ������� ������������ �����, ��� �������, ��� ��������������� �����������
*	������ ��������� �� �������
************************************************************************************************/
FP32 CFunctionClass::GetMaxTime()
{
	float MAX=-FLT_MAX;	//�� ����, ��-����� ������ ��� ������, ���� ��������� ������...
	for(size_t i=0; i<pPlotXYItems.size();i++)
	{//��������������� �������� ��������
		if(pPlotXYItems[i].x>MAX)
		{
			MAX=pPlotXYItems[i].x;
		}
	}
	//������ ��, ��� ���������
	return MAX;
}

FP32 CFunctionClass::GetMinTime()
{
	float MIN=FLT_MAX;	//�� ����, ��-����� ������ ��� ������, ���� ��������� ������...
	for(size_t i=0; i<pPlotXYItems.size();i++)
	{//��������������� �������� ��������
		if(pPlotXYItems[i].x<MIN)
		{
			MIN=pPlotXYItems[i].x;
		}
	}
	//������ ��, ��� ���������
	return MIN;
}


/***************************	CutTimeOneFunc	**********************************************
*	��� ������� �������� �� ������� ��� �������� ��������, ��� �������� �� �������� � ��������
************************************************************************************************/
bool CFunctionClass::CutTimeOneFunc(FP32 TimeFrom, FP32 TimeTo, bool reverse)
{	//reverse == true - �������� ����������, � ��������� ��������
	//reverse == false - ��� ��������
	if(TimeTo<TimeFrom)
	{//����� �� ���� ������-�� ������ ������� ������
		return false;
	}
	size_t i;
	unsigned long number=0;
	for(i=0;i<pPlotXYItems.size();i++)
	{//��������������� ����������� ��� �������� �������
		if(reverse)
		{//�������� ����������
			if(pPlotXYItems[i].x<TimeFrom || pPlotXYItems[i].x>TimeTo)
			{//������� �� ����� � ������ ��������, ������ ���
				pPlotXYItems.erase(pPlotXYItems.begin()+number);
				i--;	//�.�. ���� ������� �������
			}
			else
			{//�������� ��������� ����������� ���������
				number++;
			}
		}
		else
		{//���������� ��������
			if(pPlotXYItems[i].x>=TimeFrom && pPlotXYItems[i].x<=TimeTo)
			{//������� �� ����� � ������ ��������, ������ ���
				pPlotXYItems.erase(pPlotXYItems.begin()+number);
				i--;	//�.�. ���� ������� �������
			}
			else
			{//�������� ��������� ����������� ���������
				number++;
			}
		}
		
	}
	return true;	//��� � �������
}

/******************************    WriteFunction    ************************************
*		���������� ������ ������� � ����� �� ��������� ����������� ������.
*		!!! ������ ����� ������� ������ � ����� FileFormats.txt
*		������� ������ 1.1
****************************************************************************************/
bool CFunctionClass::WriteFunction(ofstream& os)
{
	CString str;
	INT16U len;
	unsigned long data_len;

	os <<'$'; //����������� �����

	/*����� ����� �������*/
	len=szFunctionName.GetLength();
	if(len>255)	return false;							//����� �� ����� ���� ������ 255 ����
	os.write((char*)&len,sizeof(INT8U));
	os <<'$';  //����������� �����
	
	/*���� ��� �������*/
	USES_CONVERSION;
	os.write(T2A(szFunctionName.GetBuffer()), len);
	os <<'$'; //����������� �����
	
	/*���� �������*/
	os.write((char *)&Color,sizeof(COLORREF));
	os <<'$'; //����������� �����
	
	/*��� ���������� �������*/
	os.write((char *)&ChartType,sizeof(INT8U));
	os <<'$'; //����������� �����

	/*����� ������� ��������� �������*/
	len=this->szYLegend.GetLength();
	if(len>255)	return false;							//����� �� ����� ���� ������ 255 ����
	os.write((char *)&len,sizeof(INT8U));
	os <<'$'; //����������� �����

	/*������� ��������� �������*/
	os.write( T2A(szYLegend.GetBuffer()), len);
	os <<'$'; //����������� �����

	/*������ ����� �������*/
	os.write((char*)&this->LineWidth, sizeof(INT8U));
	os <<'$'; //����������� �����

	/*��� ����� �������*/
	os.write((char*)&this->LineType, sizeof(INT8U));
	os <<'$'; //����������� �����

	/*�������� AlwaysDown*/
	os.write((char*)&this->AlwaysDown, sizeof(INT8U));
	os <<'$'; //����������� �����

	/*�������������� ���� ������� ColorEx*/
	os.write( (char *)&this->ColorEx, sizeof(COLORREF));
	os <<'$'; //����������� �����

	/*���������������� �����������*/
	os.write( (char *)&this->ProporCoef, sizeof(FP32));
	os <<'$'; //����������� �����

	/*���������� ��������� (�����) � �������*/
	data_len=(INT64U)this->pPlotXYItems.size();
	//����� ������ ����� �� � ���������� ��������� CDPoint, � � ������
	//��� �������, �� ����������
	data_len=data_len*2*sizeof(float);
	os.write((char *)&data_len,sizeof(INT64U));
	os <<'$'; //����������� �����
	
	/*���� �������� ������� (�����)*/
	for(unsigned long i=0;i<this->pPlotXYItems.size();i++)
	{
		os.write((char *)&(this->pPlotXYItems[i].x),sizeof(float));
		os.write((char *)&(this->pPlotXYItems[i].y),sizeof(float));
	}
	os <<'$'; //����������� �����

	return true;		//��� ������ ���������
}


/***************************    ReadFunction_1_0   *************************************
*		��������� ������� �� ������. ������ 1.0
****************************************************************************************/
bool CFunctionClass::ReadFunction_1_0(ifstream& is)
{
	char separate=0;
	INT8U len=0;
	char name[255];
	unsigned long data_len;
	
	//�������� �������� �� ���������, ����� ������� ��������� ���������
	this->ProporCoef=1; //��������� ����������� ������

	is.read(&separate,1); //������� �����������
	if(separate!='$')
	{
		//�������� ������� ������ �� ���������, �� ����� ����������
		is.close();
		return false;
	}
	separate=0;	//���� ����� ����� �����, �� ��� ������ seperate �� ���������� 

	is.read((char*)&len,sizeof(unsigned char));//������� ����� ����� �������
	
	/*������� ����� ����� �������*/
	is.read(&separate,1); //������� �����������
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("��������� �������� ����������� ����� ���� ����� ����� �������, �������� ��������"));
		is.close();
		return false;
	}
	separate=0;	//���� ����� ����� �����, �� ��� ������ seperate �� ����������

	/*������� ��� �������*/
	is.read(name, len);
	name[len]=0; //������� ������ �������������� �����
	this->szFunctionName=name;

	is.read(&separate,1); //������� �����������
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("��������� �������� ����������� ����� ���� ����� �������, �������� ��������"));
		is.close();
		return false;
	}
	separate=0;	//���� ����� ����� �����, �� ��� ������ seperate �� ����������

	/*������� ���� ����������� �������*/
	is.read((char *)&this->Color, sizeof(COLORREF));

	is.read(&separate,1); //������� �����������
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("��������� �������� ����������� ����� ���� ����� �������, �������� ��������"));
		is.close();
		return false;
	}
	separate=0;	//���� ����� ����� �����, �� ��� ������ seperate �� ����������

	/*������� ��� �������*/
	is.read((char*)&this->ChartType, sizeof(unsigned char));

	is.read(&separate,1); //������� �����������
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("��������� �������� ����������� ����� ���� ���� �������, �������� ��������"));
		is.close();
		return false;
	}
	separate=0;	//���� ����� ����� �����, �� ��� ������ seperate �� ����������

	/*������� ����� ������ ��������� �������*/
	is.read((char*)&len,sizeof(unsigned char));//������� ����� ����� �������
	
	is.read(&separate,1); //������� �����������
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("��������� �������� ����������� ����� ���� ����� ������� ��������� �������, �������� ��������"));
		is.close();
		return false;
	}
	separate=0;	//���� ����� ����� �����, �� ��� ������ seperate �� ����������

	/*������� ������� ��������� �������*/
	is.read(name,len);
	name[len]=0;
	this->szYLegend=name;

	is.read(&separate,1); //������� �����������
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("��������� �������� ����������� ����� ���� ������� ��������� �������, �������� ��������"));
		is.close();
		return false;
	}
	separate=0;	//���� ����� ����� �����, �� ��� ������ seperate �� ����������

	/*������� ����� � ������ ���� � �������*/
	is.read((char *) &data_len, sizeof(long));

	is.read(&separate,1); //������� �����������
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("��������� �������� ����������� ����� ���� ����� ������, �������� ��������"));
		is.close();
		return false;
	}
	separate=0;	//���� ����� ����� �����, �� ��� ������ seperate �� ����������

	data_len=data_len/(2*sizeof(float));

	for (unsigned long i=0; i<data_len;i++)
	{
		//CDPoint point;
		float x,y;
		is.read((char*)&x,sizeof(float));
		is.read((char*)&y,sizeof(float));
		this->pPlotXYItems.push_back(CDPoint(x,y));
	}

	is.read(&separate,1); //������� �����������
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("��������� �������� ����������� ����� ������, �������� ��������"));
		is.close();
		return false;
	}
	separate=0;	//���� ����� ����� �����, �� ��� ������ seperate �� ����������

	this->Visible=true;	//������ ���� �� ����� ������ - ������� ����������� ��������� � �� ����� ����������

	return true;
}

/***************************    ReadFunction_1_1   *************************************
*		��������� ������� �� ������. ������ 1.1
****************************************************************************************/
bool CFunctionClass::ReadFunction_1_1(ifstream& is)
{
	char separate=0;
	INT8U len=0;
	char name[255];
	unsigned long data_len;
	
	is.read(&separate,1); //������� �����������
	if(separate!='$')
	{
		//�������� ������� ������ �� ���������, �� ����� ����������
		is.close();
		return false;
	}
	separate=0;	//���� ����� ����� �����, �� ��� ������ seperate �� ���������� 

	is.read((char*)&len,sizeof(unsigned char));//������� ����� ����� �������
	
	/*������� ����� ����� �������*/
	is.read(&separate,1); //������� �����������
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("��������� �������� ����������� ����� ���� ����� ����� �������, �������� ��������"));
		is.close();
		return false;
	}
	separate=0;	//���� ����� ����� �����, �� ��� ������ seperate �� ����������

	/*������� ��� �������*/
	is.read(name, len);
	name[len]=0; //������� ������ �������������� �����
	this->szFunctionName=name;

	is.read(&separate,1); //������� �����������
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("��������� �������� ����������� ����� ���� ����� �������, �������� ��������"));
		is.close();
		return false;
	}
	separate=0;	//���� ����� ����� �����, �� ��� ������ seperate �� ����������

	/*������� ���� ����������� �������*/
	is.read((char *)&this->Color, sizeof(COLORREF));

	is.read(&separate,1); //������� �����������
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("��������� �������� ����������� ����� ���� ����� �������, �������� ��������"));
		is.close();
		return false;
	}
	separate=0;	//���� ����� ����� �����, �� ��� ������ seperate �� ����������

	/*������� ��� �������*/
	is.read((char*)&this->ChartType, sizeof(unsigned char));

	is.read(&separate,1); //������� �����������
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("��������� �������� ����������� ����� ���� ���� �������, �������� ��������"));
		is.close();
		return false;
	}
	separate=0;	//���� ����� ����� �����, �� ��� ������ seperate �� ����������

	/*������� ����� ������ ��������� �������*/
	is.read((char*)&len,sizeof(unsigned char));//������� ����� ����� �������
	
	is.read(&separate,1); //������� �����������
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("��������� �������� ����������� ����� ���� ����� ������� ��������� �������, �������� ��������"));
		is.close();
		return false;
	}
	separate=0;	//���� ����� ����� �����, �� ��� ������ seperate �� ����������

	/*������� ������� ��������� �������*/
	is.read(name,len);
	name[len]=0;
	this->szYLegend=name;

	is.read(&separate,1); //������� �����������
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("��������� �������� ����������� ����� ���� ������� ��������� �������, �������� ��������"));
		is.close();
		return false;
	}
	separate=0;	//���� ����� ����� �����, �� ��� ������ seperate �� ����������

	/*������ ����� �������*/
	is.read((char*)&this->LineWidth, sizeof(INT8U));
	is.read(&separate,1); //������� �����������
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("��������� �������� ����������� ����� ���� ������ ����� �������, �������� ��������"));
		is.close();
		return false;
	}
	separate=0;	//���� ����� ����� �����, �� ��� ������ seperate �� ����������

	/*��� ����� �������*/
	is.read((char*)&this->LineType, sizeof(INT8U));
	is.read(&separate,1); //������� �����������
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("��������� �������� ����������� ����� ���� ���� ����� �������, �������� ��������"));
		is.close();
		return false;
	}
	separate=0;	//���� ����� ����� �����, �� ��� ������ seperate �� ����������

	/*�������� AlwaysDown*/
	is.read((char*)&this->AlwaysDown, sizeof(INT8U));
	is.read(&separate,1); //������� �����������
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("��������� �������� ����������� ����� ���� �������� \"Always Down\" ����� �������, �������� ��������"));
		is.close();
		return false;
	}
	separate=0;	//���� ����� ����� �����, �� ��� ������ seperate �� ����������

	/*�������������� ���� ������� ColorEx*/
	is.read( (char *)&this->ColorEx, sizeof(COLORREF));
	is.read(&separate,1); //������� �����������
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("��������� �������� ����������� ����� ���� �������� \"ColorEx\" �������, �������� ��������"));
		is.close();
		return false;
	}
	separate=0;	//���� ����� ����� �����, �� ��� ������ seperate �� ����������

	/*���������������� �����������*/
	is.read( (char *)&this->ProporCoef, sizeof(FP32));
	is.read(&separate,1); //������� �����������
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("��������� �������� ����������� ����� ���� ����������������� ������������ �������, �������� ��������"));
		is.close();
		return false;
	}
	separate=0;	//���� ����� ����� �����, �� ��� ������ seperate �� ����������

	/*������� ����� � ������ ���� � �������*/
	is.read((char *) &data_len, sizeof(long));

	is.read(&separate,1); //������� �����������
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("��������� �������� ����������� ����� ���� ����� ������, �������� ��������"));
		is.close();
		return false;
	}
	separate=0;	//���� ����� ����� �����, �� ��� ������ seperate �� ����������

	data_len=data_len/(2*sizeof(float));

	for (unsigned long i=0; i<data_len;i++)
	{
		//CDPoint point;
		float x,y;
		is.read((char*)&x,sizeof(float));
		is.read((char*)&y,sizeof(float));
		this->pPlotXYItems.push_back(CDPoint(x,y));
	}

	is.read(&separate,1); //������� �����������
	if(separate!='$')
	{
		AfxMessageBox(_TEXT("��������� �������� ����������� ����� ������, �������� ��������"));
		is.close();
		return false;
	}
	separate=0;	//���� ����� ����� �����, �� ��� ������ seperate �� ����������

	this->Visible=true;	//������ ���� �� ����� ������ - ������� ����������� ��������� � �� ����� ����������

	return true;
}
