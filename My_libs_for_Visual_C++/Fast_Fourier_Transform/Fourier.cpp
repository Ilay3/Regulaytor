/*********************************************************************************************************
***********************************************************************************************************
**	Здесь храняться функции для собсно преобразования с типом float
*********************************************************************************************************
*********************************************************************************************************/
#include "stdafx.h"
#include "Fourier.h"		//преобразование Фурье


/*************************	FFT_Transform	*****************************************************
*		Это сердце нашего преобразования.
*		На выходе ядря я получаю два массива pRealOut и pImagOut, в котором находятся КОМПЛЕКСЫ. Только
*	из такоего же состояния доступно обратное преобразование фурье.
*
*	Пока функция работает так, что при обратном БПФ входной спектр амплитуд всегда преобразовывается к двустороннему
*		InverseTransform -	FFT_DIRECT прямое БПФ
*							FFT_INVERSE обратное БПФ
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
    {//размер массива не степень двойки, что является необходимым для этого алгоритма БПФ
        return false;
    }

	if(pRealIn==NULL || pRealOut==NULL || pImagOut==NULL)
	{//один из необходимых массивов равен нулю
		return false;
	}

	if (InverseTransform==FFT_INVERSE)
	{//заказали обратное преобразование Фурье
		/*
		!!!! перенести в что-то похожее на FFT_Process_Inverse!!!
		if(flags==FFT_INV_TWO_SIDED_AMPLITUDE_SPECTRUM_CUTTED)
		{//правильный двусторонний спектр амплитуд,но симметричная часть вырезана
			//для начала восстановим симметричную часть
			//Элементы как-бы сдвинуты, чтобы постоянная составляющая не влияла
			for(INT16U i=1; i<size_of_sample/2; i++)
			{
				pRealIn[i+size_of_sample/2]=pRealIn[size_of_sample/2-i];
			}
		}
		*/
        angle_numerator = - angle_numerator;	//для внутренних нужд алгоритма
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
	return true;	//всё прошло успешно
}

/***************************    FFT_Process    ************************************
*		Функция дополнительно обрабатывает полученные с помощью FFT_TransformComplex
*	комплексное БПФ.
*		Дополнительные опции:
*	Для ИСТИННОГО ДВУСТОРОННЕГО СПЕКТРА АМПЛИТУД: каждый элемент результата поделить на число измерений
*	Для ДВУСТОРОННЕГО СПЕКТРА МОЩНОСТИ: просто возводим в квадрат
*	Для ОДНОСТОРОННЕГО СПЕКТРА МОЩНОСТИ: возводим в квадрат, затем умножаем на 2 все, кроме нулевой гармоники
*	Для СПЕКТРА СРЕДНЕ-КВАДРАТИЧНЫХ ЗНАЧЕНИЙ (СКЗ): из спектра мощности извлечь квадратный корень
**********************************************************************************/
bool FFT_Process(FP32* pRealIn, FP32* pImagIn, INT16U size_of_sample, INT8U Flags)
{
	//нужно поделить на количество точек, чтобы результат совпадал по масштабу (особенность БПФ)
	for (INT16U i=0; i < size_of_sample; i++ )
	{
		pRealIn[i] /= (FP32)size_of_sample;
		pImagIn[i] /= (FP32)size_of_sample;
	}

	if(Flags==FFT_ONE_SIDED_AMPLITUDE_SPECTRUM || Flags==FFT_TWO_SIDED_AMPLITUDE_SPECTRUM)
	{//нужно перевести из комплексов в амплитуду и фазу
		for(INT16U i=0; i<size_of_sample; i++)
		{//пока для понятности я напрасно делаю в 2 раза больше вычислений
			FP32 lReal=pRealIn[i];
			FP32 lImag=pImagIn[i];
			//амплитуда ищется какгипотинуза вектора в комплексной плоскости
			pRealIn[i]=sqrt(lReal*lReal+lImag*lImag);
			//фаза ищется как арктангенс, но ещё нужно учесть углы >PI и <-PI
			FP32 temp=atan(lImag/lReal);
			if(lReal<0)
			{//угол в левой части комплексной плоскости
				temp=-temp;
			}
			//???? здесь еще можно ввести сдвиг на 2PI положительных углов
		}
	}
	if(Flags==FFT_TWO_SIDED_AMPLITUDE_SPECTRUM)
	{//НУЖНО ИСКАТЬ ОДНОСТОРОННИЙ СПЕКТР АМПЛИТУД
		INT16U i;
		for(i=0; i<size_of_sample/2; i++)
		{//первую половину амплитуд удваиваю, фазу не трогаю
			pRealIn[i]*=2.f;
		}
		for(i; i<size_of_sample; i++)
		{//остальное (для наглядности только затираю
			pRealIn[i]=0;
			pImagIn[i]=0;
		}
	}
	
	if(Flags==FFT_MEAN_SQUARE_SPECTRUM)
	{//спектр средне-квадратичных значений
		for(INT16U i=0; i<size_of_sample/2; i++)
		{//Удвоим каждую ненулевую гармонику
			pRealIn[i]=sqrt(pRealIn[i]);
		}
	}
	else if(Flags==FFT_ONE_SIDED_POWER_SPECTRUM)
	{//ОДНОСТОРОННИЙ СПЕКТР МОЩНОСТИ (информация о фазе на выходе отсутствует)
		INT16U i;
		for(i=0; i<size_of_sample/2; i++)
		{//возведенный в квадрат и деленный пополам модуль комплексной точки
			pRealIn[i]=(pRealIn[i]*pRealIn[i]+pImagIn[i]*pImagIn[i])/2.f;
			pImagIn[i]=0;
		}
		for(i; i<size_of_sample; i++)
		{//обнулю остатки
			pRealIn[i]=0;
			pImagIn[i]=0;
		}
	}

	return true;
}

