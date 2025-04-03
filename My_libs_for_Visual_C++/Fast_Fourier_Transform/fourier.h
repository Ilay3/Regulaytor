/*************************************************************************************************
**************************************************************************************************
**	���� ������ �������� ������� ��� ������� � ��������� ����������� �������� �������������� �����. 
**	��������� ������ ������� ��� ������ � ����� float � double (����� ����������� ������ ��������).
**	����� ���� �� ������� �� �� ������������ (� ������������� complex), ������ ������� ���� �����
**	������������� �� ����������.
**	����� ��������� �����������������. ���� ������ ����� �������� ��� �� �������� ��������� ������
**	(�� ����� ������ ��� �� ����������),	��� � �� ����������� (����� ������ ������� ����������).
**	��������� ������: ������� �� �������������� ��� ���������� ���������� (����������� �������), �
**	����� �������� �������, ���� ��� �� ���� ���������� ����� ����������. ��� ������� ������� � ���-
**	�������� ������� ���� ��������������� ���������� ���������, ���� ����� ������ ������.
**************************************************************************************************
***************************************************************************************************/

/******************************	���� ����������� ���������		************************
*	27.02.07 - ������
*	28.02.07 - ��� ������� �������� � ����� Fourier_Class, ��� ����� ������� ������������
*				(�������� ��� ��������� ������). ���������������� ��������� ���������, ��
*				�������� ����� ����� ��� �� ��� ��������� ��������
**************************************************************************************************/

#pragma once //��������� �� ���������� ���������
#include <math.h>
#include <complex>

#include "types_visual_c++.h"				//��� ���� ������
#include "ERRORS\ERRORS.h"					//��� ��������� ������
#include "FunctionClass\FunctionClass.h"	//����� ��������� ��������������� �������� �� ������
#include "Math_ex_visual_c++.h"				//��� ��


//***********	��������� ��� ������� fft_float	****************************
	//�������� InverseTransform
#define FFT_DIRECT	0	//����� ������ ������������� �����
#define FFT_INVERSE 1	//����� �������� �������������� �����

// �������� flags ������� FFT_Transform
//������ ������������� �����: ��� �� ����� �������� �� ������?
#define FFT_DO_NONE							0x00 // ������������ ������ ��������, ���� �� �������� �� ���������� ���������
#define FFT_ONE_SIDED_AMPLITUDE_SPECTRUM	0x01 // ������������� ������ ��������
#define FFT_TWO_SIDED_AMPLITUDE_SPECTRUM	0x02 // ���������� ������������ ������ ��������
#define FFT_ONE_SIDED_POWER_SPECTRUM		0x03 // ������������� ������ �������� 
#define FFT_TWO_SIDED_POWER_SPECTRUM		0x04 // ������������ ������ ��������
#define FFT_MEAN_SQUARE_SPECTRUM			0x05 // ������� ������ - ������������ ��������
		// ��������� - �� �����
		// � ������� 2 � 3 ������ ����� ����������, ����� �� ������� ��������, �.�. �� ����� ������� ����������
//�������� �������������� �����: � ����� ������� ������� ������
#define FFT_INV_DO_NONE								0x00 // ������������ ������ ��������, ���� �� �������� �� ���������� ���������
#define FFT_INV_TWO_SIDED_AMPLITUDE_SPECTRUM		0x01 // ���������� ������������ ������ ��������
#define FFT_INV_TWO_SIDED_AMPLITUDE_SPECTRUM_CUTTED 0x02 // ���������� ������������ ������ ��������, �� ������������ ����� ��������
		// ��������� - �� �����

/******************	������������� �������	******************************************/
/*
//��� ������ � ������� ���������
void fft_double (
    unsigned int  NumSamples,     // ������ �������� �������, ������ ���� �������� ������
    unsigned char InverseTransform, // ����, 0 - ������ ���, 1 - �������� ���
    double   *RealIn,              	// ������ ������� ������������ ��������
    double   *ImaginaryIn,         	// ������ ������� ����������� ��������, ���� NULL, �� ��� �������
    double   *RealOut,             	// ������ �������� ������������ ��������
    double   *ImaginaryOut );      // ������ �������� ����������� ��������
*/

bool	FFT_IsPowerOfTwo ( unsigned int x );										//���������, �������� �� ����� �������� ������
INT16U	FFT_NumberOfBitsNeeded ( unsigned int PowerOfTwo );
INT16U	FFT_ReverseBits ( unsigned int index, unsigned int NumBits );
FP32	FFT_Index_to_frequency ( unsigned int NumSamples, unsigned int Index );	
//bool	FFT_SetTime_real(float *mas, int size, bool copy=true);	//��������� �������������� ��������� ������ ������� �� ����������
//bool	FFT_SetTime_real(CDPoint *mas, int size);					//�� �� �����, �� �������� � ��������, ��� ��� ���� �����
//bool	FFT_GetTimeReal(FunctionClass *pFunction,CString name);	//�������� ������ �� ������� � �������
float	FFT_GetFloatPoint(float time);	//������ ����� �� ������� �� �������� ��������
//bool	FFT_GetFilteredFunction(FunctionClass *pFunction, int points_count, CString name);	//��������� ������������� �������
//�������� ������� ������� ��� �������������� �����
bool	FFT_TransformComplex (	FP32* pRealIn,
								FP32* pImagIn,
								FP32* pRealOut,
								FP32* pImagOut,
								INT16U size_of_sample,
								INT8U InverseTransform=FFT_DIRECT);
bool	FFT_Process			 (	FP32*  pRealIn,
								FP32*  pImagIn,
								INT16U size_of_sample,
								INT8U  Flags);
INT16U	FFT_Nearest2Power(INT16U value, INT8U *pWhatPower=NULL);
	
/*class Fourier_Class
{//�������� ���� ������� ����  � ��� �����
	//��������������� �������
	
	
public:
	float* Time_real;	//������ ��� �������������� �������� �� ��������� �������
	bool Time_real_extern;	
	float* Time_imag;	//������ ��� ������ �������� �� ��������� �������
	bool Time_imag_extern;
	float* Freq_real;	//������ ��� ������������ �������� � ��������� �������
	bool Freq_real_extern;
	float* Freq_imag;	//������ ��� ������ �������� � ��������� �������
	bool Freq_imag_extern;
	
	float PeriodOfSamples;//������ ����� ����������� �����������

	
	

	bool GetPowerDensitySpectrum(FunctionClass *pFunction, CString name, float multiplier=1);		//������ ������ ��������
	bool GetAmplitudeSpectrum(FunctionClass *pFunction, CString name, float multiplier=1);			//������ ����������� ������

	
	unsigned char Filter[20];	//���� ����������� ������, ������� ����� ����������, ����� ��������� 
								//��������� ��� ���������� ������� �� �������������� �����.
								//������ ���������������� ��� - ��������� ��������� (������� � �������)
								// 1 - ��������� �� �����������
								// 0 - ��������� �����������
	
	Fourier_Class();	//�����������
	~Fourier_Class();	//����������
	
	int Size;			//������ �������
	//float GivePoint(float *RealIn, float *RealOut, float time);
};*/







