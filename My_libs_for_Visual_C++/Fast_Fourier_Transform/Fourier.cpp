/*********************************************************************************************************
***********************************************************************************************************
**	����� ��������� ������� ��� ������ �������������� � ����� float
*********************************************************************************************************
*********************************************************************************************************/
#include "stdafx.h"
#include "Fourier.h"		//�������������� �����


/*************************	FFT_Transform	*****************************************************
*		��� ������ ������ ��������������.
*		�� ������ ���� � ������� ��� ������� pRealOut � pImagOut, � ������� ��������� ���������. ������
*	�� ������� �� ��������� �������� �������� �������������� �����.
*
*	���� ������� �������� ���, ��� ��� �������� ��� ������� ������ �������� ������ ����������������� � �������������
*		InverseTransform -	FFT_DIRECT ������ ���
*							FFT_INVERSE �������� ���
**************************************************************************************************/
bool FFT_TransformComplex(
	FP32* pRealIn,
	FP32* pImagIn,
	FP32* pRealOut,
	FP32* pImagOut,
	INT16U size_of_sample,
	INT8U InverseTransform)		
{
    unsigned NumBits;    /* Number of bits needed to store indices */
    unsigned i, j, k, n;
    unsigned Blocksize_of_sample, BlockEnd;

    FP32 angle_numerator = 2.0f * (FP32)MATH_PI;
    FP32 tr, ti;     /* temp real, temp imaginary */

    if ( !FFT_IsPowerOfTwo(size_of_sample) )
    {//������ ������� �� ������� ������, ��� �������� ����������� ��� ����� ��������� ���
        return false;
    }

	if(pRealIn==NULL || pRealOut==NULL || pImagOut==NULL)
	{//���� �� ����������� �������� ����� ����
		return false;
	}

	if (InverseTransform==FFT_INVERSE)
	{//�������� �������� �������������� �����
		/*
		!!!! ��������� � ���-�� ������� �� FFT_Process_Inverse!!!
		if(flags==FFT_INV_TWO_SIDED_AMPLITUDE_SPECTRUM_CUTTED)
		{//���������� ������������ ������ ��������,�� ������������ ����� ��������
			//��� ������ ����������� ������������ �����
			//�������� ���-�� ��������, ����� ���������� ������������ �� ������
			for(INT16U i=1; i<size_of_sample/2; i++)
			{
				pRealIn[i+size_of_sample/2]=pRealIn[size_of_sample/2-i];
			}
		}
		*/
        angle_numerator = - angle_numerator;	//��� ���������� ���� ���������
	}

    NumBits = FFT_NumberOfBitsNeeded ( size_of_sample );

    /*
    **   Do simultaneous data copy and bit-reversal ordering into outputs...
    */

    for ( i=0; i < size_of_sample; i++ )
    {
        j = FFT_ReverseBits ( i, NumBits );
        pRealOut[j] = pRealIn[i];
        pImagOut[j] = (pImagIn == NULL) ? 0.0f : pImagIn[i];
    }

    /*
    **   Do the FFT itself...
    */

    BlockEnd = 1;
    for ( Blocksize_of_sample = 2; Blocksize_of_sample <= size_of_sample; Blocksize_of_sample <<= 1 )
    {
        FP32 delta_angle = angle_numerator / (FP32)Blocksize_of_sample;
        FP32 sm2 = sin ( -2 * delta_angle );
        FP32 sm1 = sin ( -delta_angle );
        FP32 cm2 = cos ( -2 * delta_angle );
        FP32 cm1 = cos ( -delta_angle );
        FP32 w = 2 * cm1;
        FP32 ar[3], ai[3];

        for ( i=0; i < size_of_sample; i += Blocksize_of_sample )
        {
            ar[2] = cm2;
            ar[1] = cm1;

            ai[2] = sm2;
            ai[1] = sm1;

            for ( j=i, n=0; n < BlockEnd; j++, n++ )
            {
                ar[0] = w*ar[1] - ar[2];
                ar[2] = ar[1];
                ar[1] = ar[0];

                ai[0] = w*ai[1] - ai[2];
                ai[2] = ai[1];
                ai[1] = ai[0];

                k = j + BlockEnd;
                tr = ar[0]*pRealOut[k] - ai[0]*pImagOut[k];
                ti = ar[0]*pImagOut[k] + ai[0]*pRealOut[k];

                pRealOut[k] = pRealOut[j] - tr;
                pImagOut[k] = pImagOut[j] - ti;

                pRealOut[j] += tr;
                pImagOut[j] += ti;
            }
        }

        BlockEnd = Blocksize_of_sample;
    }
	return true;	//�� ������ �������
}

