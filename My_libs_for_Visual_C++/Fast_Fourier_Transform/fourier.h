/*************************************************************************************************
**************************************************************************************************
**	Этот модуль содержит функции для прямого и обратного дискретного быстрого преобразования Фурье. 
**	Оставлены разные функции при работе с типом float и double (вдруг понадобится особая точность).
**	Можно было бы сделать всё по современному (с использование complex), однако оставим хоть лучик
**	переносимости на контроллер.
**	ВВиду последних совершенствований. Этот модуль может работать как со внешними массивами данных
**	(ну чтобы лишний раз не копировать),	так и со внутренними (когда формат внешних отличается).
**	Идеология такова: сначала мы подготавливаем все внутренние переменные (настраиваем массивы), а
**	потом вызываем функции, куда уже не надо передавать вагон аргументов. Эти функции работая с вну-
**	тренними данными либо непосредственно возвращают результат, либо также меняют данные.
**************************************************************************************************
***************************************************************************************************/

/******************************	ЛИСТ РЕГИСТРАЦИИ ИЗМЕНЕНИЙ		************************
*	27.02.07 - создан
*	28.02.07 - все функции запиханы в класс Fourier_Class, для более широких возможностей
*				(например для обработки ошибок). Функциональность конкретно расширена, но
*				осталось много места под то что предстоит дописать
**************************************************************************************************/

#pragma once //защитимся от повторного включения
#include <math.h>
#include <complex>

#include "types_visual_c++.h"				//мои типы данных
#include "ERRORS\ERRORS.h"					//для обработки ошибок
#include "FunctionClass\FunctionClass.h"	//чтобы облегчить воспроизведение спектров на экране
#include "Math_ex_visual_c++.h"				//для ПИ


//***********	Константы для функции fft_float	****************************
	//аргумент InverseTransform
#define FFT_DIRECT	0	//нужно прямое преобразоване Фурье
#define FFT_INVERSE 1	//нужно обратное преобразование Фурье

// аргумент flags функции FFT_Transform
//ПРЯМОЕ ПРЕОБРАЗВАНИЕ ФУРЬЕ: что мы хотим получить на выходе?
#define FFT_DO_NONE							0x00 // двусторонний спектр амплитуд, даже не деленный на количество измерений
#define FFT_ONE_SIDED_AMPLITUDE_SPECTRUM	0x01 // односторонний спектр амплитуд
#define FFT_TWO_SIDED_AMPLITUDE_SPECTRUM	0x02 // правильный двусторонний спектр амплитуд
#define FFT_ONE_SIDED_POWER_SPECTRUM		0x03 // односторонний спектр мощности 
#define FFT_TWO_SIDED_POWER_SPECTRUM		0x04 // двусторонний спектр мощности
#define FFT_MEAN_SQUARE_SPECTRUM			0x05 // спектро средне - квадратичных значений
		// остальное - НЕ КАТИТ
		// В случаях 2 и 3 мнимые части обнуляются, чтобы не вносить путаницу, т.к. не несут никакой информации
//ОБРАТНОЕ ПРЕОБРАЗОВАНИЕ ФУРЬЕ: в каком формате входные данные
#define FFT_INV_DO_NONE								0x00 // двусторонний спектр амплитуд, даже не деленный на количество измерений
#define FFT_INV_TWO_SIDED_AMPLITUDE_SPECTRUM		0x01 // правильный двусторонний спектр амплитуд
#define FFT_INV_TWO_SIDED_AMPLITUDE_SPECTRUM_CUTTED 0x02 // правильный двусторонний спектр амплитуд, но симметричная часть вырезана
		// остальное - НЕ КАТИТ

/******************	Задекларируем функции	******************************************/
/*
//для работы с двойной точностью
void fft_double (
    unsigned int  NumSamples,     // размер входного массива, должно быть степенью двойки
    unsigned char InverseTransform, // флаг, 0 - прямое БПФ, 1 - обратное БПФ
    double   *RealIn,              	// массив входных вещественных значений
    double   *ImaginaryIn,         	// массив входных комплексных значений, если NULL, то все нулевые
    double   *RealOut,             	// массив выходных вещественных значений
    double   *ImaginaryOut );      // массив выходных комплексных значений
*/

bool	FFT_IsPowerOfTwo ( unsigned int x );										//проверяет, является ли число степенью двойки
INT16U	FFT_NumberOfBitsNeeded ( unsigned int PowerOfTwo );
INT16U	FFT_ReverseBits ( unsigned int index, unsigned int NumBits );
FP32	FFT_Index_to_frequency ( unsigned int NumSamples, unsigned int Index );	
//bool	FFT_SetTime_real(float *mas, int size, bool copy=true);	//Переносит действительный временной массив снаружи во внутренний
//bool	FFT_SetTime_real(CDPoint *mas, int size);					//То же самое, но работает с массивом, где еще есть время
//bool	FFT_GetTimeReal(FunctionClass *pFunction,CString name);	//Копирует данные из массива в функцию
float	FFT_GetFloatPoint(float time);	//выдает точку во времени от заданных спектров
//bool	FFT_GetFilteredFunction(FunctionClass *pFunction, int points_count, CString name);	//заполняет фильтрованную функцию
//основная рульная функция для преобразования фурье
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
{//описания ВСЕХ функций даны  в срр файле
	//вспомогательные функции
	
	
public:
	float* Time_real;	//массив для действительных значений во временной области
	bool Time_real_extern;	
	float* Time_imag;	//массив для мнимых значений во временной области
	bool Time_imag_extern;
	float* Freq_real;	//массив для вещественных значений в частотной области
	bool Freq_real_extern;
	float* Freq_imag;	//массив для мнимых значений в частотной области
	bool Freq_imag_extern;
	
	float PeriodOfSamples;//период между дискретными измерениями

	
	

	bool GetPowerDensitySpectrum(FunctionClass *pFunction, CString name, float multiplier=1);		//выдает спектр мощности
	bool GetAmplitudeSpectrum(FunctionClass *pFunction, CString name, float multiplier=1);			//выдает амплитудный спектр

	
	unsigned char Filter[20];	//пока статический массив, который будет отображать, какие гармоники 
								//учитывать при отборжении функции от преобразования Фурье.
								//каждый последоватьльный бит - очередная гармоника (начиная с нулевой)
								// 1 - гармоника не фильтруется
								// 0 - гармоника фильтруется
	
	Fourier_Class();	//конструктор
	~Fourier_Class();	//деструктор
	
	int Size;			//размер выборки
	//float GivePoint(float *RealIn, float *RealOut, float time);
};*/







