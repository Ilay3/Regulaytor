#include "stdafx.h"
#include "Filters.h"
#include "Math_ex_visual_c++.h"	//��� ��
#include <math.h>				//����������

/**************************************************************************
*						����������� �� ���������
**************************************************************************/
CAperiodicLinkRandPeriod::CAperiodicLinkRandPeriod()
{
	Init(1);
}

/**************************************************************************
*							�������������
**************************************************************************/
bool CAperiodicLinkRandPeriod::Init(FP32 FreqCutOff_hz, FP32 StartValue)
{
	if(!SetFreqCutOff(FreqCutOff_hz)) return false;
	m_y_1=StartValue;
	return true;
}

/***********************     SetFreqCutOff    ****************************
*		������ ������� ����� ������� (� ������)
*************************************************************************/
bool CAperiodicLinkRandPeriod::SetFreqCutOff(FP32 FreqCutOff_hz)
{
	if(FreqCutOff_hz<=0) return false;
	m_T=1.f/FreqCutOff_hz;
	return true;
}

/***********************     Calculate     *****************************
*		��������� ����� ������� �� ��������� ����� � ������� �����
*	���������� ����������.
***********************************************************************/
FP32 CAperiodicLinkRandPeriod::Calculate(FP32 input, FP32 period_s)
{
	if(period_s<=0) return 0;

	FP32 temp=m_T/period_s;
	FP32 lResult=(input+m_y_1*temp)/(1+temp);
	m_y_1=lResult;
	return lResult;
}

/**********************************************************************
*						�����������
**********************************************************************/
CAperiodicLinkRandPeriodAutoStep::CAperiodicLinkRandPeriodAutoStep()
{
	m_PredTime=GetTickCount();
}
CAperiodicLinkRandPeriodAutoStep::CAperiodicLinkRandPeriodAutoStep(FP32 FreqCutOff_hz, FP32 StartValue)
{
	Init(FreqCutOff_hz,StartValue);
	m_PredTime=GetTickCount();
}

/***********************     Calculate     *****************************
*		��������� ����� ������� �� ��������� �����. ������ ����� �����������
*	����������� � ������� ������� GetTickCount().
*		pPeriod - ���� �� NULL, �� ���� ����� ������� ������ �����
*					���� � ���������� �����������
***********************************************************************/
FP32 CAperiodicLinkRandPeriodAutoStep::Calculate(FP32 input, INT32U *pPeriod)
{
	INT32U lTickCount=GetTickCount();
	INT32U lDifference=lTickCount-m_PredTime;
	FP32 lResult;
	if(lDifference==0)
	{//������, ��� ������ ��������� ������ � ���������� � ���� ������������
	//� � �� ���� ����� ����� ���� �������
		//������������� ���������� 1 ��
		lDifference=1;
	}

	lResult=CAperiodicLinkRandPeriod::Calculate(input,lDifference/1000.f);
	m_PredTime=lTickCount;
	if(pPeriod!=NULL) *pPeriod=lDifference;
	return lResult;
}

/**********************    FreqCalculate    ******************************
*		��� ������� ��������� ������������� �������� �� �� ������ �����,
*	� �� ������ ������� �� ����������� ���������. �� ������� ����� ���� 
*	��������-�������������� ������.
*		pPeriod - ���� �� NULL, �� ���� ����� ������� ������ �����
*					���� � ���������� �����������
*************************************************************************/
FP32 CAperiodicLinkRandPeriodAutoStep::FreqCalculate(INT32U *pPeriod)
{
	INT32U lTickCount=GetTickCount();
	INT32U lDifference=lTickCount-m_PredTime;
	if(lDifference==0)
	{//������, ��� ������ ��������� ������ � ���������� � ���� ������������
	//� � �� ���� ����� ����� ���� �������
		//������������� ���������� 1 ��
		lDifference=1;
	}
	FP32 lResult;
	FP32 inv_temp=1000.f/(FP32)lDifference;
	lResult=CAperiodicLinkRandPeriod::Calculate(inv_temp,(FP32)(lDifference/1000.f));
	m_PredTime=lTickCount;
	if(pPeriod!=NULL) *pPeriod=lDifference;
	return lResult;
}