/***************************    FFT_Process    ************************************
*		������� ������������� ������������ ���������� � ������� FFT_TransformComplex
*	����������� ���.
*		�������������� �����:
*	��� ��������� ������������� ������� ��������: ������ ������� ���������� �������� �� ����� ���������
*	��� ������������� ������� ��������: ������ �������� � �������
*	��� �������������� ������� ��������: �������� � �������, ����� �������� �� 2 ���, ����� ������� ���������
*	��� ������� ������-������������ �������� (���): �� ������� �������� ������� ���������� ������
**********************************************************************************/
bool FFT_Process(FP32* pRealIn, FP32* pImagIn, INT16U size_of_sample, INT8U Flags)
{
	//����� �������� �� ���������� �����, ����� ��������� �������� �� �������� (����������� ���)
	for (INT16U i=0; i < size_of_sample; i++ )
	{
		pRealIn[i] /= (FP32)size_of_sample;
		pImagIn[i] /= (FP32)size_of_sample;
	}

	if(Flags==FFT_ONE_SIDED_AMPLITUDE_SPECTRUM || Flags==FFT_TWO_SIDED_AMPLITUDE_SPECTRUM)
	{//����� ��������� �� ���������� � ��������� � ����
		for(INT16U i=0; i<size_of_sample; i++)
		{//���� ��� ���������� � �������� ����� � 2 ���� ������ ����������
			FP32 lReal=pRealIn[i];
			FP32 lImag=pImagIn[i];
			//��������� ������ ������������� ������� � ����������� ���������
			pRealIn[i]=sqrt(lReal*lReal+lImag*lImag);
			//���� ������ ��� ����������, �� ��� ����� ������ ���� >PI � <-PI
			FP32 temp=atan(lImag/lReal);
			if(lReal<0)
			{//���� � ����� ����� ����������� ���������
				temp=-temp;
			}
			//???? ����� ��� ����� ������ ����� �� 2PI ������������� �����
		}
	}
	if(Flags==FFT_TWO_SIDED_AMPLITUDE_SPECTRUM)
	{//����� ������ ������������� ������ ��������
		INT16U i;
		for(i=0; i<size_of_sample/2; i++)
		{//������ �������� �������� ��������, ���� �� ������
			pRealIn[i]*=2.f;
		}
		for(i; i<size_of_sample; i++)
		{//��������� (��� ����������� ������ �������
			pRealIn[i]=0;
			pImagIn[i]=0;
		}
	}
	
	if(Flags==FFT_MEAN_SQUARE_SPECTRUM)
	{//������ ������-������������ ��������
		for(INT16U i=0; i<size_of_sample/2; i++)
		{//������ ������ ��������� ���������
			pRealIn[i]=sqrt(pRealIn[i]);
		}
	}
	else if(Flags==FFT_ONE_SIDED_POWER_SPECTRUM)
	{//������������� ������ �������� (���������� � ���� �� ������ �����������)
		INT16U i;
		for(i=0; i<size_of_sample/2; i++)
		{//����������� � ������� � �������� ������� ������ ����������� �����
			pRealIn[i]=(pRealIn[i]*pRealIn[i]+pImagIn[i]*pImagIn[i])/2.f;
			pImagIn[i]=0;
		}
		for(i; i<size_of_sample; i++)
		{//������ �������
			pRealIn[i]=0;
			pImagIn[i]=0;
		}
	}

	return true;
}

/**********************************************************************************************************
************************************************************************************************************
**		����� ���� �� ����� �������� �������������� �������
**************************************************************************************************************
***********************************************************************************************************/
#define BITS_PER_WORD   (sizeof(unsigned int) * 8)


bool FFT_IsPowerOfTwo ( unsigned int x )
{
    if ( x < 2 )
        return false;

    if ( x & (x-1) )        //��� ������ �������, ������� ��������� �� ������
        return false;

    return true;
}