/**********************************************************************************************************
************************************************************************************************************
**		Здесь судя по всему хранятся дополнительные функции
**************************************************************************************************************
***********************************************************************************************************/
#define BITS_PER_WORD   (sizeof(unsigned int) * 8)


bool FFT_IsPowerOfTwo ( unsigned int x )
{
    if ( x < 2 )
        return false;

    if ( x & (x-1) )        //Это крутая формула, которая избавляет от циклов
        return false;

    return true;
}


INT16U FFT_NumberOfBitsNeeded ( unsigned int PowerOfTwo )
{
    unsigned i;

    if ( PowerOfTwo < 2 )
    {//ОШИБКА
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
*		Эта функция настраивает массив действительной части сигнала во временной области. 
*	Из ее особенностей:
*		1. Автоматически подстраивает функцию и ее размер до степени двойки (в большую сторону)
*		2. Может скопировать массив во внутренний буфер, или работать с внешним
*		3. Если размер выборки поменялся, функция автоматически очищает все прежние буферы 
*				и указатели
****************************************************************************************************/
/*bool FFT_SetTime_real(CMassive *pMas, INT16U size_of_sample)
{
	int CustomarySize=1;	//размер будущего массива
	if(size<2)
	{//Размер массива для БПФ должен быть не меньше 2
		return false;
	}

	while(1)
	{//последовательно пройдемся по всем степеням двойки, пока не превысим значения
		CustomarySize=CustomarySize<<1;
		if(CustomarySize>size_of_sample)
		{
			break;
		}
	}

	if(Size==CustomarySize)
	{//выбрка и так уже нужного размера
		return true;
	}

	//расширю вектор до степени двойки
	pMas->resize(CustomarySize);

	return true;	//все нормально
}*/

/*********************************	SetTime_real(CDPoint *mas, int size, float period)	*************************
*		Аналогична предыдущей, но входным параметром является массив точек,  содержащих время.
*	Сделана просто для удобства. Естесственно здесь всегда будет копирование во внутренний буфер.
*	Т.к. задано еще и время в каждой точке, то можно попутно вычислить и время дискретизации
*	(алгоритм там примитивный).
*****************************************************************************************************/
/*bool FFT_SetTime_real(CDPoint *mas, int size)					
{
	// mas - указатель на массив из полноценных точек
	// size - размер этого массива
	int CustomarySize=1;	//размер будущего массива
	if(size<2)
	{//Размер массива для БПФ должен быть не меньше 2
		return false;
	}

	if(IsPowerOfTwo(size))
	{//просто умный пользователь уже заранее подготовит корректные данные
		CustomarySize=size;
	}
	else
	{//это не степень двойки, поэтому автоматически расширим массив до степени
		while(1)
		{//последовательно пройдемся по всем степеням двойки, пока не превысим значения
			CustomarySize=CustomarySize<<1;
			if(CustomarySize>size)
			{
				break;
			}
		}
	}

	if(Size!=CustomarySize)
	{//Размер выборок предыдущих преобразований не совпадает
		//Значится так, в этом случае мы уже не сможем работать
		//с прежними массивами (размер-то не тот), поэтому
		//придется из отнять у родителя (если массив внешний), или
		//удалить!
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
		Size=CustomarySize;//присвоим размер глобальной переменной
	}
	
	//скопируем массив во временный буфер
	Time_real= new float [CustomarySize];	//выделим память под новый массив
	
	int i=0;
	for(;i<size;i++)
	{//перекопируем основную часть массива
		Time_real[i]=mas[i].y;
	}
	//дальше будем рулить той частью выборки, которой не достало, при увеличении до степени двойки
	for(;i<CustomarySize;i++)
	{//а недостающие элементы перенесем из последнего элемента (что в общем не совсем правильно)
		Time_real[i]=Time_real[i-1];
	}

	//ну а теперь, коль уж нам предоставилась возможность - сразу зададим и время дискретизации
	PeriodOfSamples=(mas[size-1].x-mas[0].x)/static_cast<float>(size);	//всё, дальше углубляться не будем
	return true;	//все нормально
}*/

/**************************		GetPowerDensitySpectrum(FunctionClass *pFunction)	****************************
*	Выдает спектр мощности, пока только односторонний, поскольку при двустороннем возникает путаница
*	как его отображать на экране
****************************************************************************************************************/
/*bool FFT_GetPowerDensitySpectrum(FunctionClass *pFunction,CString name, float multiplier)	
{
	//multiplier - растягивает спектр по оси Х. Например его можно подобрать так, чтобы первая
	//				гармоника была слева экрана, а последняя точно справа.
	//name		 - имя будущей функции

	if(pFunction==NULL)
	{//Для отображения спектра мощности на экране был передан нулевой указатель
		return false;
	}
	//Очистим функцию, если в ней что-то осталось
	pFunction->pPlotXYItems.clear();
	
	//перейдем к нормальному по амплитуде спектру 
	int i=0;
	for(i=0; i<=Size/2; i++)
	{//пройдёмся по каждому значащему элементу, оставив симметричную правую часть за бортом
		float new_value=Freq_real[i]/Size;	//нормализуем (Разделим каждый элемент на кол-во измерений)
		new_value*=new_value;						//возведем в квадрат
		if(i!=0)
		{//удвоим все, кроме нулевой гармоники
			new_value+=new_value;
		}

		//добавим точку в функцию
		pFunction->pPlotXYItems.push_back(CDPoint(static_cast<float>(i)*multiplier,new_value));
	}
	
	//внешний вид функции
	pFunction->szFunctionName=name;
	pFunction->ChartType=G_BARCHART_TIGHT;
	pFunction->AlwaysDown=true;
	pFunction->Visible=true;
	//единицу измерения, масштабный коэффициент оставим на усмотрение пользователя

	return true;	//всё нормально
}*/

/**************************		GetAmplitudeSpectrum(FunctionClass *pFunction)	****************************
*	Выдает амплитудный мощности, пока только односторонний, поскольку при двустороннем возникает путаница
*	как его отображать на экране
****************************************************************************************************************/
/*bool FFT_GetAmplitudeSpectrum(FunctionClass *pFunction,CString name, float multiplier)	
{
	//multiplier - растягивает спектр по оси Х. Например его можно подобрать так, чтобы первая
	//				гармоника была слева экрана, а последняя точно справа.
	//name		 - имя будущей функции

	if(pFunction==NULL)
	{//Для отображения спектра мощности на экране был передан нулевой указатель
		return false;
	}
	//Очистим функцию, если в ней что-то осталось
	pFunction->pPlotXYItems.clear();
	
	//перейдем к нормальному по амплитуде спектру 
	int i=0;
	for(i=0; i<=Size/2; i++)
	{//пройдёмся по каждому значащему элементу, оставив симметричную правую часть за бортом
		float new_value;									
		complex <float> c (Freq_real[i]/Size, Freq_imag[i]/Size);//нормализуем действитеьную часть (Разделим каждый элемент на кол-во измерений)
		new_value=abs(c);	//получим амплитуду
	
		if(i!=0)
		{//удвоим все, кроме нулевой гармоники
			new_value+=new_value;
		}

		//добавим точку в функцию
		pFunction->pPlotXYItems.push_back(CDPoint(static_cast<float>(i)*multiplier,new_value));
	}
	
	//внешний вид функции
	pFunction->szFunctionName=name;
	pFunction->ChartType=G_BARCHART_TIGHT;
	pFunction->AlwaysDown=true;
	pFunction->Visible=true;
	//единицу измерения, масштабный коэффициент оставим на усмотрение пользователя

	return true;	//всё нормально
}*/

/********************************	GetFloatPoint(float time)	********************************
*		Выдает точку во времени от заданных спектров
************************************************************************************************/
float FFT_GetFloatPoint(FP32* pFreqReal, FP32* pFreqImag, INT16U size_of_sample, INT8U* pFilter, FP32 time)
{
	FP32 point=0;
	//постоянная составляющая
	if(pFilter[0] & 0x01)
	{
		point=pFreqReal[0]/size_of_sample;
		//а мнимая часть здесь по определению нулевая
	}

	unsigned char num_byte;	//|
	unsigned char num_bit;	//| вынесем сюда только для удосбства отладки
	for(INT16U i=1; i<=size_of_sample/2;i++)
	{
		//узнаем в каком байте лежит этот флаг
		num_byte=i/8;
		//узнаем в каком бите лежит этот флаг
		num_bit=i%8;
		if(pFilter[num_byte] & 1<<num_bit)
		{//эту гармонику нужно учесть
			FP32 freq=2*(FP32)MATH_PI*i*time/size_of_sample;	//текущая частота и время
			point+=2*(pFreqReal[i]/size_of_sample*(FP32)cos(freq)+pFreqImag[i]/size_of_sample*(FP32)sin(freq));	
		}
	}
	return point;	//вернем эту точку
}

/**************************	GetFilteredFunction(FunctionClass *pFunction, int points_count)	*************
*		Эта функция заполняет функцию на графике, с учетом полученного спектра и составом гармоник,
*	определенном в массиве Filter. Заполняется ровно один период сигнала.
*******************************************************************************************************/
/*bool GetFilteredFunction(FunctionClass *pFunction, int points_count, CString name)
{
	//pFunction - функция в которую будут заполняться данные
	//points_count - количество точек, которое будет определять функцию

	if(pFunction==NULL)
	{
		m_Error.SetERROR("Для отображения фильтрованной функции на экране был передан нулевой указатель",5);
		return false;
	}

	pFunction->pPlotXYItems.clear();	//отчистим
	
	//узнаем, с какой частотой нам нужно будет проставлять точки
	float time_per_point=PeriodOfSamples*Size/points_count;

	float time=0;
	for (int i=0;i<points_count; i++, time+=time_per_point)
	{//будем последовательно вычислять все точки
		pFunction->pPlotXYItems.push_back(CDPoint(time,GetFloatPoint(time)));
	}

	//займемся внешним видом функции
	pFunction->AlwaysDown=false;
	pFunction->ChartType=G_LINECHART;
	pFunction->szFunctionName=name;
	pFunction->Visible=true;

	return true;	//всё нормально
}*/

/***************************	GetTimeReal(FunctionClass *pFunction,CString name)	***************
*		Копирует действительную часть данных временной области в функцию.
**************************************************************************************************/
/*bool GetTimeReal(FunctionClass *pFunction,CString name)
{
	if(pFunction==NULL)
	{
		m_Error.SetERROR("Для действительной части временной области на экране был передан нулевой указатель",5);
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
*		Возвращает ближайшую (в большую сторону) степень двойки числа на входе. Применяется
*	чтобы узнать до скольки расширять исходный массив при быстром преобразовании Фурье
**********************************************************************************************/
INT16U	FFT_Nearest2Power(INT16U value, INT8U *pWhatPower)
{
	INT16U lCustomarySize=1;				//|
	if(pWhatPower!=NULL) *pWhatPower=0;		//|изначальная степень
	while(1)
	{//последовательно пройдемся по всем степеням двойки, пока не превысим значения
		if(lCustomarySize>=value)
		{//сработало!
			break;
		}
		lCustomarySize=lCustomarySize<<1;
		if(pWhatPower!=NULL) *pWhatPower++;
	}
	return lCustomarySize;
}