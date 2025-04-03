#pragma once
#include <ERRORS\ERRORS.h>		//журнал регистрации ошибок
#include <Types_Visual_c++.h>	//мои типы данных
#include <vector>
using namespace std;


typedef struct _NONLINEAR_STRUCT {
	float fY;
	float fX;
	float fh;
	float fgammai;
} NONLINEAR_STRUCT;

typedef struct _EGU_STRUCT {
	float fY;
	float fX;
} EGU_STRUCT;


class CVERSION_4Dlg;//объявление класса родительского окна

class GLAVNIY_CLASS_Struct
{
	/*
	ЗАПИСЫВАТЬ ПЕРЕМЕННЫЕ НАПРЯМУЮ КАТЕГОРИЧЕСКИ ЗАПРЕЩАЕТСЯ, Т.К. ОНИ МОГУТ ОДНОВРЕМЕННО ОТОБРАЖАТЬСЯ НА ЭКРАНЕ
	ПОЛЬЗУЙТЕСЬ Add...
	*/

public:
	//	КОНСТРУКТОР - ДЕСТРУКТОР
	GLAVNIY_CLASS_Struct();
	~GLAVNIY_CLASS_Struct();

	//	Функция инициализации
	bool InitGLAVNIY_CLASS(CVERSION_4Dlg * main_window, ERROR_Class *pError);//должна вызываться после присвоения источников

	//класс главного окна
	CVERSION_4Dlg *MAIN_WINDOW;
	
	//все существенные ошибки пишутся сюда
	ERROR_Class *m_pError;		
		
	/**  УСТАВКИ  **/
	FP32	var_UST_Freq_Rot;		//уставка частоты вращения
	
	bool	var_UST_OnLoad;			//режим работы 
									//false - холостой ход
									//true - под нагрузкой
	
	bool	var_UST_Run_Stop;		//false - дизель остановлен
									//true - дизель запущен

	// Добавлено 29.11.2010
	// Вариант протокола с передачей положения рейки. Рейка задается в окне ввода.
	// Расширенный протокол активизируется, а окно показывается при выборе в меню "Расширенный протокол"

	FP32	var_UST_Reyka;			// задание положения рейки
	bool	var_EXT_Protocol;		// активизация протокола и отображение окна ввода рейки

	bool	var_UST_Bux;			// буксование

	/**  ПЕРМЕННЫЕ СОСТОЯНИЯ ОБЪЕКТА  **/
	FP32	var_Freq_Rot;			//истинная частота вращения
	FP32	var_Reyka;				//положение рейки [мм]
	FP32	var_Oil_Temp;			//температура масла [С]
	
	unsigned char	var_Nadduv_Limit;//0x00 - нет ограничения по наддуву
									//0xFE - есть
									//0xEE - вышел из строя датчик давления наддува

	FP32	var_Freq_Turbine;		//обороты турбины
	FP32	var_Nadduv_Pressure;	//давление наддува

	bool	var_Run_Stop;			//true - дизель работает
									//false - дизель остановлен

	bool	var_Raznos;				//true - есть разнос
									//false - нет разноса

	INT8U	var_Oil_Prot;			//защита по маслу
									//0х00 - нет защиты
									//0х40 - сигнализация
									//0хС0 - защита

	FP32 var_Oil_Pressure;		//давление масла




	//Функции записи частоты вращения, перекодировка в тип Лобанова уже внутри
	// dir = true - поменяли значение в окне программы
	// dir = false - где-то еще и в окне нужно обновить
	//Возвращает 
	// true - функция отработала успешно
	// false - при отработке были ошибки, они записаны в ERRORS
	bool Add_UST_Freq_Rot(bool dir, FP32 temp=-1); 
	bool Add_UST_ONLOAD(bool OnNagr);
	bool Add_UST_Run_Stop(bool Run);

	bool Add_Freq_Rot		(bool dir, FP32 temp=-1); 
	bool Add_Reyka			(bool dir, FP32 temp=-1); 
	bool Add_Oil_Temp		(bool dir, FP32 temp=-1); 
	bool Add_Freq_Turbine	(bool dir, FP32 temp=-1); 
	bool Add_Nadduv_Pressure(bool dir, FP32 temp=-1); 
	bool Add_Oil_Prot		(INT8U temp=-1); 
	bool Add_Oil_Pressure	(bool dir, FP32 temp=-1); 
	bool Add_Nadduv_Limit	(INT8U temp=-1);
	bool Add_Run_Stop		(bool Run);
	bool Add_Raznos			(bool dir, bool temp=false);

	/******	 ДОПОЛНИТЕЛЬНЫЕ НАСТРОЙКИ   **************************/
	INT8U m_ReceiveControl;
	const static INT8U RECEIVE_CONTROL_THRESHOLD=3;	//если отправлено это количество безответных
													//запросов, то связь считатеся потерянной
	UINT period_of_transfer;	//период в мс, с которым сообщения будут 
								//отправляться на МПСУ_ТП
	const static UINT_PTR PERIOD_OF_TRANSFER_ID=0x00;	//идентификатор этого таймера

private:
#define GR_NOTINITIALIZED	0x00	//ничего не инициализировано
#define GR_ENABLED			0x01	//построение графика включено
#define GR_DISABLED			0x02	//в график ничего не добавляется
	INT8U m_GraphRegime;			//режим работы графика
public:
	bool SetGraphRegime(INT8U regime);
	INT8U GetGraphRegime();
	

	// Расчет нелинейности
	CString strNumberSensor;

	typedef vector<NONLINEAR_STRUCT, allocator<NONLINEAR_STRUCT>> VECTOR_NONLINEAR;

	VECTOR_NONLINEAR m_VectNonLinear;

	float fXStart;
	float fdX;
	float fA, fB;
	float fhmax;
	float fhmaxp;
	float fhmaxm;
	float fgamma;

	CString time_date;
	void  CalcLinear(void);






	typedef vector<EGU_STRUCT, allocator<EGU_STRUCT>> VECTOR_EGU;
	VECTOR_EGU m_VectEGU;

	float fdXe;	// шаг по Х для ЭГУ
	float fYw; // линейность участка ЭГУ
	float fK;  // значение шага, мм/шаг
	float fH;  // "0" шкалы нагрузка, мм
	float fL;  // расстояние от торца втулки до торца ползуна
	int   Fix_w;	 // индекс выбранного окна

	void  CalcEGU(void);

};
	