INT16U FFT_NumberOfBitsNeeded ( unsigned int PowerOfTwo )
{
    unsigned i;

    if ( PowerOfTwo < 2 )
    {//������
        return 0;
    }

    for ( i=0; ; i++ )
    {
        if ( PowerOfTwo & (1 << i) )
            return i;
    }
}



INT16U FFT_ReverseBits ( unsigned int index, unsigned int NumBits )
{
    unsigned int i, rev;

    for ( i=rev=0; i < NumBits; i++ )
    {
        rev = (rev << 1) | (index & 1);
        index >>= 1;
    }

    return rev;
}


FP32 FFT_Index_to_frequency ( unsigned int NumSamples, unsigned int Index )
{
    if ( Index >= NumSamples )
        return 0.0;
    else if ( Index <= NumSamples/2 )
        return (FP32)Index / (FP32)NumSamples;

    return -(FP32)(NumSamples-Index) / (FP32)NumSamples;
}

/**************************************		SetTime_real(float *mas, int size, bool copy, float period)	************
*		��� ������� ����������� ������ �������������� ����� ������� �� ��������� �������. 
*	�� �� ������������:
*		1. ������������� ������������ ������� � �� ������ �� ������� ������ (� ������� �������)
*		2. ����� ����������� ������ �� ���������� �����, ��� �������� � �������
*		3. ���� ������ ������� ���������, ������� ������������� ������� ��� ������� ������ 
*				� ���������
****************************************************************************************************/
/*bool FFT_SetTime_real(CMassive *pMas, INT16U size_of_sample)
{
	int CustomarySize=1;	//������ �������� �������
	if(size<2)
	{//������ ������� ��� ��� ������ ���� �� ������ 2
		return false;
	}

	while(1)
	{//��������������� ��������� �� ���� �������� ������, ���� �� �������� ��������
		CustomarySize=CustomarySize<<1;
		if(CustomarySize>size_of_sample)
		{
			break;
		}
	}

	if(Size==CustomarySize)
	{//������ � ��� ��� ������� �������
		return true;
	}

	//������� ������ �� ������� ������
	pMas->resize(CustomarySize);

	return true;	//��� ���������
}*/

/*********************************	SetTime_real(CDPoint *mas, int size, float period)	*************************
*		���������� ����������, �� ������� ���������� �������� ������ �����,  ���������� �����.
*	������� ������ ��� ��������. ������������ ����� ������ ����� ����������� �� ���������� �����.
*	�.�. ������ ��� � ����� � ������ �����, �� ����� ������� ��������� � ����� �������������
*	(�������� ��� �����������).
*****************************************************************************************************/
/*bool FFT_SetTime_real(CDPoint *mas, int size)					
{
	// mas - ��������� �� ������ �� ����������� �����
	// size - ������ ����� �������
	int CustomarySize=1;	//������ �������� �������
	if(size<2)
	{//������ ������� ��� ��� ������ ���� �� ������ 2
		return false;
	}

	if(IsPowerOfTwo(size))
	{//������ ����� ������������ ��� ������� ���������� ���������� ������
		CustomarySize=size;
	}
	else
	{//��� �� ������� ������, ������� ������������� �������� ������ �� �������
		while(1)
		{//��������������� ��������� �� ���� �������� ������, ���� �� �������� ��������
			CustomarySize=CustomarySize<<1;
			if(CustomarySize>size)
			{
				break;
			}
		}
	}

	if(Size!=CustomarySize)
	{//������ ������� ���������� �������������� �� ���������
		//�������� ���, � ���� ������ �� ��� �� ������ ��������
		//� �������� ��������� (������-�� �� ���), �������
		//�������� �� ������ � �������� (���� ������ �������), ���
		//�������!
		if(Time_real!=NULL && !Time_real_extern)
		{
			delete [] Time_real;
		}
		if(Time_imag!=NULL && !Time_imag_extern)
		{
			delete [] Time_imag;
		}
		if(Freq_real!=NULL && !Freq_real_extern)
		{
			delete [] Freq_real;
		}
		if(Freq_imag!=NULL && !Freq_imag_extern)
		{
			delete [] Freq_imag;
		}
		Size=CustomarySize;//�������� ������ ���������� ����������
	}
	
	//��������� ������ �� ��������� �����
	Time_real= new float [CustomarySize];	//������� ������ ��� ����� ������
	
	int i=0;
	for(;i<size;i++)
	{//������������ �������� ����� �������
		Time_real[i]=mas[i].y;
	}
	//������ ����� ������ ��� ������ �������, ������� �� �������, ��� ���������� �� ������� ������
	for(;i<CustomarySize;i++)
	{//� ����������� �������� ��������� �� ���������� �������� (��� � ����� �� ������ ���������)
		Time_real[i]=Time_real[i-1];
	}

	//�� � ������, ���� �� ��� �������������� ����������� - ����� ������� � ����� �������������
	PeriodOfSamples=(mas[size-1].x-mas[0].x)/static_cast<float>(size);	//��, ������ ����������� �� �����
	return true;	//��� ���������
}*/