/**********************    GetOptimalPointsCount_LF    ****************************
*		�������� ������������� ��� ������ �� ������� �������� �����������. 
*	���� �������� ������� ��������������. ��. ������� ���. 398. ����� ��������
*	���������� ������������ �������������. ������������������:
*		��� ������� ����������, �.�. ��������� ������ �������, �������� 2�� - ���
*	��������� ��������� ������ �������
*********************************************************************************/
bool GetOptimalPointsCount_LF(INT16U MaxCount, FP32 ThresholdFrequency, FP32 SampleRate, INT16U* pOptimalCount)
{
	//�������� � �������� �������
	FP32 lRadianThresholdFreq=2*MATH_PI*ThresholdFrequency;

	//������� ����������� ���������� �����, ����������� ��� ������������� ����������
	//���� ������, ��� ��� ����� ��� ������� ���� ������ sin(n*T*wc) � ������ �������
	INT16U lMinPointsCount=2/ThresholdFrequency/SampleRate;

	if(lMinPointsCount<4)		//������ �� ������ (�� ����� ���� ������ 10 ����� �� 2 ������� ���������)
		return false;			//������� ������� ������ ������������� ��� ���������� ������ �������

	if(lMinPointsCount>MaxCount)
		return false;			//������� ��������� ������ ��� ���������� ������ �������

	INT16U lOptimalPointsCount=lMinPointsCount;
	
	//����� ���������, ����� ���������� �����, ����� ���� ���������� �������������� ������� ���������� �� ��������� ������
	//(����� ���������� ��������), � ���� ������ ������ ������ ����� ����������� �� ��� ������

	lOptimalPointsCount=lMinPointsCount;
	INT16U i=4;		//����� � ������� ����� ���������, �.�. ����� ����� � lMinPointsCount
	do
	{
		INT16U temp=i/ThresholdFrequency/SampleRate;
		if(temp>MaxCount)
			break;							//���������� �� ������ �������
		else
			lOptimalPointsCount=temp;		//���, ����� ���-�� ����� ��� ���� ��������
		i+=2;
	}
	while(1);
	if(!(lOptimalPointsCount & 0x1))
		lOptimalPointsCount--;				//� ������ ���� �������� �� ���������
	
	*pOptimalCount=lOptimalPointsCount;
	return true;
}

/*************************    CreateCorsWeighting_LF    ************************
*		������� ������������ ��� ������� ������ ������ �� ���������� �������
*	� ��������� ��������. ������� �������� �����������.
*		���� ����������� �� ���� �������������, �� ���������� ������ ��-��
*	�������� � ������ ������������ ����. 
*	pMas - ��������� �� ������ ������������� �������
*	Size - ������ ����� �������
*	ThresholdFrequency - ������� ����� [��]
*	SampleRate - ������ ������������� [c]
*	���� ������ �� ������ � ����� ������� �������������, ��� ����� ������ �������,
*	�� ������������ ����.
*		1. ����� ��������� ���������� �������������� 
*		Hd(n)=2*fc*sin(n*T*wc)/(n*T*wc)
*			n - ����� ������������ (�.�. ������������� � �������������)
*			T - ������ ������������� [c]
*			fc - ������� ����� [��]
*			wc - ������� ����� [���*�]
********************************************************************************/
bool CreateCorsWeighting_LF(FP32 *pMas,INT16U Size, FP32 ThresholdFrequency, FP32 SampleRate)
{
	if(!(Size & 0x1))
		return false;		//������ ������� �� ����, � ��� �������� ���������� �����

	//������ ������������ �����
	INT16U lSideSize=(Size-1)>>1;

	//�������� � �������� �������
	FP32 lRadianThresholdFreq=2.f*MATH_PI*ThresholdFrequency;

	//������� ����� H(0)=2*fc
	pMas[lSideSize]=2.f*ThresholdFrequency;
	for(INT16U i=1; i<=lSideSize; i++)
	{
		//H(n)=2*fc*sin(wc*n*T)
		FP32 temp1=lRadianThresholdFreq*i*SampleRate;
		FP32 temp=2.f*ThresholdFrequency*sin(temp1)/temp1;
		pMas[lSideSize+i]=pMas[lSideSize-i]=temp;

	}
	
	return true;
}

// w(n)=0.54+0.46*cos(2*pi*n/N) ��� -(N-2)/2<=n<=(N-2)/2
// 0 � ������ �������
bool ProcessHamming(FP32 *pMas,INT16U Size, FP32 ThresholdFrequency, FP32 SampleRate)
{
	INT16U lSideSize=Size>>1;	//������ ������������ �����
	for(INT16S n=-lSideSize; n<=lSideSize; n++)
	{
		pMas[n+lSideSize]*=0.54f+0.46f*cos(2*MATH_PI*n/Size);
	}
	return true;
}
