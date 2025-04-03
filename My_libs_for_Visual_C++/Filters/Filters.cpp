#include "stdafx.h"
#include "Filters.h"
#include "Math_ex_visual_c++.h"	//для ПИ
#include <math.h>				//математика

/**************************************************************************
*						КОНСТРУКТОР ПО УМОЛЧАНИЮ
**************************************************************************/
CAperiodicLinkRandPeriod::CAperiodicLinkRandPeriod()
{
	Init(1);
}

/**************************************************************************
*							ИНИЦИАЛИЗАЦИЯ
**************************************************************************/
bool CAperiodicLinkRandPeriod::Init(FP32 FreqCutOff_hz, FP32 StartValue)
{
	if(!SetFreqCutOff(FreqCutOff_hz)) return false;
	m_y_1=StartValue;
	return true;
}

/***********************     SetFreqCutOff    ****************************
*		Задает частоту среза фильтра (в герцах)
*************************************************************************/
bool CAperiodicLinkRandPeriod::SetFreqCutOff(FP32 FreqCutOff_hz)
{
	if(FreqCutOff_hz<=0) return false;
	m_T=1.f/FreqCutOff_hz;
	return true;
}

/***********************     Calculate     *****************************
*		Вычисляет выход фильтра на основании входа и периода между
*	предыдущим измерением.
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
*						КОНСТРУКТОР
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
*		Вычисляет выход фильтра на основании входа. Период между измерениями
*	вычисляется с помощью функции GetTickCount().
*		pPeriod - если не NULL, то сюда будет записан период между
*					этим и предыдущим измерениями
***********************************************************************/
FP32 CAperiodicLinkRandPeriodAutoStep::Calculate(FP32 input, INT32U *pPeriod)
{
	INT32U lTickCount=GetTickCount();
	INT32U lDifference=lTickCount-m_PredTime;
	FP32 lResult;
	if(lDifference==0)
	{//похоже, два подряд измерения попали в промежуток в одну миллисекунду
	//и я не могу найти между ними разницу
		//принудительно подставляю 1 мс
		lDifference=1;
	}

	lResult=CAperiodicLinkRandPeriod::Calculate(input,lDifference/1000.f);
	m_PredTime=lTickCount;
	if(pPeriod!=NULL) *pPeriod=lDifference;
	return lResult;
}

/**********************    FreqCalculate    ******************************
*		Эта функция вычисляет фильтрованное значение не на основе входа,
*	а на основе времени до предыдущего измерения. Оч полезна когда идет 
*	частотно-модулированный сигнал.
*		pPeriod - если не NULL, то сюда будет записан период между
*					этим и предыдущим измерениями
*************************************************************************/
FP32 CAperiodicLinkRandPeriodAutoStep::FreqCalculate(INT32U *pPeriod)
{
	INT32U lTickCount=GetTickCount();
	INT32U lDifference=lTickCount-m_PredTime;
	if(lDifference==0)
	{//похоже, два подряд измерения попали в промежуток в одну миллисекунду
	//и я не могу найти между ними разницу
		//принудительно подставляю 1 мс
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
*		Создание коэффициентов КИХ филтра НЧ методом простого взвешивания. 
*	Дает линейную фазовую характеристику. см. Айфичер стр. 398. Всего нечётное
*	количество симметричных коэффициентов. Последовательность:
*		Все частоты приведённые, т.е. находятся внутри выборки, например 2Гц - два
*	колебания синусоиды внутри выборки
*********************************************************************************/
bool GetOptimalPointsCount_LF(INT16U MaxCount, FP32 ThresholdFrequency, FP32 SampleRate, INT16U* pOptimalCount)
{
	//перевожу в круговую частоту
	FP32 lRadianThresholdFreq=2*MATH_PI*ThresholdFrequency;

	//вычислю минимальное количество точек, необходимое для осуществления фильтрации
	//буду читать, что мне нужен как минимум один период sin(n*T*wc) в каждую сторону
	INT16U lMinPointsCount=2/ThresholdFrequency/SampleRate;

	if(lMinPointsCount<4)		//просто из головы (не может быть меньше 10 точек на 2 периода синусоиды)
		return false;			//слишком большой период дискретизации для реализации такого фильтра

	if(lMinPointsCount>MaxCount)
		return false;			//слишком маленький массив для реализации такого фильтра

	INT16U lOptimalPointsCount=lMinPointsCount;
	
	//нужно подобрать, такое количество точек, чтобы урез импульсной характеристики фильтра приходился на обнуление синуса
	//(целое количество периодов), в этом случае эффект Гиббса будет проявляться не так сильно

	lOptimalPointsCount=lMinPointsCount;
	INT16U i=4;		//начну с третьей точки обнуления, т.к. вторя вошла в lMinPointsCount
	do
	{
		INT16U temp=i/ThresholdFrequency/SampleRate;
		if(temp>MaxCount)
			break;							//перевалили за размер массива
		else
			lOptimalPointsCount=temp;		//нет, такое кол-во точек ещё могу уместить
		i+=2;
	}
	while(1);
	if(!(lOptimalPointsCount & 0x1))
		lOptimalPointsCount--;				//в случае чего уменьшаю до нечётного
	
	*pOptimalCount=lOptimalPointsCount;
	return true;
}

/*************************    CreateCorsWeighting_LF    ************************
*		Создает коэффициенты КИХ фильтра нижних частот по указанному размеру
*	и частотным уставкам. МЕТОДОМ ПРОСТОГО ВЗВЕШИВАНИЯ.
*		Если фильтровать по этим коэффициентам, то получается ошибка из-за
*	урезания в идеале бесконечного ряда. 
*	pMas - указатель на массив коэффициентов фильтра
*	Size - размер этого массива
*	ThresholdFrequency - частота среза [Гц]
*	SampleRate - период дискретизации [c]
*	Если фильтр не влазит в такую частоту дискретизации, или такой размер массива,
*	то возвращается ложь.
*		1. Найти идеальную импульсную характеристику 
*		Hd(n)=2*fc*sin(n*T*wc)/(n*T*wc)
*			n - номер коэффициента (м.б. положительные и отрицательные)
*			T - период дискретизации [c]
*			fc - частота среза [Гц]
*			wc - частота среза [рад*с]
********************************************************************************/
bool CreateCorsWeighting_LF(FP32 *pMas,INT16U Size, FP32 ThresholdFrequency, FP32 SampleRate)
{
	if(!(Size & 0x1))
		return false;		//массив урезать не надо, а там нечётное количество точек

	//размер симметричной части
	INT16U lSideSize=(Size-1)>>1;

	//перевожу в круговую частоту
	FP32 lRadianThresholdFreq=2.f*MATH_PI*ThresholdFrequency;

	//средняя точка H(0)=2*fc
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

// w(n)=0.54+0.46*cos(2*pi*n/N) при -(N-2)/2<=n<=(N-2)/2
// 0 в других случаях
bool ProcessHamming(FP32 *pMas,INT16U Size, FP32 ThresholdFrequency, FP32 SampleRate)
{
	INT16U lSideSize=Size>>1;	//ширина симметричной части
	for(INT16S n=-lSideSize; n<=lSideSize; n++)
	{
		pMas[n+lSideSize]*=0.54f+0.46f*cos(2*MATH_PI*n/Size);
	}
	return true;
}