/**************************		GetPowerDensitySpectrum(FunctionClass *pFunction)	****************************
*	������ ������ ��������, ���� ������ �������������, ��������� ��� ������������ ��������� ��������
*	��� ��� ���������� �� ������
****************************************************************************************************************/
/*bool FFT_GetPowerDensitySpectrum(FunctionClass *pFunction,CString name, float multiplier)	
{
	//multiplier - ����������� ������ �� ��� �. �������� ��� ����� ��������� ���, ����� ������
	//				��������� ���� ����� ������, � ��������� ����� ������.
	//name		 - ��� ������� �������

	if(pFunction==NULL)
	{//��� ����������� ������� �������� �� ������ ��� ������� ������� ���������
		return false;
	}
	//������� �������, ���� � ��� ���-�� ��������
	pFunction->pPlotXYItems.clear();
	
	//�������� � ����������� �� ��������� ������� 
	int i=0;
	for(i=0; i<=Size/2; i++)
	{//�������� �� ������� ��������� ��������, ������� ������������ ������ ����� �� ������
		float new_value=Freq_real[i]/Size;	//����������� (�������� ������ ������� �� ���-�� ���������)
		new_value*=new_value;						//�������� � �������
		if(i!=0)
		{//������ ���, ����� ������� ���������
			new_value+=new_value;
		}

		//������� ����� � �������
		pFunction->pPlotXYItems.push_back(CDPoint(static_cast<float>(i)*multiplier,new_value));
	}
	
	//������� ��� �������
	pFunction->szFunctionName=name;
	pFunction->ChartType=G_BARCHART_TIGHT;
	pFunction->AlwaysDown=true;
	pFunction->Visible=true;
	//������� ���������, ���������� ����������� ������� �� ���������� ������������

	return true;	//�� ���������
}*/

/**************************		GetAmplitudeSpectrum(FunctionClass *pFunction)	****************************
*	������ ����������� ��������, ���� ������ �������������, ��������� ��� ������������ ��������� ��������
*	��� ��� ���������� �� ������
****************************************************************************************************************/
/*bool FFT_GetAmplitudeSpectrum(FunctionClass *pFunction,CString name, float multiplier)	
{
	//multiplier - ����������� ������ �� ��� �. �������� ��� ����� ��������� ���, ����� ������
	//				��������� ���� ����� ������, � ��������� ����� ������.
	//name		 - ��� ������� �������

	if(pFunction==NULL)
	{//��� ����������� ������� �������� �� ������ ��� ������� ������� ���������
		return false;
	}
	//������� �������, ���� � ��� ���-�� ��������
	pFunction->pPlotXYItems.clear();
	
	//�������� � ����������� �� ��������� ������� 
	int i=0;
	for(i=0; i<=Size/2; i++)
	{//�������� �� ������� ��������� ��������, ������� ������������ ������ ����� �� ������
		float new_value;									
		complex <float> c (Freq_real[i]/Size, Freq_imag[i]/Size);//����������� ������������� ����� (�������� ������ ������� �� ���-�� ���������)
		new_value=abs(c);	//������� ���������
	
		if(i!=0)
		{//������ ���, ����� ������� ���������
			new_value+=new_value;
		}

		//������� ����� � �������
		pFunction->pPlotXYItems.push_back(CDPoint(static_cast<float>(i)*multiplier,new_value));
	}
	
	//������� ��� �������
	pFunction->szFunctionName=name;
	pFunction->ChartType=G_BARCHART_TIGHT;
	pFunction->AlwaysDown=true;
	pFunction->Visible=true;
	//������� ���������, ���������� ����������� ������� �� ���������� ������������

	return true;	//�� ���������
}*/

/********************************	GetFloatPoint(float time)	********************************
*		������ ����� �� ������� �� �������� ��������
************************************************************************************************/
float FFT_GetFloatPoint(FP32* pFreqReal, FP32* pFreqImag, INT16U size_of_sample, INT8U* pFilter, FP32 time)
{
	FP32 point=0;
	//���������� ������������
	if(pFilter[0] & 0x01)
	{
		point=pFreqReal[0]/size_of_sample;
		//� ������ ����� ����� �� ����������� �������
	}

	unsigned char num_byte;	//|
	unsigned char num_bit;	//| ������� ���� ������ ��� ��������� �������
	for(INT16U i=1; i<=size_of_sample/2;i++)
	{
		//������ � ����� ����� ����� ���� ����
		num_byte=i/8;
		//������ � ����� ���� ����� ���� ����
		num_bit=i%8;
		if(pFilter[num_byte] & 1<<num_bit)
		{//��� ��������� ����� ������
			FP32 freq=2*(FP32)MATH_PI*i*time/size_of_sample;	//������� ������� � �����
			point+=2*(pFreqReal[i]/size_of_sample*(FP32)cos(freq)+pFreqImag[i]/size_of_sample*(FP32)sin(freq));	
		}
	}
	return point;	//������ ��� �����
}

/**************************	GetFilteredFunction(FunctionClass *pFunction, int points_count)	*************
*		��� ������� ��������� ������� �� �������, � ������ ����������� ������� � �������� ��������,
*	������������ � ������� Filter. ����������� ����� ���� ������ �������.
*******************************************************************************************************/
/*bool GetFilteredFunction(FunctionClass *pFunction, int points_count, CString name)
{
	//pFunction - ������� � ������� ����� ����������� ������
	//points_count - ���������� �����, ������� ����� ���������� �������

	if(pFunction==NULL)
	{
		m_Error.SetERROR("��� ����������� ������������� ������� �� ������ ��� ������� ������� ���������",5);
		return false;
	}

	pFunction->pPlotXYItems.clear();	//��������
	
	//������, � ����� �������� ��� ����� ����� ����������� �����
	float time_per_point=PeriodOfSamples*Size/points_count;

	float time=0;
	for (int i=0;i<points_count; i++, time+=time_per_point)
	{//����� ��������������� ��������� ��� �����
		pFunction->pPlotXYItems.push_back(CDPoint(time,GetFloatPoint(time)));
	}

	//�������� ������� ����� �������
	pFunction->AlwaysDown=false;
	pFunction->ChartType=G_LINECHART;
	pFunction->szFunctionName=name;
	pFunction->Visible=true;

	return true;	//�� ���������
}*/

/***************************	GetTimeReal(FunctionClass *pFunction,CString name)	***************
*		�������� �������������� ����� ������ ��������� ������� � �������.
**************************************************************************************************/
/*bool GetTimeReal(FunctionClass *pFunction,CString name)
{
	if(pFunction==NULL)
	{
		m_Error.SetERROR("��� �������������� ����� ��������� ������� �� ������ ��� ������� ������� ���������",5);
		return false;
	}

	pFunction->pPlotXYItems.clear();

	float time=0;
	for(int i=0; i<Size; i++, time+=PeriodOfSamples)
	{
		pFunction->pPlotXYItems.push_back(CDPoint(time,Time_real[i]));
	}

	return true;
}*/

/************************    FFT_Nearest2Power    *********************************************
*		���������� ��������� (� ������� �������) ������� ������ ����� �� �����. �����������
*	����� ������ �� ������� ��������� �������� ������ ��� ������� �������������� �����
**********************************************************************************************/
INT16U	FFT_Nearest2Power(INT16U value, INT8U *pWhatPower)
{
	INT16U lCustomarySize=1;				//|
	if(pWhatPower!=NULL) *pWhatPower=0;		//|����������� �������
	while(1)
	{//��������������� ��������� �� ���� �������� ������, ���� �� �������� ��������
		if(lCustomarySize>=value)
		{//���������!
			break;
		}
		lCustomarySize=lCustomarySize<<1;
		if(pWhatPower!=NULL) *pWhatPower++;
	}
	return lCustomarySize;
}